
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
WCHAR *wFilePath = L"D:/Workspace/LaoQiongGui/sample.wav";
static WCHAR *defaultPath = L"D:/Workspace/LaoQiongGui/sample.wav";

// CXAudioDemoDlg 对话框
CXAudioDemoDlg::CXAudioDemoDlg(CWnd *pParent /*=nullptr*/)
	: CDialogEx(IDD_XAUDIODEMO_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	displayStr = "";
	pMindEyeFactory = NULL;
	logger = spdlog::basic_logger_mt("basic_logger", "logs/basic_log.txt");
	return;
}

BEGIN_MESSAGE_MAP(CXAudioDemoDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(PLAY_BTN, &CXAudioDemoDlg::OnPlayBtnClick)
ON_BN_CLICKED(STOP_BTN, &CXAudioDemoDlg::OnStopBtnCilck)
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
	pMindEyeFactory = new MindEyeFactory();

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

void CXAudioDemoDlg::OnPlayBtnClick()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwChunkSize;
	DWORD dwChunkPosition;
	WAVEFORMATEXTENSIBLE WFXLeft = {};
	WAVEFORMATEXTENSIBLE WFXRight = {};
	HANDLE hFile = ::CreateFile(
		wFilePath, GENERIC_READ, FILE_SHARE_READ,
		NULL, OPEN_EXISTING, 0, NULL);
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if (pSourceVoice1 == NULL || pSourceVoice2 == NULL)
	{
		FLOAT LevelMatrix[2] = {0};
		FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
		ReadChunkData(hFile, &WFXLeft, dwChunkSize, dwChunkPosition);
		ReadChunkData(hFile, &WFXRight, dwChunkSize, dwChunkPosition);
		pMindEyeFactory->pXAudio2->CreateSourceVoice(
			&pSourceVoice1, &WFXLeft.Format);
		WFXRight.Format.nSamplesPerSec = (DWORD)((WFXRight.Format.nSamplesPerSec / 1.005) / 10) * 10;
		WFXRight.Format.nAvgBytesPerSec = WFXRight.Format.nSamplesPerSec * WFXRight.Format.nBlockAlign;
		pMindEyeFactory->pXAudio2->CreateSourceVoice(
			&pSourceVoice2, &WFXRight.Format);
		LevelMatrix[0] = 1;
		LevelMatrix[1] = 0;
		pSourceVoice1->SetOutputMatrix(
			pMindEyeFactory->pMasterVoice, 1, 2, LevelMatrix, NULL);
		LevelMatrix[0] = 0;
		LevelMatrix[1] = 1;
		pSourceVoice2->SetOutputMatrix(
			pMindEyeFactory->pMasterVoice, 1, 2, LevelMatrix, NULL);
	}
	UpdateData(TRUE);
	XAUDIO2_BUFFER buffer = {0};
	FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	BYTE *pDataBuffer = new BYTE[dwChunkSize];
	ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
	buffer.AudioBytes = dwChunkSize;
	buffer.pAudioData = pDataBuffer;
	buffer.Flags = XAUDIO2_END_OF_STREAM;
	pSourceVoice1->SubmitSourceBuffer(
		&buffer, NULL);
	pSourceVoice2->SubmitSourceBuffer(
		&buffer, NULL);
	pSourceVoice1->Start();
	pSourceVoice2->Start();
	::CloseHandle(hFile);

	logger->info("Play button clicked.");
	UpdateData(FALSE);
}

void CXAudioDemoDlg::OnStopBtnCilck()
{
	// TODO: 在此添加控件通知处理程序代码
	if (pSourceVoice1 == NULL || pSourceVoice2 == NULL)
		return;
	UpdateData(TRUE);
	pSourceVoice1->Stop();
	pSourceVoice1->FlushSourceBuffers();
	pSourceVoice2->Stop();
	pSourceVoice2->FlushSourceBuffers();

	logger->info("Stop button clicked.");
	UpdateData(FALSE);
}
