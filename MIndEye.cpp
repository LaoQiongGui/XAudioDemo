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

	// 初始化每个混音声源对应的监听器
	DWORD dwChannelMask;
	pMasterVoice->GetChannelMask(&dwChannelMask);
	X3DAudioInitialize(dwChannelMask, 1000, X3DInstance);
	if (hr != S_OK)
		throw(std::runtime_error("X3DAudioInitializeError"));
	dynamicListeners = std::set<ListenerDto>();
	for (int i = 0; i < WFX.Format.nChannels; i++)
	{
		ListenerDto listenerDto = {};
		listenerDto.UID = i;
		XAUDIO2_SEND_DESCRIPTOR audioSend = {0, pMasterVoice};
		XAUDIO2_VOICE_SENDS audioSends = {1, &audioSend};
		pXAudio2->CreateSubmixVoice(
			&listenerDto.pSubmixVoice, 1, WFX.Format.nSamplesPerSec,
			NULL, PSMAINMIX, &audioSends, NULL);
		FLOAT *pLevelMatrix = new FLOAT[WFX.Format.nChannels]();
		for (DWORD j = 0; j < WFX.Format.nChannels; j++)
		{
			if (j == i)
				pLevelMatrix[j] = 1;
			else
				pLevelMatrix[j] = 0;
		}
		listenerDto.pSubmixVoice->SetOutputMatrix(
			pMasterVoice, 1, WFX.Format.nChannels,
			pLevelMatrix, NULL);
		dynamicListeners.insert(listenerDto);
	}

	MindEye::getAmbientEmitterDtoStatus = NULL;
	MindEye::getDynamicEmitterDtoStatus = NULL;
	MindEye::getDynamicListenerStatus = NULL;
}

MindEye::~MindEye()
{
}

// 设置获取环境音效的回调函数
VOID MindEye::setAmbientEmitterProc(std::set<EmitterDto> (*getAmbientEmitterDtoStatus)())
{
	MindEye::getAmbientEmitterDtoStatus = getAmbientEmitterDtoStatus;
	return;
}

// 设置获取动态音效的回调函数
VOID MindEye::setDynamicEmitterProc(std::set<EmitterDto> (*getDynamicEmitterDtoStatus)())
{
	MindEye::getDynamicEmitterDtoStatus = getDynamicEmitterDtoStatus;
	return;
}

// 设置获取动态监听器状态的回调函数
VOID MindEye::setDynamicListenerProc(std::set<ListenerDto> (*getDynamicListenerStatus)())
{
	MindEye::getDynamicListenerStatus = getDynamicListenerStatus;
	return;
}

HWND MindEye::getHWndPlayer()
{
	return MindEye::hWndPlayer;
}

// 删除音频发生器 入参为待删除的音频发生器
VOID MindEye::removeDynamicEmitters(std::set<EmitterDto> tmpDynamicEmitters)
{
	auto pTmpDynamicEmitter = tmpDynamicEmitters.begin();
	while (pTmpDynamicEmitter != tmpDynamicEmitters.end()) {
		EmitterDto tmpDynamicEmitter = *dynamicEmitters.find(*pTmpDynamicEmitter);
		tmpDynamicEmitter.pAudio2SourceVoice->Stop();
		dynamicEmitters.erase(tmpDynamicEmitter);
		pTmpDynamicEmitter++;
	}
	return;
}

