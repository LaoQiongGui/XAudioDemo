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

	TimestampCouple(const DWORD64 dwTimestampIn = 0)
	{
		dwTimestamp = dwTimestampIn;
		dwBufferTimestamp = dwTimestampIn;
	}

	bool operator==(const TimestampCouple &timestampCouple) const
	{
		return dwTimestamp == timestampCouple.dwTimestamp && dwBufferTimestamp == timestampCouple.dwBufferTimestamp;
	}
};

class MindEyeEmitter
{
private:
	DWORD64 UID;
	// 发生器中声源数量
	DWORD emitterSize;
	XAUDIO2_BUFFER buffer;
	IXAudio2SourceVoice **pAudio2SourceVoices;
	X3DAUDIO_EMITTER emitter;
	// 刷新标记：判断当前声音帧中是否已刷新
	BOOL refreshFlg;
	// 上一帧时间对
	TimestampCouple *lastTimestampCouples;
	// 当前帧时间对
	TimestampCouple *currentTimestampCouples;
	// 声音发出-到达时间对队列
	std::deque<TimestampCouple> *pTimestampCouples;

	// 计算延迟函数
	DWORD CalculateDelay();

protected:
public:
	MindEyeEmitter(EmitterInfoDto infoIn);
	~MindEyeEmitter();

	HRESULT OnInit(
		IXAudio2* pXAudio2,
		WAVEFORMATEXTENSIBLE dstWFX,
		DWORD64 dwTimeStamp
	);
	HRESULT OnSetTargets(IXAudio2SubmixVoice *pSubmixVoice, DWORD dwNum);
	HRESULT OnRefresh(DWORD64 dwTimeStamp);
	VOID OnRefreshPhyInfo();
	VOID OnRefreshVolumes();
	VOID OnRefreshSampleRates();

	BOOL operator==(const EmitterInfoDto &emitterDto) const;
	BOOL operator<(const EmitterInfoDto &emitterDto) const;
	BOOL operator>(const EmitterInfoDto &emitterDto) const;
	BOOL operator==(const MindEyeEmitter &mindEyeEmitter) const;
	BOOL operator<(const MindEyeEmitter &mindEyeEmitter) const;
	BOOL operator>(const MindEyeEmitter &mindEyeEmitter) const;
};

std::set<EmitterInfoDto> operator-(const std::set<EmitterInfoDto>, const std::set<EmitterInfoDto>);