#pragma once
#include "Common.h"
#include "Dtos.h"
#include "CodeTable.h"

struct CreateWindowInDto
{
	HWND hWnd;
	HANDLE hEvent;
};

class MindEye
{
private:
	std::set<EmitterDto> (*getAmbientSounds)();
	std::set<EmitterDto> (*getDynamicSounds)();
	VOID OnRefresh();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEye mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

public:
	MindEye();
	~MindEye();

	VOID setAmbientEmitterProc(std::set<EmitterDto> (*getAmbientSounds)());
	VOID setDynamicEmitterProc(std::set<EmitterDto> (*getDynamicSounds)());
	HWND getHWndPlayer();

protected:
	HANDLE hPlayer;
	HANDLE hMsgSender;
	HWND hWndPlayer;
	IXAudio2 *pXAudio2;
	IXAudio2MasteringVoice *pMasterVoice;
	WAVEFORMATEXTENSIBLE WFX;
	X3DAUDIO_HANDLE X3DInstance;
	// 环境音频发生器
	std::set<EmitterDto> ambientEmitters;
	// 环境音频接收器
	IXAudio2SubmixVoice* pAmbientSubmixVoice;
	// 动态音频发生器
	std::set<EmitterDto> dynamicEmitters;
	// 动态音频接收器
	std::set<ListenerDto> dynamicListeners;
};