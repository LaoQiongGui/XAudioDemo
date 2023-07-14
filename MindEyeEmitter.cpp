#include "pch.h"
#include "MindEyeEmitter.h"

BOOL EmitterInfoDto::operator==(const EmitterInfoDto &emitterDto) const
{
    return ID == emitterDto.ID;
}

BOOL EmitterInfoDto::operator<(const EmitterInfoDto &emitterDto) const
{
    return ID < emitterDto.ID;
}

BOOL EmitterInfoDto::operator>(const EmitterInfoDto &emitterDto) const
{
    return ID > emitterDto.ID;
}

/**
 * 单UID构造函数.
 *
 * \param UID：UID
 */
EmitterInfoDto::EmitterInfoDto(const DWORD64 UID)
{
    EmitterInfoDto::ID = UID;
    EmitterInfoDto::spaceInfo = SpaceInfo();
    EmitterInfoDto::bufferSize = 0;
    EmitterInfoDto::pBufferData = NULL;
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

/**
 * .
 *
 * \param infoIn：发生器音频信息
 */
MindEyeEmitter::MindEyeEmitter(EmitterInfoDto infoIn)
{
    ID = infoIn.ID;
    emitterSize = 0;
    // 创建声源缓冲
    buffer = {};
    buffer.AudioBytes = infoIn.bufferSize;
    BYTE *pBufferData = new BYTE[buffer.AudioBytes];
    memcpy_s(pBufferData, buffer.AudioBytes,
             infoIn.pBufferData, buffer.AudioBytes);
    buffer.pAudioData = pBufferData;
    pAudio2SourceVoices = NULL;
}

/**
 * 析构函数.
 *
 */
MindEyeEmitter::~MindEyeEmitter()
{
    // 停止并销毁所有声源
    for (DWORD i = 0; i < emitterSize; i++)
    {
        pAudio2SourceVoices[i]->Stop();
        pAudio2SourceVoices[i]->DestroyVoice();
    }

    // 销毁声源数组
    delete[] pAudio2SourceVoices;

    // 销毁缓冲器
    delete[] buffer.pAudioData;
}

/**
 * 获取发生器ID.
 *
 * \return 发生器ID
 */
DWORD64 MindEyeEmitter::getID()
{
    return ID;
}

/**
 * 更新发生器信息.
 *
 * \param infoIn：发生器音频信息
 * \return
 */
HRESULT MindEyeEmitter::onUpdate(SpaceInfo spaceInfo)
{
    // 更新空间信息
    MindEyeEmitter::spaceInfo = spaceInfo;
    return S_OK;
}

BOOL MindEyeEmitter::operator==(const EmitterInfoDto &emitterDto) const
{
    return ID == emitterDto.ID;
}

BOOL MindEyeEmitter::operator<(const EmitterInfoDto &emitterDto) const
{
    return ID < emitterDto.ID;
}

BOOL MindEyeEmitter::operator>(const EmitterInfoDto &emitterDto) const
{
    return ID > emitterDto.ID;
}

BOOL MindEyeEmitter::operator==(const MindEyeEmitter &mindEyeEmitter) const
{
    return ID == mindEyeEmitter.ID;
}

BOOL MindEyeEmitter::operator<(const MindEyeEmitter &mindEyeEmitter) const
{
    return ID < mindEyeEmitter.ID;
}

BOOL MindEyeEmitter::operator>(const MindEyeEmitter &mindEyeEmitter) const
{
    return ID > mindEyeEmitter.ID;
}
