#include "pch.h"
#include "MindEyeSoundEnv.h"

/**
 * 构造声音环境.
 *
 * \param GroupIDIn：声音环境UID
 * \param pXAudio2In：XAudio引擎
 * \param pMasterVoiceIn：主音频
 * \param WFXIn：主音频格式
 * \param getEmitterInfoIn：获取音频发生器信息函数
 * \param getListenerInfoIn：获取音频接收器信息函数
 */
MindEyeSoundEnv::MindEyeSoundEnv(
    DWORD GroupIDIn,
    IXAudio2 *pXAudio2In,
    IXAudio2SubmixVoice *pMasterVoiceIn,
    WAVEFORMATEXTENSIBLE WFXIn,
    std::set<EmitterInfoDto> (*getEmitterInfoIn)(),
    std::set<ListenerInfoDto> (*getListenerInfoIn)())
{
    // 初始化UID
    GroupID = GroupIDIn;
    EmitterUID = 0;
    ListenerUID = 0;

    // 配置主音频
    pXAudio2 = pXAudio2In;
    pMasterVoice = pMasterVoiceIn;
    WFX = WFXIn;

    // 创建左右声道音频
    pSubmixVoiceLeft = NULL;
    pXAudio2->CreateSubmixVoice(
        &pSubmixVoiceLeft, 1, WFX.Format.nSamplesPerSec,
        0, 254);
    pSubmixVoiceRight = NULL;
    pXAudio2->CreateSubmixVoice(
        &pSubmixVoiceRight, 1, WFX.Format.nSamplesPerSec,
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
    getEmitterInfo = getEmitterInfoIn;
    getListenerInfo = getListenerInfoIn;
}

MindEyeSoundEnv::~MindEyeSoundEnv()
{
}

/**
 * 刷新声音环境.
 *
 * \return
 */
HRESULT MindEyeSoundEnv::OnRefresh()
{
    // 最新发生器信息
    std::set<EmitterInfoDto> emitterInfos = getEmitterInfo();
    auto pEmitter = emitters.begin();

    // 删除多余的发生器
    while (pEmitter != emitters.end())
    {
        MindEyeEmitter emitter = *pEmitter;
        EmitterInfoDto emitterInfo = EmitterInfoDto(emitter.getID());
        if (emitterInfos.find(emitterInfo) == emitterInfos.end())
        {
            emitters.erase(pEmitter);
            delete &emitter;
        }
        pEmitter++;
    }

    // 新增/更新发生器
    auto pEmitterInfo = emitterInfos.begin();
    while (pEmitterInfo != emitterInfos.end())
    {
        EmitterInfoDto emitterInfo = *pEmitterInfo;
        pEmitter = emitters.find(emitterInfo);
        if (pEmitter == emitters.end())
        {
            // 新增发生器
            MindEyeEmitter emitter = *(new MindEyeEmitter(emitterInfo));
            emitters.insert(emitter);
        }
        else
        {
            // 更新发生器
            MindEyeEmitter emitter = *pEmitter;
            emitter.onUpdate(emitterInfo.spaceInfo);
        }
        pEmitterInfo++;
    }

    // 获取最新接收器信息
    std::set<ListenerInfoDto> listenerInfos = getListenerInfo();
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

BOOL MindEyeSoundEnv::operator==(const MindEyeSoundEnv &env) const
{
    return GroupID == env.GroupID;
}

BOOL MindEyeSoundEnv::operator<(const MindEyeSoundEnv &env) const
{
    return GroupID < env.GroupID;
}

BOOL MindEyeSoundEnv::operator>(const MindEyeSoundEnv &env) const
{
    return GroupID > env.GroupID;
}
