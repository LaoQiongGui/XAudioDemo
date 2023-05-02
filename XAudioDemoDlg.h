
// XAudioDemoDlg.h: 头文件
//

#pragma once
#include "Common.h"
#include "MIndEye.h"

// CXAudioDemoDlg 对话框
class CXAudioDemoDlg : public CDialogEx
{
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

protected:
	HICON m_hIcon;
	MindEye mindEye;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
