#include "pch.h"
#include "MindEyeSoundEnv.h"

/**
 * 构造声音环境.
 *
 * \param pXAudio2：XAudio引擎
 * \param pMasterVoice：主音频
 * \param WFX：主音频格式
 * \param getEmitterInfo：获取音频发生器信息函数
 * \param getListenerInfo：获取音频接收器信息函数
 */
MindEyeSoundEnv::MindEyeSoundEnv(
    IXAudio2 *pXAudio2,
    IXAudio2MasteringVoice *pMasterVoice,
    WAVEFORMATEX WFXOut,
    std::function<std::set<MindEyeEmitterInfo>(void)> getEmitterInfo,
    std::function<std::set<MindEyeListenerInfo>(void)> getListenerInfo)
{
    // 配置主音频
    MindEyeSoundEnv::pXAudio2 = pXAudio2;
    MindEyeSoundEnv::pMasterVoice = pMasterVoice;

    // 设置声源音频格式
    MindEyeSoundEnv::WFXOut = WFXOut;

    // 设置输出音频格式
    MindEyeSoundEnv::WFXSrc = MindEyeSoundEnv::WFXOut;
    MindEyeSoundEnv::WFXSrc.nChannels = 1;
    MindEyeSoundEnv::WFXSrc.nAvgBytesPerSec =
        MindEyeSoundEnv::WFXSrc.nSamplesPerSec * MindEyeSoundEnv::WFXSrc.nBlockAlign;

    // 创建左右声道音频
    pSubmixVoiceLeft = NULL;
    pXAudio2->CreateSubmixVoice(
        &pSubmixVoiceLeft, 1, WFXSrc.nSamplesPerSec,
        0, 254);
    pSubmixVoiceRight = NULL;
    pXAudio2->CreateSubmixVoice(
        &pSubmixVoiceRight, 1, WFXSrc.nSamplesPerSec,
        0, 254);
    // 设置左右声道音频与主音频映射
    XAUDIO2_SEND_DESCRIPTOR send = {0, pMasterVoice};
    XAUDIO2_VOICE_SENDS sendList = {1, &send};
    pSubmixVoiceLeft->SetOutputVoices(&sendList);
    pSubmixVoiceRight->SetOutputVoices(&sendList);
    FLOAT outputMatrix[2] = {};
    outputMatrix[0] = 1.0f;
    outputMatrix[1] = 0.0f;
    pSubmixVoiceLeft->SetOutputMatrix(pMasterVoice, 1, 2, outputMatrix);
    outputMatrix[0] = 0.0f;
    outputMatrix[1] = 1.0f;
    pSubmixVoiceRight->SetOutputMatrix(pMasterVoice, 1, 2, outputMatrix);

    // 设置函数指针
    MindEyeSoundEnv::getEmitterInfo = getEmitterInfo;
    MindEyeSoundEnv::getListenerInfo = getListenerInfo;
}

MindEyeSoundEnv::~MindEyeSoundEnv()
{
    if (pSubmixVoiceLeft != NULL)
    {
        pSubmixVoiceLeft->DestroyVoice();
    }
    if (pSubmixVoiceRight != NULL)
    {
        pSubmixVoiceRight->DestroyVoice();
    }
}

/**
 * 刷新声音环境.
 *
 * \return
 */
