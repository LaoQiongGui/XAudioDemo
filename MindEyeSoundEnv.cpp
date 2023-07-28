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
    HRESULT hr = S_OK;

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

    sprintf_s(logStr, MAX_PATH, "pXAudio2: %p", MindEyeSoundEnv::pXAudio2);
    logger->info(logStr);
    sprintf_s(logStr, MAX_PATH, "pMasterVoice: %p", MindEyeSoundEnv::pMasterVoice);
    logger->info(logStr);

    // 创建左右声道音频
    pSubmixVoiceLeft = NULL;
    hr = pXAudio2->CreateSubmixVoice(
        &pSubmixVoiceLeft, 1, WFXSrc.nSamplesPerSec,
        0, 254);
    sprintf_s(logStr, MAX_PATH, "pSubmixVoiceLeft: %p", MindEyeSoundEnv::pSubmixVoiceLeft);
    logger->info(logStr);
    ASSERT(hr == S_OK);

    pSubmixVoiceRight = NULL;
    hr = pXAudio2->CreateSubmixVoice(
        &pSubmixVoiceRight, 1, WFXSrc.nSamplesPerSec,
        0, 254);
    sprintf_s(logStr, MAX_PATH, "pSubmixVoiceRight: %p", MindEyeSoundEnv::pSubmixVoiceRight);
    logger->info(logStr);
    ASSERT(hr == S_OK);

    // 设置左右声道音频与主音频映射
    XAUDIO2_SEND_DESCRIPTOR send = {0, pMasterVoice};
    XAUDIO2_VOICE_SENDS sendList = {1, &send};
    hr = pSubmixVoiceLeft->SetOutputVoices(&sendList);
    ASSERT(hr == S_OK);
    hr = pSubmixVoiceRight->SetOutputVoices(&sendList);
    ASSERT(hr == S_OK);
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
    HRESULT hr = S_OK;

    DWORD dwError = 0;

    // 获取最新发生器信息
    std::set<MindEyeEmitterInfo> emitterInfos = getEmitterInfo();

    // 获取最新接收器信息
    std::set<MindEyeListenerInfo> listenerInfos = getListenerInfo();

    if (emitterInfos.size() == 0 || listenerInfos.size() == 0)
    {
        logStr = "Empty emitterInfos or listenerInfos.";
        logger->info(logStr);
        return hr;
    }

    // 音效链
    XAUDIO2_EFFECT_DESCRIPTOR effectDesc;

    // 音效参数
    MindEyeRenderer2DParams rendererParams = MindEyeRenderer2DParams();

    // 发生器信息
    auto pEmitterInfo = emitterInfos.begin();
    // 发生器
    auto pEmitter = emitters.begin();
    // 接收器信息
    auto pListener = listenerInfos.begin();

    // 删除多余的发生器
    while (pEmitter != emitters.end())
    {
        MindEyeEmitterInfo emitterInfo = MindEyeEmitterInfo();
        emitterInfo.ID = (*pEmitter).ID;
        if (emitterInfos.find(emitterInfo) == emitterInfos.end())
        {
            emitters.erase(pEmitter);
            delete &(*pEmitter);
        }
        pEmitter++;
    }

    // 新增、更新发生器
    while (pEmitterInfo != emitterInfos.end())
    {
        MindEyeEmitter *pEmitterTmp = new MindEyeEmitter();
        MindEyeEmitterInfo* pEmitterInfoTmp = &MindEyeEmitterInfo(*pEmitterInfo);
        pEmitterTmp->ID = pEmitterInfoTmp->ID;
        pEmitter = emitters.find(*pEmitterTmp);
        if (pEmitter == emitters.end())
        {
            // 新增发生器
            // 设置ID和空间信息
            pEmitterTmp->ID = pEmitterInfoTmp->ID;
            pEmitterTmp->spaceInfo = pEmitterInfoTmp->spaceInfo;
            // 设置缓冲区数据
            pEmitterTmp->buffer.AudioBytes = pEmitterInfoTmp->bufferSize;
            pEmitterTmp->buffer.pAudioData = pEmitterInfoTmp->pBufferData;
            // 设置循环播放
            pEmitterTmp->buffer.LoopCount = XAUDIO2_LOOP_INFINITE;

            // 创建左声道声源
            hr = pXAudio2->CreateSourceVoice(
                &pEmitterTmp->pAudio2SourceVoiceL, &WFXSrc);
            sprintf_s(logStr, MAX_PATH, "pAudio2SourceVoiceL: %p", pEmitterTmp->pAudio2SourceVoiceL);
            logger->info(logStr);
            ASSERT(hr == S_OK);
            // 左声道声源->左声道混音
            pEmitterTmp->sendL.pOutputVoice = pSubmixVoiceLeft;
            pEmitterTmp->sendlistL.SendCount = 1;
            pEmitterTmp->sendlistL.pSends = &pEmitterTmp->sendL;
            // hr = pEmitterTmp->pAudio2SourceVoiceL->SetOutputVoices(&pEmitterTmp->sendlistL);
            ASSERT(hr == S_OK);
            // 创建音效
            MindEyeRenderer2D *pRendererL = NULL;
            MindEyeRenderer2D::CreateInstance(NULL, 0, &pRendererL);
            effectDesc.InitialState = true;
            effectDesc.OutputChannels = 1;
            effectDesc.pEffect = static_cast<IXAPO *>(pRendererL);
            // 设置音效
            pEmitterTmp->effectChainL.EffectCount = 1;
            pEmitterTmp->effectChainL.pEffectDescriptors = &effectDesc;
            hr = pEmitterTmp->pAudio2SourceVoiceL->SetEffectChain(&pEmitterTmp->effectChainL);
            ASSERT(hr == S_OK);
            pRendererL->Release();
            // 设置音效参数
            rendererParams.spaceInfoEmitter = pEmitterTmp->spaceInfo;
            pListener = listenerInfos.begin();
            rendererParams.spaceInfoListener = (*pListener).spaceInfo;
            pListener++;
            hr = pEmitterTmp->pAudio2SourceVoiceL->SetEffectParameters(0, &rendererParams, sizeof MindEyeRenderer2DParams);
            ASSERT(hr == S_OK);

            // 创建右声道声源
            pXAudio2->CreateSourceVoice(
                &pEmitterTmp->pAudio2SourceVoiceR, &WFXSrc);
            // 右声道声源->右声道混音
            pEmitterTmp->sendR.pOutputVoice = pSubmixVoiceLeft;
            pEmitterTmp->sendlistR.SendCount = 1;
            pEmitterTmp->sendlistR.pSends = &pEmitterTmp->sendR;
            // hr = pEmitterTmp->pAudio2SourceVoiceR->SetOutputVoices(&pEmitterTmp->sendlistR);
            ASSERT(hr == S_OK);
            // 创建音效
            MindEyeRenderer2D *pRendererR = NULL;
            MindEyeRenderer2D::CreateInstance(NULL, 0, &pRendererR);
            effectDesc.InitialState = true;
            effectDesc.OutputChannels = 1;
            effectDesc.pEffect = static_cast<IXAPO *>(pRendererR);
            // 设置音效
            pEmitterTmp->effectChainR.EffectCount = 1;
            pEmitterTmp->effectChainR.pEffectDescriptors = &effectDesc;
            hr = pEmitterTmp->pAudio2SourceVoiceR->SetEffectChain(&pEmitterTmp->effectChainR);
            ASSERT(hr == S_OK);
            pRendererR->Release();
            // 设置音效参数
            rendererParams.spaceInfoEmitter = pEmitterTmp->spaceInfo;
            rendererParams.spaceInfoListener = (*pListener).spaceInfo;
            pListener++;
            hr = pEmitterTmp->pAudio2SourceVoiceR->SetEffectParameters(0, &rendererParams, sizeof MindEyeRenderer2DParams);
            dwError = GetLastError();
            ASSERT(hr == S_OK);
            // 加入发生器
            emitters.insert(*pEmitterTmp);
        }
        else
        {
            delete pEmitterTmp;

            // 更新发生器音效参数
            rendererParams.spaceInfoEmitter = (*pEmitter).spaceInfo;
            pListener = listenerInfos.begin();
            // 左声道
            rendererParams.spaceInfoListener = (*pListener).spaceInfo;
            pListener++;
            hr = (*pEmitter).pAudio2SourceVoiceL->SetEffectParameters(0, &rendererParams, sizeof MindEyeRenderer2DParams);

            // 右声道
            rendererParams.spaceInfoListener = (*pListener).spaceInfo;
            pListener++;
            hr = (*pEmitter).pAudio2SourceVoiceR->SetEffectParameters(0, &rendererParams, sizeof MindEyeRenderer2DParams);
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
