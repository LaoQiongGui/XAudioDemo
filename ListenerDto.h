#pragma once
#include "Common.h"

struct ListenerDto
{
	DWORD64 UID;
	IXAudio2SubmixVoice *pSubmixVoice;
	X3DAUDIO_VECTOR Position;
	X3DAUDIO_VECTOR Velocity;
	X3DAUDIO_LISTENER listener;

	BOOL operator==(const ListenerDto &listenerDto) const;
	BOOL operator<(const ListenerDto &listenerDto) const;
	BOOL operator>(const ListenerDto &listenerDto) const;
};