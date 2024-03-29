
// XAudioDemoDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "XAudioDemo.h"
#include "XAudioDemoDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

HRESULT hr;
WCHAR wLoadPath[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/origin/2.wav";
WCHAR wSavePathLF[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/LF/2.wav";
WCHAR wSavePathMF[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/MF/2.wav";
WCHAR wSavePathHF[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/HF/2.wav";

extern std::shared_ptr<spdlog::logger> logger = spdlog::basic_logger_mt("basic_logger", "logs/basic_log.txt");
extern CHAR *logStr = new CHAR[MAX_PATH]{};

void CXAudioDemoDlg::OnInitLogger()
{
	spdlog::set_level(spdlog::level::info);
	spdlog::flush_on(spdlog::level::err);
	spdlog::flush_every(std::chrono::seconds(10));
}

void CXAudioDemoDlg::OnInitMindEye()
{
	WAVEFORMATEX WFX = {};
	WFX.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
	WFX.nChannels = 2;
	WFX.nSamplesPerSec = 44100;
	WFX.nAvgBytesPerSec = 2 * 44100 * 4;
	WFX.nBlockAlign = 4;
	WFX.wBitsPerSample = 4 * 8;
	WFX.cbSize = sizeof(WAVEFORMATEX);
	pMindEyeFactory = new MindEyeFactory(WFX);
}

void CXAudioDemoDlg::OnInitTts()
{
	DWORD dwTokensNumber = 0;
	WCHAR *wTokenName = NULL;
	IEnumSpObjectTokens *pIEnumSpObjectTokens = NULL;
	ISpObjectToken *pSpObjectToken = NULL;

	::CoInitializeEx(NULL, COINIT_MULTITHREADED);
	::CoCreateInstance(CLSID_SpVoice, NULL, CLSCTX_INPROC_SERVER, IID_ISpVoice, (void **)&pSpVoice);
	::SpEnumTokens(SPCAT_VOICES, NULL, NULL, &pIEnumSpObjectTokens);
	pIEnumSpObjectTokens->GetCount(&dwTokensNumber);
	for (DWORD i = 0; i < dwTokensNumber; i++)
	{
		pIEnumSpObjectTokens->Item(i, &pSpObjectToken);
		pSpObjectToken->GetId(&wTokenName);
		if (wcsstr(wTokenName, L"zh-CN") != 0)
		{
			pSpVoice->SetVoice(pSpObjectToken);
			break;
		}
	}
	pSpVoice->SetRate(10);
}

// CXAudioDemoDlg 对话框
CXAudioDemoDlg::CXAudioDemoDlg(CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_XAUDIODEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	displayStr = "";
	pMindEyeFactory = NULL;
	return;
}

BEGIN_MESSAGE_MAP(CXAudioDemoDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(PLAY_BTN, &CXAudioDemoDlg::OnPlayBtnClick)
ON_BN_CLICKED(STOP_BTN, &CXAudioDemoDlg::OnStopBtnCilck)
ON_BN_CLICKED(FFT_BUTTON, &CXAudioDemoDlg::OnFFTBtnClicked)
ON_BN_CLICKED(SAVE_BUTTON, &CXAudioDemoDlg::OnSaveBtnClicked)
ON_BN_CLICKED(SPEAK_BUTTON1, &CXAudioDemoDlg::OnSpeakBtnClicked)
END_MESSAGE_MAP()

void CXAudioDemoDlg::DoDataExchange(CDataExchange *pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, DISPLAY, displayStr);
}

// CXAudioDemoDlg 消息处理程序
BOOL CXAudioDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu *pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);	 // 设置大图标
	SetIcon(m_hIcon, FALSE); // 设置小图标

	// TODO: 在此添加额外的初始化代码
	// 初始化日志
	OnInitLogger();

	// 初始化MindEye
	OnInitMindEye();

	// 初始化TTS
	OnInitTts();
	return TRUE; // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。
void CXAudioDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 当用户拖动最小化窗口时系统调用此函数取得光标
// 显示。
HCURSOR CXAudioDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

/**
 * 播放按钮点击事件.
 *
 */
void CXAudioDemoDlg::OnPlayBtnClick()
{
	std::function<std::map<DWORD64, MindEyeEmitterInfo *>(void)> getEmitterInfo = getEmitterInfoForTest;
	std::function<std::map<DWORD64, MindEyeListenerInfo *>(void)> getListenerInfo = getListenerInfoForTest;
	pMindEyeFactory->addEnv(1, getEmitterInfo, getListenerInfo);
	logger->info("Play button clicked.");
}

/**
 * 停止按钮点击事件.
 *
 */
void CXAudioDemoDlg::OnStopBtnCilck()
{
	pMindEyeFactory->removeEnv(1);
	logger->info("Stop button clicked.");
}

/**
 * FFT按钮点击事件.
 *
 */
void CXAudioDemoDlg::OnFFTBtnClicked()
{
	HANDLE hFileIn;
	HANDLE hFileOutList[3] = {};
	DWORD dwFrequencyList[3] = {750, 1500, 3000};

	WCHAR wLoadFilePattern[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/origin/*.wav";
	WCHAR wLoadFolder[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/origin/";
	WCHAR wSaveFolderLF[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/LF/";
	WCHAR wSaveFolderMF[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/MF/";
	WCHAR wSaveFolderHF[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/sound effect/HF/";
	WCHAR wLoadPath[MAX_PATH] = L"";
	WCHAR wSavePathLF[MAX_PATH] = L"";
	WCHAR wSavePathMF[MAX_PATH] = L"";
	WCHAR wSavePathHF[MAX_PATH] = L"";
	WIN32_FIND_DATAW ffd;
	HANDLE hFindFile = ::FindFirstFile(wLoadFilePattern, &ffd);
	do
	{
		swprintf_s(wLoadPath, MAX_PATH, L"%s%s", wLoadFolder, ffd.cFileName);
		hFileIn = ::CreateFile(
			wLoadPath, GENERIC_READ, FILE_SHARE_READ,
			NULL, OPEN_EXISTING, 0, NULL);
		swprintf_s(wSavePathLF, MAX_PATH, L"%s%s", wSaveFolderLF, ffd.cFileName);
		hFileOutList[0] = ::CreateFile(
			wSavePathLF, GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, 0, NULL);
		swprintf_s(wSavePathMF, MAX_PATH, L"%s%s", wSaveFolderMF, ffd.cFileName);
		hFileOutList[1] = ::CreateFile(
			wSavePathMF, GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, 0, NULL);
		swprintf_s(wSavePathHF, MAX_PATH, L"%s%s", wSaveFolderHF, ffd.cFileName);
		hFileOutList[2] = ::CreateFile(
			wSavePathHF, GENERIC_WRITE, FILE_SHARE_READ,
			NULL, CREATE_ALWAYS, 0, NULL);
		DivideFrequency(hFileIn, hFileOutList, dwFrequencyList, 3);
	} while (::FindNextFile(hFindFile, &ffd));
	::FindClose(hFindFile);
	logger->info("FFT button clicked.");
}

/**
 * 转存按钮点击事件.
 *
 */
void CXAudioDemoDlg::OnSaveBtnClicked()
{
	logger->info("Save button clicked.");

	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD dwSize = 0;
	WavFileStruct wavFileStruct;
	WAVEFORMATEX WFXR, WFXW;
	BYTE *pDataBufferR;
	FLOAT32 *pDataBufferW;
	WCHAR wLoadPath[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/test/INT16.wav";
	WCHAR wSavePath[] = L"D:/Workspace/LaoQiongGui/The-concert-of-Isaac/resources/test/FLOAT32_OUT.wav";
	HANDLE hFile;

	// 打开音频文件（读取）
	hFile = ::CreateFile(
		wLoadPath, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		logger->error(L"打开音频文件（读取）失败 {0}", wLoadPath);
	}

	// 读取波形图
	FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	ReadChunkData(hFile, &WFXR, dwChunkSize, dwChunkPosition);

	// 读取音频数据
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	// 读取缓冲区
	dwSize = dwChunkSize / WFXR.nBlockAlign;
	pDataBufferR = new BYTE[dwSize * WFXR.nBlockAlign]{};
	ReadChunkData(hFile, pDataBufferR, dwChunkSize, dwChunkPosition);

	// 关闭音频文件（读取）
	::CloseHandle(hFile);

	// 打开音频文件（写入）
	hFile = ::CreateFile(
		wSavePath, GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, 0, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		logger->error(L"打开音频文件（写入）失败 {0}", wLoadPath);
	}

	// 设置写入波形图
	WFXW = WFXR;
	SetWFXF32(&WFXW);
	wavFileStruct.FillWAVEFORMATEX(WFXW);

	// 写入缓冲区
	pDataBufferW = new FLOAT32[dwSize]{};
	for (DWORD i = 0; i < dwSize; i++)
	{
		// 缓冲区数据转为INT16格式
		INT16 DataBufferI16 = (pDataBufferR[2 * i + 1] << 8) | pDataBufferR[2 * i];
		FLOAT32 DataBufferI32 = DataBufferI16;
		pDataBufferW[i] = DataBufferI32 / 32768;
	}

	// 设置写入缓冲区大小
	wavFileStruct.FillBufferSize(dwSize * WFXW.nBlockAlign);

	// 写入音频文件头
	::WriteFile(hFile, &wavFileStruct, sizeof(wavFileStruct), NULL, NULL);

	// 写入音频数据体
	::WriteFile(hFile, pDataBufferW, dwSize * WFXW.nBlockAlign, NULL, NULL);

	// 关闭音频文件（写入）
	::CloseHandle(hFile);
	delete[] pDataBufferR, pDataBufferW;
	return;
}

/**
 * 朗读按钮点击事件.
 *
 */
void CXAudioDemoDlg::OnSpeakBtnClicked()
{
	HRESULT hr = pSpVoice->Speak(L"这是一段中文测试文本。", SPF_ASYNC, NULL);
	return;
}
