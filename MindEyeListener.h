#pragma once
#include "Common.h"

struct ListenerInfoDto
{
	DWORD64 UID;
	// 坐标
	FLOAT PX;
	FLOAT PY;
	FLOAT PZ;
	// 速度
	FLOAT VX;
	FLOAT VY;
	FLOAT VZ;

	BOOL operator==(const ListenerInfoDto &listenerDto) const;
	BOOL operator<(const ListenerInfoDto &listenerDto) const;
	BOOL operator>(const ListenerInfoDto &listenerDto) const;
};

class MindEyeListener
{
private:
	DWORD64 UID;
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