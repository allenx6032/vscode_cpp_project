#pragma once
#include "afxdialogex.h"
#include "chat.h"

// NewChatDlg 对话框

class NewChatDlg : public CDialogEx
{
	DECLARE_DYNAMIC(NewChatDlg)

public:
	NewChatDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~NewChatDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_NEWCHAT_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	Chat* chatContext;

	CString m_strChatName;
	CComboBox m_ComboBox_LoadPrompt;
	CString m_strUserName;
	CString m_strAIName;
	CString m_strSystemPrompt;
	std::string m_userIcon;
	std::string m_AIIcon;
	virtual BOOL OnInitDialog();

	std::vector<CString> m_Prompts;
	void LoadPrompts(CString prompt_name);
	afx_msg void OnCbnSelchangeComboSelectPrompt();
	CEdit m_Edit_SystemPrompt;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
