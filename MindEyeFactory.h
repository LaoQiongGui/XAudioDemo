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
private:
	std::map<DWORD, MindEyeSoundEnv *> MindEyeSoundEnvMap;
	HRESULT OnRefresh();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEyeFactory mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

protected:
	HANDLE hPlayer;
	HANDLE hMsgSender;
	HWND hWndPlayer;

	// 输出音频格式
	WAVEFORMATEX WFX;
	// 主引擎
	IXAudio2 *pXAudio2;
	// 主音频
	IXAudio2MasteringVoice *pMasterVoice;

public:
	MindEyeFactory(WAVEFORMATEX WFX);
	~MindEyeFactory();

	HRESULT addEnv(
		DWORD dwEnvId,
		std::function<std::map<DWORD64, MindEyeEmitterInfo *>(void)> getEmitterInfo,
		std::function<std::map<DWORD64, MindEyeListenerInfo *>(void)> getListenerInfo);
	HRESULT removeEnv(DWORD dwEnvId);

	HWND getHWndPlayer();
};