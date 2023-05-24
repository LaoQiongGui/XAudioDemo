#include "pch.h"
#include "MindEyeSoundEnv.h"

MindEyeSoundEnv::MindEyeSoundEnv(DWORD GroupIDIn, IXAudio2 *pXAudio2In, IXAudio2SubmixVoice *pMasterVoiceIn, WAVEFORMATEXTENSIBLE WFXIn)
{
    GroupID = GroupIDIn;
    EmitterUID = 0;
    ListenerUID = 0;
    pXAudio2 = pXAudio2In;
    pMasterVoice = pMasterVoiceIn;
    pSubmixVoice = NULL;
    WFX = WFXIn;
    XAUDIO2_SEND_DESCRIPTOR send = {0, pMasterVoice};
    XAUDIO2_VOICE_SENDS sendList = {1, &send};
    pXAudio2->CreateSubmixVoice(
        &pSubmixVoice, WFX.Format.nChannels, WFX.Format.nSamplesPerSec,
        0, 254, &sendList);
}

MindEyeSoundEnv::~MindEyeSoundEnv()
{
    auto pMindEyeEmitter = emitters.begin();
    while (pMindEyeEmitter != emitters.end())
    {
        MindEyeEmitter mindEyeEmitter = *pMindEyeEmitter;
        delete &mindEyeEmitter;
        pMindEyeEmitter++;
    }

    auto pMindEyeListener = listeners.begin();
    while (pMindEyeListener != listeners.end())
    {
        MindEyeListener mindEyeListener = *pMindEyeListener;
        delete &mindEyeListener;
        pMindEyeListener++;
    }
    pSubmixVoice->DestroyVoice();
}

HRESULT MindEyeSoundEnv::OnRefresh(DWORD64 dwTimeStamp)
{
    auto pEmitter = emitters.begin();
    while (pEmitter != emitters.end())
    {
        MindEyeEmitter emitter = *pEmitter;
        HRESULT hr = emitter.OnRefresh(dwTimeStamp);
        // 删除未更新的发生器
        if (hr == E_ABORT)
        {
            delete &emitter;
        }
        pEmitter++;
    }
    return S_OK;
}

// 获取环境复杂度，用于多线程优化
DWORD MindEyeSoundEnv::getEnvComplexity()
{
    DWORD dwComplexity = emitters.size() * listeners.size();
    return dwComplexity;
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
