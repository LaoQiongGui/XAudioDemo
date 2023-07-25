#include "pch.h"
#include "MindEyeFactory.h"

MindEyeFactory::MindEyeFactory(WAVEFORMATEX WFX)
{
	// 创建消息窗口
	CreateWindowInDto *pCreateWindowInDto = new CreateWindowInDto();
	HANDLE hEvent = ::CreateEvent(NULL, TRUE, FALSE, L"CreateMsgWindowEvent");
	if (hEvent == NULL)
	{
		throw(std::runtime_error("CreateEventError"));
	}

	pCreateWindowInDto->hEvent = hEvent;
	hPlayer = ::CreateThread(
		NULL, NULL, (LPTHREAD_START_ROUTINE)MindEyeFactory::createMsgWindow,
		pCreateWindowInDto, NULL, NULL);

	// 等待窗口初始化完毕
	::WaitForSingleObject(hEvent, INFINITE);
	hWndPlayer = pCreateWindowInDto->hWnd;
	delete pCreateWindowInDto;

	// 创建消息发送线程
	hMsgSender = ::CreateThread(
		NULL, NULL, (LPTHREAD_START_ROUTINE)MindEyeFactory::createMsgSender,
		this, NULL, NULL);

	// 设置波形图属性
	MindEyeFactory::WFX = WFX;
	WFX.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	WFX.nChannels = 2;
	WFX.nSamplesPerSec = 44100;
	WFX.nAvgBytesPerSec = 2 * 44100 * 4;
	WFX.nBlockAlign = 4;
	WFX.wBitsPerSample = 4 * 8;
	WFX.cbSize = sizeof WAVEFORMATEX;

	HRESULT hr = S_OK;
	// 初始化COM
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK)
	{
		throw(std::runtime_error("CoInitializeExError"));
	}

	// 创建XAudio引擎
	hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (hr != S_OK)
	{
		throw(std::runtime_error("XAudio2CreateError"));
	}

	// 初始化输出声源
	hr = pXAudio2->CreateMasteringVoice(
		&pMasterVoice, WFX.nChannels, WFX.nSamplesPerSec);
	if (hr != S_OK)
	{
		throw(std::runtime_error("CreateMasterVoiceError"));
	}
}

MindEyeFactory::~MindEyeFactory()
{
}

/**
 * 添加声音环境.
 * 
 * \param dwEnvId：待添加环境ID
 * \param getEmitterInfo：获取音频发生器信息函数
 * \param getListenerInfo：获取音频接收器信息函数
 * \return：成功添加返回S_OK
 */
HRESULT MindEyeFactory::addEnv(
	DWORD dwEnvId,
	std::function<std::set<MindEyeEmitterInfo>(void)> getEmitterInfo,
	std::function<std::set<MindEyeListenerInfo>(void)> getListenerInfo)
{
	// 声音环境已存在则错误返回
	if (MindEyeSoundEnvMap.find(dwEnvId) != MindEyeSoundEnvMap.end()) {
		return E_ACCESSDENIED;
	}

	// 声音环境不存在则新建并添加声音环境
	MindEyeSoundEnv *pMindEyeSoundEnv =
		new MindEyeSoundEnv(pXAudio2, pMasterVoice, WFX, getEmitterInfo, getListenerInfo);
	std::pair<DWORD, MindEyeSoundEnv> mindEyeSoundEnvEntry = { dwEnvId , *pMindEyeSoundEnv };
	MindEyeSoundEnvMap.insert(mindEyeSoundEnvEntry);
	return S_OK;
}

/**
 * 删除声音环境.
 * 
 * \param dwEnvId：待删除环境ID
 * \return：成功删除返回S_OK
 */
HRESULT MindEyeFactory::removeEnv(DWORD dwEnvId)
{
	auto pMindEyeSoundEnvEntry = MindEyeSoundEnvMap.find(dwEnvId);
	// 声音环境不存在则错误返回
	if (MindEyeSoundEnvMap.find(dwEnvId) == MindEyeSoundEnvMap.end()) {
		return E_ACCESSDENIED;
	}

	// 声音环境存在则删除并释放内存
	MindEyeSoundEnvMap.erase(pMindEyeSoundEnvEntry);
	MindEyeSoundEnv mindEyeSoundEnv = (*pMindEyeSoundEnvEntry).second;
	delete &mindEyeSoundEnv;
	return S_OK;
}

HWND MindEyeFactory::getHWndPlayer()
{
	return MindEyeFactory::hWndPlayer;
}

HRESULT MindEyeFactory::OnRefresh()
{
	LARGE_INTEGER performanceCount = {};
	QueryPerformanceCounter(&performanceCount);
	DWORD64 dwTimeStamp = performanceCount.QuadPart;
	return S_OK;
}

int APIENTRY MindEyeFactory::createMsgWindow(CreateWindowInDto *pCreateWindowInDto)
{
	// 创建消息窗口
	HINSTANCE hInstance = ::GetModuleHandle(NULL);

	// 注册窗口类
	int ret = registAudioPlayerClass(hInstance);
	if (ret == NULL) {
		return -1;
	}
	pCreateWindowInDto->hWnd = ::CreateWindowEx(
		NULL, DEFAULTWINDOWCLASSNAME, DEFAULTWINDOWNAME, NULL,
		0, 0, 0, 0, HWND_MESSAGE, NULL, hInstance, NULL);
	if (pCreateWindowInDto->hWnd == NULL) {
		return -1;
	}

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

int APIENTRY MindEyeFactory::createMsgSender(MindEyeFactory mindEye)
{
	while (true)
	{
		::SendNotifyMessage(mindEye.hWndPlayer, MY_REFRESH, (WPARAM)&mindEye, NULL);
		::Sleep(25);
	}
}

ATOM MindEyeFactory::registAudioPlayerClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEXW);
	wcex.lpfnWndProc = MindEyeFactory::AudioPlayerWndProc;
	wcex.hInstance = hInstance;
	wcex.lpszClassName = DEFAULTWINDOWCLASSNAME;

	return RegisterClassEx(&wcex);
}

LRESULT CALLBACK MindEyeFactory::AudioPlayerWndProc(HWND hWnd, UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	MindEyeFactory *pMindEye = NULL;
	switch (uMessage)
	{
	case MY_REFRESH:
		/*if (wParam == NULL)
			break;
		pMindEye = (MindEye *)wParam;
		pMindEye->OnRefresh();*/
		break;
	default:
		return DefWindowProc(hWnd, uMessage, wParam, lParam);
	}
	return 0;
}
