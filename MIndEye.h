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
	IXAudio2SourceVoice *pAudio2SourceVoice;

	BOOL operator==(const SourceVoiceDto &sourceVoiceDto) const;
	BOOL operator<(const SourceVoiceDto &sourceVoiceDto) const;
	BOOL operator>(const SourceVoiceDto &sourceVoiceDto) const;
};

std::set<SourceVoiceDto> operator-(const std::set<SourceVoiceDto>& setFirst, const std::set<SourceVoiceDto>& setSecond);

class MindEye
{
private:
	std::set<SourceVoiceDto> (*getAmbientSounds)();
	std::set<SourceVoiceDto> (*getDynamicSounds)();
	VOID OnRefresh();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEye mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

public:
	MindEye();
	~MindEye();

	VOID setAmbientSoundProc(std::set<SourceVoiceDto> (*getAmbientSounds)());
	VOID setDynamicSoundProc(std::set<SourceVoiceDto> (*getDynamicSounds)());
	HWND getHWndPlayer();

protected:
	HANDLE hPlayer;
	HANDLE hMsgSender;
	HWND hWndPlayer;
	IXAudio2 *pXAudio2;
	IXAudio2MasteringVoice *pMasterVoice;
	IXAudio2SubmixVoice *pAmbientSubmixVoice;
	IXAudio2SubmixVoice **pSubmixVoiceGroup;
	std::set<SourceVoiceDto> ambientSoundSet;
	std::set<SourceVoiceDto> dynamicSoundSet;
	WAVEFORMATEXTENSIBLE WFX;
	X3DAUDIO_HANDLE X3DInstance;
	X3DAUDIO_LISTENER *listenerGroup;
};