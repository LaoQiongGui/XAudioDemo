#pragma once
#include "Common.h"

struct EmitterInfoDto
{
	DWORD64 UID;
	// 音频缓冲区
	UINT32 bufferSize;
	BYTE *pBufferData;
	// 坐标
	FLOAT PX;
	FLOAT PY;
	FLOAT PZ;
	// 速度
	FLOAT VX;
	FLOAT VY;
	FLOAT VZ;

	BOOL operator==(const EmitterInfoDto &emitterDto) const;
	BOOL operator<(const EmitterInfoDto &emitterDto) const;
	BOOL operator>(const EmitterInfoDto &emitterDto) const;
};

struct TimestampCouple
{
	DWORD64 dwTimestamp;
	DWORD64 dwBufferTimestamp;

	TimestampCouple() = default;

	TimestampCouple(const DWORD64 &dwTimestamp, const DWORD64 &dwBufferTimestamp)
		: dwTimestamp(dwTimestamp), dwBufferTimestamp(dwBufferTimestamp)
	{
	}

	TimestampCouple(const DWORD64 dwTimestampIn)
	{
		dwTimestamp = dwTimestampIn;
		dwBufferTimestamp = dwTimestampIn;
	}

	bool operator==(const TimestampCouple &other) const
	{
		return dwTimestamp == other.dwTimestamp && dwBufferTimestamp == other.dwBufferTimestamp;
	}
};

class MindEyeEmitter
{
private:
	DWORD64 UID;
	XAUDIO2_BUFFER buffer;
	IXAudio2SourceVoice *pAudio2SourceVoice;
	X3DAUDIO_EMITTER emitter;
	// 刷新标记：判断当前声音帧中是否已刷新
	BOOL refreshFlg;
	// 上一帧时间对
	TimestampCouple lastTimestampCouple;
	// 当前帧时间对
	TimestampCouple currentTimestampCouple;
	// 声源-缓冲区时间队列
	std::deque<TimestampCouple> timestampCouples;

	// 计算延迟函数
	DWORD CalculateDelay();

protected:
public:
	MindEyeEmitter(EmitterInfoDto infoIn);
	~MindEyeEmitter();

	HRESULT OnInit(IXAudio2 *pXAudio2, WAVEFORMATEXTENSIBLE WFX, DWORD64 dwTimeStamp);
	HRESULT OnRefresh(DWORD64 dwTimeStamp);

	BOOL operator==(const EmitterInfoDto &emitterDto) const;
	BOOL operator<(const EmitterInfoDto &emitterDto) const;
	BOOL operator>(const EmitterInfoDto &emitterDto) const;
	BOOL operator==(const MindEyeEmitter &mindEyeEmitter) const;
	BOOL operator<(const MindEyeEmitter &mindEyeEmitter) const;
	BOOL operator>(const MindEyeEmitter &mindEyeEmitter) const;
};

std::set<EmitterInfoDto> operator-(const std::set<EmitterInfoDto>, const std::set<EmitterInfoDto>);