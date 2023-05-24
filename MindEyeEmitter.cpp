#include "pch.h"
#include "MindEyeEmitter.h"

BOOL EmitterInfoDto::operator==(const EmitterInfoDto &emitterDto) const
{
    return UID == emitterDto.UID;
}

BOOL EmitterInfoDto::operator<(const EmitterInfoDto &emitterDto) const
{
    return UID < emitterDto.UID;
}

BOOL EmitterInfoDto::operator>(const EmitterInfoDto &emitterDto) const
{
    return UID > emitterDto.UID;
}

std::set<EmitterInfoDto> operator-(
    const std::set<EmitterInfoDto> set1,
    const std::set<EmitterInfoDto> set2)
{
    std::set<EmitterInfoDto> resultSet = std::set<EmitterInfoDto>();
    auto pEmitterDto1 = set1.begin();
    auto pEmitterDto2 = set2.begin();
    while (
        pEmitterDto1 != set1.end() &&
        pEmitterDto2 != set2.end())
    {
        if (*pEmitterDto1 == *pEmitterDto2)
        {
            pEmitterDto1++;
            pEmitterDto2++;
        }
        else if (*pEmitterDto1 < *pEmitterDto2)
        {
            resultSet.insert(*pEmitterDto1);
            pEmitterDto1++;
        }
        else
        {
            pEmitterDto2++;
        }
    }

    while (pEmitterDto1 != set1.end())
    {
        resultSet.insert(*pEmitterDto1);
        pEmitterDto1++;
    }
    return resultSet;
}

DWORD MindEyeEmitter::CalculateDelay()
{
    // TODO：计算延迟
    return 0;
}

MindEyeEmitter::MindEyeEmitter(EmitterInfoDto infoIn)
{
    UID = infoIn.UID;
    buffer = {};
    buffer.AudioBytes = infoIn.bufferSize;
    BYTE *pBufferData = new BYTE[buffer.AudioBytes];
    memcpy_s(pBufferData, buffer.AudioBytes,
             infoIn.pBufferData, buffer.AudioBytes);
    buffer.pAudioData = pBufferData;
    pAudio2SourceVoice = NULL;
    emitter = {};
    emitter.Position = {infoIn.PX, infoIn.PY, infoIn.PZ};
    emitter.Velocity = {infoIn.VX, infoIn.VY, infoIn.VZ};
    refreshFlg = FALSE;
    lastTimestampCouple = TimestampCouple(0);
    currentTimestampCouple = TimestampCouple(0);
}

MindEyeEmitter::~MindEyeEmitter()
{
    pAudio2SourceVoice->Stop();
    pAudio2SourceVoice->DestroyVoice();
    delete[] buffer.pAudioData;
}

HRESULT MindEyeEmitter::OnInit(
    IXAudio2 *pXAudio2, WAVEFORMATEXTENSIBLE WFX, DWORD64 dwTimeStamp)
{
    lastTimestampCouple.dwTimestamp = dwTimeStamp;
    HRESULT hr = pXAudio2->CreateSourceVoice(
        &pAudio2SourceVoice, (WAVEFORMATEX *)&WFX);
    return hr;
}

HRESULT MindEyeEmitter::OnRefresh(DWORD64 dwTimeStamp)
{

    // 当前帧未刷新的声音返回错误
    if (!refreshFlg)
    {
        return E_ABORT;
    }
    refreshFlg = FALSE;
    // TODO：声音帧刷新逻辑
    return S_OK;
}

BOOL MindEyeEmitter::operator==(const EmitterInfoDto &emitterDto) const
{
    return UID == emitterDto.UID;
}

BOOL MindEyeEmitter::operator<(const EmitterInfoDto &emitterDto) const
{
    return UID < emitterDto.UID;
}

BOOL MindEyeEmitter::operator>(const EmitterInfoDto &emitterDto) const
{
    return UID > emitterDto.UID;
}

BOOL MindEyeEmitter::operator==(const MindEyeEmitter &mindEyeEmitter) const
{
    return UID == mindEyeEmitter.UID;
}

BOOL MindEyeEmitter::operator<(const MindEyeEmitter &mindEyeEmitter) const
{
    return UID < mindEyeEmitter.UID;
}

BOOL MindEyeEmitter::operator>(const MindEyeEmitter &mindEyeEmitter) const
{
    return UID > mindEyeEmitter.UID;
}
