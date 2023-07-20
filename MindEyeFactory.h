#pragma once
#include "Common.h"
#include "MindEyeDto.h"
#include "CodeTable.h"
#include "MindEyeSoundEnv.h"

struct CreateWindowInDto
{
	HWND hWnd;
	HANDLE hEvent;
};

class MindEyeFactory
{
public:
	// private:
	std::set<MindEyeEmitterInfo> (*getAmbientEmitterDtoStatus)();
	std::set<MindEyeEmitterInfo> (*getDynamicEmitterDtoStatus)();
	std::set<MindEyeListenerInfo> (*getDynamicListenerStatus)();
	std::set<MindEyeSoundEnv> MindEyeSoundEnvs;
	HRESULT OnRefresh();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEyeFactory mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

	// public:
	MindEyeFactory();
	~MindEyeFactory();

	VOID setAmbientEmitterProc(std::set<MindEyeEmitterInfo> (*getAmbientEmitterDtoStatus)());
	VOID setDynamicEmitterProc(std::set<MindEyeEmitterInfo> (*getDynamicEmitterDtoStatus)());
	VOID setDynamicListenerProc(std::set<MindEyeListenerInfo> (*getDynamicListenerStatus)());
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
	std::set<MindEyeEmitterInfo> ambientEmitters;
	// 环境音频接收器
	IXAudio2SubmixVoice *pAmbientSubmixVoice;
	// 动态音频发生器
	std::set<MindEyeEmitterInfo> dynamicEmitters;
	// 动态音频接收器
	std::set<MindEyeListenerInfo> dynamicListeners;
};