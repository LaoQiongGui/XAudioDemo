#pragma once
#include "Common.h"

struct EmitterInfoDto
{
	DWORD64 UID;
	// 音频缓冲区
	UINT32 bufferSize;
	BYTE* pBufferData;
	// 坐标
	FLOAT PX;
	FLOAT PY;
	FLOAT PZ;
	// 速度
	FLOAT VX;
	FLOAT VY;
	FLOAT VZ;

	BOOL operator==(const EmitterInfoDto& emitterDto) const;
	BOOL operator<(const EmitterInfoDto& emitterDto) const;
	BOOL operator>(const EmitterInfoDto& emitterDto) const;
};

class MindEyeEmitter
{
private:
	DWORD64 UID;
	XAUDIO2_BUFFER buffer;
	IXAudio2SourceVoice *pAudio2SourceVoice;
	X3DAUDIO_EMITTER emitter;

protected:
public:
	MindEyeEmitter(EmitterInfoDto infoIn);
	~MindEyeEmitter();

	HRESULT Init(IXAudio2 *pXAudio2, WAVEFORMATEXTENSIBLE WFX);
	HRESULT UpdateSampleRate(DWORD dwSampleRate);
};

std::set<EmitterInfoDto> operator-(const std::set<EmitterInfoDto>, const std::set<EmitterInfoDto>);