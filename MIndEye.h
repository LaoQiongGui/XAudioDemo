#pragma once
#include "Common.h"
#include "CodeTable.h"

struct CreateWindowInDto
{
	HWND hWnd;
	HANDLE hEvent;
};

class MindEye
{
private:
	static int APIENTRY createMsgWindow(CreateWindowInDto* pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEye mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

public:
	MindEye();
	~MindEye();

protected:
	HANDLE hPlayer;
	HANDLE hMsgSender;
	HWND hWndPlayer;
	IXAudio2 *pXAudio2;
	IXAudio2MasteringVoice *pMasterVoice;
	IXAudio2SubmixVoice **pSubmixVoiceGroup;
	WAVEFORMATEXTENSIBLE WFX;
	X3DAUDIO_HANDLE X3DInstance;
	X3DAUDIO_LISTENER *listenerGroup;
	LPX3DAUDIO_EMITTER *staticEmitterGroup;
	LPX3DAUDIO_EMITTER *dynamicEmitterGroup;
};