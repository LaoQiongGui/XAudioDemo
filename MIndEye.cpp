#include "pch.h"
#include "MIndEye.h"

MindEye::MindEye()
{
	// 创建消息窗口
	CreateWindowInDto *pCreateWindowInDto = new CreateWindowInDto();
	HANDLE hEvent = ::CreateEvent(NULL, TRUE, FALSE, L"CreateMsgWindowEvent");
	if (hEvent == NULL)
		throw(std::runtime_error("CreateEventError"));
	pCreateWindowInDto->hEvent = hEvent;
	hPlayer = ::CreateThread(
		NULL, NULL, (LPTHREAD_START_ROUTINE)MindEye::createMsgWindow,
		pCreateWindowInDto, NULL, NULL);

	// 等待窗口初始化完毕
	::WaitForSingleObject(hEvent, INFINITE);
	hWndPlayer = pCreateWindowInDto->hWnd;
	delete pCreateWindowInDto;

	// 创建消息发送线程
	hMsgSender = ::CreateThread(
		NULL, NULL, (LPTHREAD_START_ROUTINE)MindEye::createMsgSender,
		this, NULL, NULL);

	// 设置波形图属性
	WFX = {0};
	WFX.Format.wFormatTag = WAVE_FORMAT_EXTENSIBLE;
	WFX.Format.nChannels = DEFAULTCHANNELS;
	WFX.Format.nSamplesPerSec = DEFAULTSAMPLERATE;
	WFX.Format.nAvgBytesPerSec = DEFAULTBYTERATE;
	WFX.Format.nBlockAlign = DEFAULTBLOCKALIGN;
	WFX.Format.cbSize = 22;

	HRESULT hr = S_OK;
	// 初始化COM
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK)
		throw(std::runtime_error("CoInitializeExError"));

	// 创建XAudio引擎
	hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (hr != S_OK)
		throw(std::runtime_error("XAudio2CreateError"));

	// 初始化输出声源
	hr = pXAudio2->CreateMasteringVoice(
		&pMasterVoice, WFX.Format.nChannels, WFX.Format.nSamplesPerSec);
	if (hr != S_OK)
		throw(std::runtime_error("CreateMasterVoiceError"));

	// 初始化环境混音器
	XAUDIO2_SEND_DESCRIPTOR audioSend = {0, pMasterVoice};
	XAUDIO2_VOICE_SENDS audioSends = {1, &audioSend};
	hr = pXAudio2->CreateSubmixVoice(
		&pAmbientSubmixVoice, 1, WFX.Format.nSamplesPerSec,
		NULL, PSMAINMIX, &audioSends, NULL);
	if (hr != S_OK)
		throw(std::runtime_error("CreateAmbientSubmixVoiceError"));

	// 初始化动态混音器
	pSubmixVoiceGroup = new IXAudio2SubmixVoice *[WFX.Format.nChannels];
	for (DWORD i = 0; i < WFX.Format.nChannels; i++)
	{
		XAUDIO2_SEND_DESCRIPTOR audioSend = {0, pMasterVoice};
		XAUDIO2_VOICE_SENDS audioSends = {1, &audioSend};
		hr = pXAudio2->CreateSubmixVoice(
			&pSubmixVoiceGroup[i], 1, WFX.Format.nSamplesPerSec,
			NULL, PSMAINMIX, &audioSends, NULL);
		if (hr != S_OK)
			throw(std::runtime_error("CreateSubmixVoiceError"));
		FLOAT *pLevelMatrix = new FLOAT[WFX.Format.nChannels]();
		for (DWORD j = 0; j < WFX.Format.nChannels; j++)
		{
			if (j == i)
				pLevelMatrix[j] = 1;
			else
				pLevelMatrix[j] = 0;
		}
		hr = pSubmixVoiceGroup[i]->SetOutputMatrix(
			pMasterVoice, 1, WFX.Format.nChannels,
			pLevelMatrix, NULL);
		if (hr != S_OK)
			throw(std::runtime_error("SetOutputMatrixError"));
	}

	// 初始化每个混音声源对应的监听器
	DWORD dwChannelMask;
	pMasterVoice->GetChannelMask(&dwChannelMask);
	X3DAudioInitialize(dwChannelMask, 1000, X3DInstance);
	if (hr != S_OK)
		throw(std::runtime_error("X3DAudioInitializeError"));
	listenerGroup = new X3DAUDIO_LISTENER[WFX.Format.nChannels];

	MindEye::getAmbientSounds = NULL;
	MindEye::getDynamicSounds = NULL;
}

