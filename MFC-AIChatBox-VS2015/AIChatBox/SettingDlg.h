#pragma once
#include "afxdialogex.h"

#include "utils.h"

// SettingDlg 对话框

class SettingDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SettingDlg)

public:
	SettingDlg(CWnd* pParent = nullptr);   // 标准构造函数
	virtual ~SettingDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SETTING_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:

	CString m_strModelPath;		// 模型文件夹路径
	int m_nPort;				// 端口号
	CString m_strModel;			// 模型文件名
	CString m_strFont;			// 字体
	int m_nFontSize;			// 字体大小
	COLORREF m_clrBackground;	// 背景颜色
	COLORREF m_clrText;			// 文本颜色
	COLORREF m_clrUser;			// 用户颜色
	COLORREF m_clrAssistant;	// 助手颜色
	BOOL m_isBold;				// 是否加粗
	BOOL m_isItalic;			// 是否斜体


	virtual BOOL OnInitDialog();
	CComboBox m_ComboBox_Models;
	CMFCColorButton m_ColorBtn_Background;
	CMFCColorButton m_ColorBtn_Text;
	CMFCColorButton m_ColorBtn_User;
	CMFCColorButton m_ColorBtn_Assistant;
	CMFCFontComboBox m_FontComboBox;
	CComboBox m_ComboBox_FontSize;
	CIPAddressCtrl m_IPAddressCtrl;
	afx_msg void OnBnClickedButtonSelectPath();

	// 将文件夹下的模型文件添加到下拉框
	BOOL AddModelsToComboBox();

	afx_msg void OnBnClickedButtonStartServer();
	afx_msg void OnBnClickedButtonStopServer();
	CString m_strHistory;
	afx_msg void OnBnClickedButtonHistoryPath();
	afx_msg void OnClose();

	bool isFirstOpen;
};
