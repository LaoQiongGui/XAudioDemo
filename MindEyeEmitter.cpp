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
}

MindEyeEmitter::~MindEyeEmitter()
{
    delete[] buffer.pAudioData;
}

HRESULT MindEyeEmitter::Init(IXAudio2 *pXAudio2, WAVEFORMATEXTENSIBLE WFX)
{
    HRESULT hr = pXAudio2->CreateSourceVoice(
        &pAudio2SourceVoice, (WAVEFORMATEX *)&WFX);
    return hr;
}

HRESULT MindEyeEmitter::UpdateSampleRate(DWORD dwSampleRate)
{
    HRESULT hr = pAudio2SourceVoice->SetSourceSampleRate(dwSampleRate);
    return hr;
}
