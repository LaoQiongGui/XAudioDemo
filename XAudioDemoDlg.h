
// XAudioDemoDlg.h: 头文件
//

#pragma once
#include "Common.h"
#include "MindEyeFactory.h"
#include "Test.h"

// CXAudioDemoDlg 对话框
class CXAudioDemoDlg : public CDialogEx
{
private:
	ISpVoice *pSpVoice = NULL;

	// 构造
public:
	CXAudioDemoDlg(CWnd *pParent = nullptr); // 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum
	{
		IDD = IDD_XAUDIODEMO_DIALOG
	};
#endif
private:
	void OnInitLogger();
	void OnInitMindEye();
	void OnInitTts();

protected:
	virtual void DoDataExchange(CDataExchange *pDX);

	HICON m_hIcon;
	MindEyeFactory *pMindEyeFactory;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CString displayStr;

	afx_msg void OnPlayBtnClick();
	afx_msg void OnStopBtnCilck();
	afx_msg void OnFFTBtnClicked();
	afx_msg void OnSaveBtnClicked();
	afx_msg void OnSpeakBtnClicked();
};
