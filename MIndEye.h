#pragma once
#include "Common.h"
#include "Dtos.h"
#include "CodeTable.h"

struct CreateWindowInDto
{
	HWND hWnd;
	HANDLE hEvent;
};

class MindEyeFactory
{
public:
	// private:
	std::set<EmitterInfoDto> (*getAmbientEmitterDtoStatus)();
	std::set<EmitterInfoDto> (*getDynamicEmitterDtoStatus)();
	std::set<ListenerInfoDto> (*getDynamicListenerStatus)();
	VOID OnRefresh();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEyeFactory mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	// public:
	MindEyeFactory();
	~MindEyeFactory();

	VOID setAmbientEmitterProc(std::set<EmitterInfoDto> (*getAmbientEmitterDtoStatus)());
	VOID setDynamicEmitterProc(std::set<EmitterInfoDto> (*getDynamicEmitterDtoStatus)());
	VOID setDynamicListenerProc(std::set<ListenerInfoDto> (*getDynamicListenerStatus)());
	HWND getHWndPlayer();

	// protected:
	HANDLE hPlayer;
	HANDLE hMsgSender;
	HWND hWndPlayer;
	IXAudio2 *pXAudio2;
	IXAudio2MasteringVoice *pMasterVoice;
	WAVEFORMATEXTENSIBLE WFX;
	X3DAUDIO_HANDLE X3DInstance;
	// 环境音频发生器
	std::set<EmitterInfoDto> ambientEmitters;
	// 环境音频接收器
	IXAudio2SubmixVoice *pAmbientSubmixVoice;
	// 动态音频发生器
	std::set<EmitterInfoDto> dynamicEmitters;
	// 动态音频接收器
	std::set<ListenerInfoDto> dynamicListeners;
};