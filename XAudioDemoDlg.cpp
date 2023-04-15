
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
	pXAudio2 = NULL;
	pMasterVoice = NULL;
	pSourceVoice = NULL;
	buffer = {0};
	wfx = {0};
}

BEGIN_MESSAGE_MAP(CXAudioDemoDlg, CDialogEx)
ON_WM_SYSCOMMAND()
ON_WM_PAINT()
ON_WM_QUERYDRAGICON()
ON_BN_CLICKED(IDOK, &CXAudioDemoDlg::OnBnClickedOk)
END_MESSAGE_MAP()

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
	// 初始化COM
	hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (hr != S_OK)
	{
		::MessageBox(NULL, L"COM初始化失败", L"初始化异常", NULL);
		return TRUE;
	}

	// 创建XAudio2引擎
	hr = XAudio2Create(&pXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	if (hr != S_OK)
	{
		::MessageBox(NULL, L"XAudio2引擎创建失败", L"初始化异常", NULL);
		return TRUE;
	}

	// 创建主语音
	IXAudio2MasteringVoice *pMasterVoice = NULL;
	hr = pXAudio2->CreateMasteringVoice(&pMasterVoice);
	if (hr != S_OK)
	{
		::MessageBox(NULL, L"主语音创建失败", L"初始化异常", NULL);
		return TRUE;
	}

	hr = loadDefault();
	if (hr != S_OK)
	{
		::MessageBox(NULL, L"载入默认格式失败", L"初始化异常", NULL);
		OnCancel();
		return TRUE;
	}

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

void CXAudioDemoDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	HANDLE hFile = ::CreateFile(
		defaultPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		::MessageBox(NULL, L"打开文件失败", L"音频加载异常", NULL);
		return;
	}

	DWORD dwRet = 0;
	dwRet = ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if (dwRet == INVALID_SET_FILE_POINTER)
	{
		::MessageBox(NULL, L"文件初始化失败", L"音频加载异常", NULL);
		return;
	}

	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;

	hr = FindChunk(hFile, fourccDATA, dwChunkSize, dwChunkPosition);
	BYTE* pDataBuffer = new BYTE[dwChunkSize];
	hr = ReadChunkData(hFile, pDataBuffer, dwChunkSize, dwChunkPosition);
	if (hr != S_OK)
	{
		::MessageBox(NULL, L"音频加载失败", L"音频加载异常", NULL);
		return;
	}

	buffer.AudioBytes = dwChunkSize; // 填充缓冲区大小
	buffer.pAudioData = pDataBuffer; // 填充缓冲区数据
	buffer.Flags = XAUDIO2_END_OF_STREAM;

	pSourceVoice->Stop();
	pSourceVoice->FlushSourceBuffers();
	pSourceVoice->SubmitSourceBuffer(&buffer);
	pSourceVoice->Start();
	if (hr != S_OK)
	{
		::MessageBox(NULL, L"语音播放失败", L"源语音异常", NULL);
		return;
	}
	return;
}

HRESULT CXAudioDemoDlg::loadDefault()
{
	HANDLE hFile = ::CreateFile(
		defaultPath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) return -1;
	DWORD dwRet = 0;
	dwRet = ::SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	if (dwRet == INVALID_SET_FILE_POINTER) return -1;
	DWORD dwChunkSize = 0;
	DWORD dwChunkPosition = 0;
	DWORD filetype = 0;
	hr = FindChunk(hFile, fourccRIFF, dwChunkSize, dwChunkPosition);
	if (hr != S_OK) return hr;
	hr = ReadChunkData(hFile, &filetype, sizeof(DWORD), dwChunkPosition);
	if (hr != S_OK) return hr;
	hr = FindChunk(hFile, fourccFMT, dwChunkSize, dwChunkPosition);
	if (hr != S_OK) return hr;
	hr = ReadChunkData(hFile, &wfx, dwChunkSize, dwChunkPosition);
	if (hr != S_OK) return hr;
	hr = pXAudio2->CreateSourceVoice(&pSourceVoice, (WAVEFORMATEX*)&wfx);
	if (hr != S_OK) return hr;
	return S_OK;
}