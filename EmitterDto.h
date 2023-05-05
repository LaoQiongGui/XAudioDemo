#pragma once
#include "Common.h"

struct EmitterDto
{
	DWORD64 UID;
	XAUDIO2_BUFFER buffer;
	IXAudio2SourceVoice* pAudio2SourceVoice;
	X3DAUDIO_VECTOR Position;
	X3DAUDIO_VECTOR Velocity;
	X3DAUDIO_EMITTER emitter;

	BOOL operator==(const EmitterDto& emitterDto) const;
	BOOL operator<(const EmitterDto& emitterDto) const;
	BOOL operator>(const EmitterDto& emitterDto) const;
};