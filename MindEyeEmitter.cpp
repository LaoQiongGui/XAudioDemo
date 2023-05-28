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

/**
 * .
 * 
 * \param infoIn：音频ID与音频缓冲
 */
MindEyeEmitter::MindEyeEmitter(EmitterInfoDto infoIn)
{
    UID = infoIn.UID;
    emitterSize = 0;
    // 创建声源缓冲
    buffer = {};
    buffer.AudioBytes = infoIn.bufferSize;
    BYTE *pBufferData = new BYTE[buffer.AudioBytes];
    memcpy_s(pBufferData, buffer.AudioBytes,
             infoIn.pBufferData, buffer.AudioBytes);
    buffer.pAudioData = pBufferData;
    pAudio2SourceVoices = NULL;

    // 初始化三维坐标
    emitter = X3DAUDIO_EMITTER();
    emitter.Position = X3DAUDIO_VECTOR();
    emitter.Velocity = X3DAUDIO_VECTOR();
    refreshFlg = FALSE;
    lastTimestampCouples = NULL;
    currentTimestampCouples = NULL;
}

/**
 * 析构函数.
 * 
 */
MindEyeEmitter::~MindEyeEmitter()
{
    // 停止并销毁所有声源
    for (DWORD i = 0; i < emitterSize; i++) {
        pAudio2SourceVoices[i]->Stop();
        pAudio2SourceVoices[i]->DestroyVoice();
    }

    // 销毁声源数组
    delete[] pAudio2SourceVoices;

    // 销毁时间对
    delete[] lastTimestampCouples;
    delete[] currentTimestampCouples;

    // 销毁声音发出-到达时间对队列
    delete[] pTimestampCouples;

    // 销毁缓冲器
    delete[] buffer.pAudioData;
}

/**
 * 初始化发生器中的声源.
 *
 * \param pXAudio2：XAudio引擎
 * \param dstWFX：音频输出格式
 * \param dwTimeStamp：当前时间戳
 * \return
 */
HRESULT MindEyeEmitter::OnInit(
    IXAudio2 *pXAudio2,
    WAVEFORMATEXTENSIBLE dstWFX,
    DWORD64 dwTimeStamp)
{
    // 禁止重复初始化
    if (emitterSize != 0) {
        return E_ACCESSDENIED;
    }

    HRESULT hr = S_OK;
    // 创建单声道声源格式
    WAVEFORMATEXTENSIBLE srcWFX = dstWFX;
    emitterSize = dstWFX.Format.nChannels;
    srcWFX.Format.nChannels = 1;
    srcWFX.Format.nAvgBytesPerSec = srcWFX.Format.nSamplesPerSec;

    // 初始化声源
    pAudio2SourceVoices =
        new IXAudio2SourceVoice *[dstWFX.Format.nChannels];
    // 初始化时间对
    lastTimestampCouples = new TimestampCouple[emitterSize]();
    currentTimestampCouples = new TimestampCouple[emitterSize]();
    pTimestampCouples = new std::deque<TimestampCouple>[emitterSize]();
    for (DWORD i = 0; i < emitterSize; i++)
    {
        hr = pXAudio2->CreateSourceVoice(
            &pAudio2SourceVoices[i], &srcWFX.Format);
        lastTimestampCouples[i] = TimestampCouple();
        currentTimestampCouples[i] = TimestampCouple();
        pTimestampCouples[i] = std::deque<TimestampCouple>();
    }

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
    // 声音帧刷新逻辑
    OnRefreshPhyInfo();
    OnRefreshVolumes();
    OnRefreshSampleRates();
    return S_OK;
}

/**
 * 更新声源的坐标，速度信息。
 *
 * \return
 */
VOID MindEyeEmitter::OnRefreshPhyInfo()
{
    // TODO
    return VOID();
}

/**
 * 更新全部声源的音量信息。
 *
 * \return
 */
VOID MindEyeEmitter::OnRefreshVolumes()
{
    // TODO
    return VOID();
}

/**
 * 更新全部声源的采样率。
 *
 * \return
 */
VOID MindEyeEmitter::OnRefreshSampleRates()
{
    // TODO
    return VOID();
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
