#pragma once
#include "Common.h"

class MindEyeListener
{
private:
	DWORD64 ID;
	IXAudio2SubmixVoice *pSubmixVoice;
	X3DAUDIO_LISTENER listener;

protected:
public:
	MindEyeListener(ListenerInfoDto infoIn);
	~MindEyeListener();

	HRESULT Init(IXAudio2 *pXAudio2, WAVEFORMATEXTENSIBLE WFX);

	BOOL operator==(const ListenerInfoDto &listenerDto) const;
	BOOL operator<(const ListenerInfoDto &listenerDto) const;
	BOOL operator>(const ListenerInfoDto &listenerDto) const;
	BOOL operator==(const MindEyeListener &mindEyeListener) const;
	BOOL operator<(const MindEyeListener &mindEyeListener) const;
	BOOL operator>(const MindEyeListener &mindEyeListener) const;
};