// 新增音频发生器 入参为待新增的音频发生器
VOID MindEye::addDynamicEmitters(std::set<EmitterDto> tmpDynamicEmitters)
{
	auto pTmpDynamicEmitter = tmpDynamicEmitters.begin();
	while (pTmpDynamicEmitter != tmpDynamicEmitters.end()) {
		EmitterDto tmpDynamicEmitter = *pTmpDynamicEmitter;
		tmpDynamicEmitter.emitter.Position = tmpDynamicEmitter.Position;
		tmpDynamicEmitter.emitter.Velocity = tmpDynamicEmitter.Velocity;
		pXAudio2->CreateSourceVoice(
			&tmpDynamicEmitter.pAudio2SourceVoice, (WAVEFORMATEX*)&WFX);
		XAUDIO2_SEND_DESCRIPTOR* pSend = 
			new XAUDIO2_SEND_DESCRIPTOR[WFX.Format.nChannels]();
		auto pDynamicListener = dynamicListeners.begin();
		for (int i = 0; i < WFX.Format.nChannels; i++) {
			ListenerDto dynamicListener = *pDynamicListener;
			pSend[i] = { 0, dynamicListener.pSubmixVoice };
			pDynamicListener++;
		}
		XAUDIO2_VOICE_SENDS sendlist = { WFX.Format.nChannels, pSend };
		tmpDynamicEmitter.pAudio2SourceVoice->SetOutputVoices(&sendlist);
		dynamicEmitters.insert(tmpDynamicEmitter);
		pTmpDynamicEmitter++;
	}
	return;
}

// 更新音频发生器状态
VOID MindEye::updateDynamicEmitters()
{
	// 遍历动态音频发生器
	auto pDynamicEmitter = dynamicEmitters.begin();
	while (pDynamicEmitter != dynamicEmitters.end()) {
		EmitterDto dynamicEmitter = *pDynamicEmitter;
		// 遍历动态音频接收器
		auto pDynamicListener = dynamicListeners.begin();
		while (pDynamicListener != dynamicListeners.end()) {
			ListenerDto dynamicListener = *pDynamicListener;
			// X3DAudio计算
			X3DAUDIO_DSP_SETTINGS DSPSettings = {};
			DSPSettings.SrcChannelCount = 1;
			DSPSettings.DstChannelCount = 1;
			X3DAudioCalculate(X3DInstance, &dynamicListener.listener, &dynamicEmitter.emitter, 
				X3DAUDIO_CALCULATE_MATRIX | X3DAUDIO_CALCULATE_DELAY | X3DAUDIO_CALCULATE_DOPPLER,
				&DSPSettings);
			// 设置音量
			dynamicEmitter.pAudio2SourceVoice->SetOutputMatrix(
				dynamicListener.pSubmixVoice, 1, 1, DSPSettings.pMatrixCoefficients);
			// 设置频率（多普勒）
			dynamicEmitter.pAudio2SourceVoice->SetFrequencyRatio(DSPSettings.DopplerFactor);
			pDynamicListener++;
		}
		pDynamicEmitter++;
	}
	return;
}

// 自动刷新函数
VOID MindEye::OnRefresh()
{
	if (MindEye::getDynamicEmitterDtoStatus == NULL)
		return;

	// 更新动态音频接收器状态
	std::set<ListenerDto> newDynamicListeners = getDynamicListenerStatus();
	auto pNewDynamicListener = newDynamicListeners.begin();
	auto pOldDynamicListener = dynamicListeners.begin();
	while (pNewDynamicListener != newDynamicListeners.end() &&
		   pOldDynamicListener != dynamicListeners.end())
	{
		ListenerDto newDynamicListener = *pNewDynamicListener;
		ListenerDto oldDynamicListener = *pOldDynamicListener;
		oldDynamicListener.Position = newDynamicListener.Position;
		oldDynamicListener.Velocity = newDynamicListener.Velocity;
		oldDynamicListener.listener.Position = oldDynamicListener.Position;
		oldDynamicListener.listener.Velocity = oldDynamicListener.Velocity;
		pNewDynamicListener++;
		pOldDynamicListener++;
	}

	// 更新动态音频发生器状态
	std::set<EmitterDto> newDynamicEmitters = MindEye::getDynamicEmitterDtoStatus();
	// 删除声源
	removeDynamicEmitters(dynamicEmitters - newDynamicEmitters);
	// 新增声源
	addDynamicEmitters(newDynamicEmitters - dynamicEmitters);
	// 更新所有声源状态
	updateDynamicEmitters();
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
