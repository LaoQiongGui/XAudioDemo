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
	std::map<DWORD, MindEyeSoundEnv> MindEyeSoundEnvMap;
	HRESULT OnRefresh();

	static int APIENTRY createMsgWindow(CreateWindowInDto *pCreateWindowInDto);
	static int APIENTRY createMsgSender(MindEyeFactory mindEye);
	static ATOM registAudioPlayerClass(HINSTANCE hInstance);
	static LRESULT CALLBACK AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam);

public:
	MindEyeFactory();
	~MindEyeFactory();

	HRESULT addEnv(
		DWORD dwEnvId,
		std::function<std::set<MindEyeEmitterInfo>(void)> getEmitterInfo,
		std::function<std::set<MindEyeListenerInfo>(void)> getListenerInfo);
	HRESULT removeEnv(DWORD dwEnvId);

	HWND getHWndPlayer();

	// protected:
	HANDLE hPlayer;
	HANDLE hMsgSender;
	HWND hWndPlayer;
	IXAudio2 *pXAudio2;
	IXAudio2MasteringVoice *pMasterVoice;
	WAVEFORMATEX WFX;
};