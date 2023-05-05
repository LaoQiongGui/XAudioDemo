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
	std::set<EmitterDto> (*getAmbientEmitterDtoStatus)();
	std::set<EmitterDto> (*getDynamicEmitterDtoStatus)();
	std::set<ListenerDto> (*getDynamicListenerStatus)();
	VOID OnRefresh();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEye mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

public:
	MindEye();
	~MindEye();

	VOID setAmbientEmitterProc(std::set<EmitterDto> (*getAmbientEmitterDtoStatus)());
	VOID setDynamicEmitterProc(std::set<EmitterDto> (*getDynamicEmitterDtoStatus)());
	VOID setDynamicListenerProc(std::set<ListenerDto> (*getDynamicListenerStatus)());
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
	IXAudio2SubmixVoice *pAmbientSubmixVoice;
	// 动态音频发生器
	std::set<EmitterDto> dynamicEmitters;
	// 动态音频接收器
	std::set<ListenerDto> dynamicListeners;
	// 删除音频发生器 入参为待删除的音频发生器
	VOID removeDynamicEmitters(std::set<EmitterDto>);
	// 新增音频发生器 入参为待新增的音频发生器
	VOID addDynamicEmitters(std::set<EmitterDto>);
	// 更新音频发生器状态
	VOID updateDynamicEmitters();
};