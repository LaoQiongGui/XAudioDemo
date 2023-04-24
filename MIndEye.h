#pragma once
#include "Common.h"
#include "CodeTable.h"

struct CreateWindowInDto
{
	HWND hWnd;
	HANDLE hEvent;
};

struct SourceVoiceDto
{
	DWORD64 UID;
	XAUDIO2_BUFFER buffer;
	X3DAUDIO_VECTOR vetPos;
	X3DAUDIO_VECTOR vetSpeed;
};

class MindEye
{
private:
	SourceVoiceDto* (*getAmbientSounds)();
	SourceVoiceDto* (*getDynamicSounds)();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEye mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

public:
	MindEye();
	~MindEye();

	VOID setAmbientSoundProc(SourceVoiceDto* (*getAmbientSounds)());
	VOID setDynamicSoundProc(SourceVoiceDto* (*getDynamicSounds)());
	HWND getHWndPlayer();

protected:
	HANDLE hPlayer;
	HANDLE hMsgSender;
	HWND hWndPlayer;
	IXAudio2 *pXAudio2;
	IXAudio2MasteringVoice *pMasterVoice;
	IXAudio2SubmixVoice *pAmbientSubmixVoice;
	IXAudio2SubmixVoice **pSubmixVoiceGroup;
	std::map<SourceVoiceDto, IXAudio2SourceVoice*> ambientSoundMap;
	std::map<SourceVoiceDto, IXAudio2SourceVoice*> dynamicSoundMap;
	WAVEFORMATEXTENSIBLE WFX;
	X3DAUDIO_HANDLE X3DInstance;
	X3DAUDIO_LISTENER *listenerGroup;
};