#pragma once
#include "Common.h"

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
	DWORD64 ID;
	// 发生器中声源数量
	DWORD emitterSize;
	// 发生器空间信息
	SpaceInfo spaceInfo;
	// 发生器音频缓冲区
	XAUDIO2_BUFFER buffer;
	IXAudio2SourceVoice **pAudio2SourceVoices;
	// 音效
	IUnknown *pRender;

protected:
public:
	MindEyeEmitter(EmitterInfoDto infoIn);
	~MindEyeEmitter();

	// 获取发生器ID
	DWORD64 getID();
	// 更新空间信息
	HRESULT onUpdate(SpaceInfo spaceInfo);

	BOOL operator==(const EmitterInfoDto &emitterDto) const;
	BOOL operator<(const EmitterInfoDto &emitterDto) const;
	BOOL operator>(const EmitterInfoDto &emitterDto) const;
	BOOL operator==(const MindEyeEmitter &mindEyeEmitter) const;
	BOOL operator<(const MindEyeEmitter &mindEyeEmitter) const;
	BOOL operator>(const MindEyeEmitter &mindEyeEmitter) const;
};

std::set<EmitterInfoDto> operator-(const std::set<EmitterInfoDto>, const std::set<EmitterInfoDto>);