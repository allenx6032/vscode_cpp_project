#pragma once
#include "afxdialogex.h"


// PromptDlg 对话框

class PromptDlg : public CDialogEx
{
	DECLARE_DYNAMIC(PromptDlg)

public:
	PromptDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~PromptDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PROMPT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CEdit m_Edit_Prompt;
	CString m_strPrompt;
	afx_msg void OnBnClickedOk();
};