MindEye::~MindEye()
{
}

// 设置获取环境音效的回调函数
VOID MindEye::setAmbientSoundProc(std::set<SourceVoiceDto> (*getAmbientSounds)())
{
	MindEye::getAmbientSounds = getAmbientSounds;
	return;
}

// 设置获取动态音效的回调函数
VOID MindEye::setDynamicSoundProc(std::set<SourceVoiceDto> (*getDynamicSounds)())
{
	MindEye::getDynamicSounds = getDynamicSounds;
	return;
}

HWND MindEye::getHWndPlayer()
{
	return MindEye::hWndPlayer;
}

// 自动刷新函数
VOID MindEye::OnRefresh()
{
	if (MindEye::getDynamicSounds == NULL)
		return;
	// 取得新的声源
	std::set<SourceVoiceDto> newSourceVoiceDto = MindEye::getDynamicSounds();
	std::set<SourceVoiceDto> diffSourceVoiceDto = {};
	return;
}

int APIENTRY MindEye::createMsgWindow(CreateWindowInDto *pCreateWindowInDto)
{
	// 创建消息窗口
	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	// 注册窗口类
	int ret = registAudioPlayerClass(hInstance);
	if (ret == NULL)
		return -1;
	pCreateWindowInDto->hWnd = ::CreateWindowEx(
		NULL, DEFAULTWINDOWCLASSNAME, DEFAULTWINDOWNAME, NULL,
		0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
	if (pCreateWindowInDto->hWnd == NULL)
		return -1;

	// 取消主线程等待
	::SetEvent(pCreateWindowInDto->hEvent);

	// 创建消息循环
	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(109));
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return 0;
}

int APIENTRY MindEye::createMsgSender(MindEye mindEye)
{
	while (true)
	{
		::SendNotifyMessage(mindEye.hWndPlayer, MY_REFRESH, (WPARAM)&mindEye, NULL);
		::Sleep(25);
	}
}

ATOM MindEye::registAudioPlayerClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.lpfnWndProc = MindEye::AudioPlayerWndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = DEFAULTWINDOWCLASSNAME;

	return RegisterClassEx(&wcex);
}

std::map<HWND, MindEye *> mindEyeWindowMap = {};

LRESULT CALLBACK MindEye::AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	MindEye *pMindEye = NULL;
	switch (uMessage)
	{
	case MY_REFRESH:
		if (wParam == NULL)
			break;
		pMindEye = (MindEye *)wParam;
		pMindEye->OnRefresh();
		break;
	default:
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	return 0;
}

BOOL SourceVoiceDto::operator==(const SourceVoiceDto &sourceVoiceDto) const
{
	if (UID == sourceVoiceDto.UID)
		return true;
	else
		return false;
}

BOOL SourceVoiceDto::operator<(const SourceVoiceDto &sourceVoiceDto) const
{
	if (UID < sourceVoiceDto.UID)
		return true;
	else
		return false;
}

BOOL SourceVoiceDto::operator>(const SourceVoiceDto &sourceVoiceDto) const
{
	if (UID > sourceVoiceDto.UID)
		return true;
	else
		return false;
}

std::set<SourceVoiceDto> operator-(const std::set<SourceVoiceDto> &inSetFirst, const std::set<SourceVoiceDto> &inSetSecond)
{
	std::set<SourceVoiceDto> outSet = std::set<SourceVoiceDto>();
	auto tmpDto1 = inSetFirst.begin();
	auto tmpDto2 = inSetSecond.begin();
	while (tmpDto1 != inSetFirst.end() && tmpDto2 != inSetSecond.end()) {
		if (*tmpDto1 == *tmpDto2) {
			tmpDto1++;
			tmpDto2++;
		}
		else if (*tmpDto1 < *tmpDto2) {
			outSet.insert(*tmpDto1);
			tmpDto1++;
		}
		else {
			tmpDto2++;
		}
	}

	while (tmpDto1 != inSetFirst.end()) {
		outSet.insert(*tmpDto1);
		tmpDto1++;
	}
	return outSet;
}