HRESULT MindEyeSoundEnv::OnRefresh()
{
    // 获取最新发生器信息
    std::set<MindEyeEmitterInfo> emitterInfos = getEmitterInfo();

    // 获取最新接收器信息
    std::set<MindEyeListenerInfo> listenerInfos = getListenerInfo();

    // 音效链
    XAUDIO2_EFFECT_DESCRIPTOR effectDesc;

    // 音效参数
    MindEyeRenderer2DParams *pParam = NULL;

    // 发生器信息
    auto pEmitterInfo = emitterInfos.begin();
    // 发生器
    auto pEmitter = emitters.begin();
    // 接收器信息
    auto pListener = listenerInfos.begin();

    // 删除多余的发生器
    while (pEmitter != emitters.end())
    {
        MindEyeEmitter emitter = *pEmitter;
        MindEyeEmitterInfo emitterInfo = MindEyeEmitterInfo();
        emitterInfo.ID = emitter.ID;
        if (emitterInfos.find(emitterInfo) == emitterInfos.end())
        {
            emitters.erase(pEmitter);
            delete &emitter;
        }
        pEmitter++;
    }

    // 新增、更新发生器
    while (pEmitterInfo != emitterInfos.end())
    {
        MindEyeEmitter emitterTmp = MindEyeEmitter();
        MindEyeEmitterInfo emitterInfoTmp = *pEmitterInfo;
        emitterTmp.ID = emitterInfoTmp.ID;
        pEmitter = emitters.find(emitterTmp);
        if (pEmitter == emitters.end())
        {
            // 新增发生器
            // 设置ID和空间信息
            emitterTmp.ID = emitterInfoTmp.ID;
            emitterTmp.spaceInfo = emitterInfoTmp.spaceInfo;
            // 设置缓冲区数据
            emitterTmp.buffer.AudioBytes = emitterInfoTmp.bufferSize;
            emitterTmp.buffer.pAudioData = emitterInfoTmp.pBufferData;
            // 设置循环播放
            emitterTmp.buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

            // 创建左声道声源
            pXAudio2->CreateSourceVoice(
                &emitterTmp.pAudio2SourceVoiceL, &WFXSrc);
            // 左声道声源->左声道混音
            emitterTmp.sendL.pOutputVoice = pSubmixVoiceLeft;
            emitterTmp.sendlistL.SendCount = 1;
            emitterTmp.sendlistL.pSends = &emitterTmp.sendL;
            emitterTmp.pAudio2SourceVoiceL->SetOutputVoices(&emitterTmp.sendlistL);
            // 创建音效
            MindEyeRenderer2D *pRendererL = NULL;
            MindEyeRenderer2D::CreateInstance(NULL, 0, &pRendererL);
            effectDesc.InitialState = true;
            effectDesc.OutputChannels = 1;
            effectDesc.pEffect = static_cast<IXAPO *>(pRendererL);
            // 设置音效
            emitterTmp.effectChainL.EffectCount = 1;
            emitterTmp.effectChainL.pEffectDescriptors = &effectDesc;
            emitterTmp.pAudio2SourceVoiceL->SetEffectChain(&emitterTmp.effectChainL);
            pRendererL->Release();
            // 设置音效参数
            pParam = new MindEyeRenderer2DParams();
            pParam->spaceInfoEmitter = emitterTmp.spaceInfo;
            pListener = listenerInfos.begin();
            pParam->spaceInfoListener = (*pListener).spaceInfo;
            pListener++;
            emitterTmp.pParamL = pParam;
            emitterTmp.pAudio2SourceVoiceL->SetEffectParameters(0, emitterTmp.pParamL, sizeof MindEyeRenderer2DParams);

            // 创建右声道声源
            pXAudio2->CreateSourceVoice(
                &emitterTmp.pAudio2SourceVoiceR, &WFXSrc);
            // 右声道声源->右声道混音
            emitterTmp.sendR.pOutputVoice = pSubmixVoiceLeft;
            emitterTmp.sendlistR.SendCount = 1;
            emitterTmp.sendlistR.pSends = &emitterTmp.sendR;
            emitterTmp.pAudio2SourceVoiceR->SetOutputVoices(&emitterTmp.sendlistR);
            // 创建音效
            MindEyeRenderer2D *pRendererR = NULL;
            MindEyeRenderer2D::CreateInstance(NULL, 0, &pRendererR);
            effectDesc.InitialState = true;
            effectDesc.OutputChannels = 1;
            effectDesc.pEffect = static_cast<IXAPO *>(pRendererR);
            // 设置音效
            emitterTmp.effectChainR.EffectCount = 1;
            emitterTmp.effectChainR.pEffectDescriptors = &effectDesc;
            emitterTmp.pAudio2SourceVoiceL->SetEffectChain(&emitterTmp.effectChainR);
            pRendererR->Release();
            // 设置音效参数
            pParam = new MindEyeRenderer2DParams();
            pParam->spaceInfoEmitter = emitterTmp.spaceInfo;
            pParam->spaceInfoListener = (*pListener).spaceInfo;
            pListener++;
            emitterTmp.pParamR = pParam;
            emitterTmp.pAudio2SourceVoiceR->SetEffectParameters(0, emitterTmp.pParamR, sizeof MindEyeRenderer2DParams);
            // 加入发生器
            emitters.insert(emitterTmp);
        }
        else
        {
            // 更新发生器音效参数
            // 左声道
            pParam = (MindEyeRenderer2DParams *)((*pEmitter).pParamL);
            pParam->spaceInfoEmitter = emitterInfoTmp.spaceInfo;
            pListener = listenerInfos.begin();
            pParam->spaceInfoListener = (*pListener).spaceInfo;
            pListener++;

            // 右声道
            pParam = (MindEyeRenderer2DParams *)((*pEmitter).pParamR);
            pParam->spaceInfoEmitter = emitterInfoTmp.spaceInfo;
            pParam->spaceInfoListener = (*pListener).spaceInfo;
        }
        pEmitterInfo++;
    }
    return S_OK;
}

/**
 * 获取环境复杂度，用于多线程优化.
 *
 * \return ：声音环境复杂度
 */
DWORD MindEyeSoundEnv::getEnvComplexity()
{
    DWORD dwEmitterNum = (DWORD)emitters.size();
    DWORD dwListenerNum = (DWORD)listeners.size();
    return dwEmitterNum * dwListenerNum;
}
