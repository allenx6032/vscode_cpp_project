// PromptDlg.cpp: 实现文件
//

#include "pch.h"
#include "AIChatBox.h"
#include "afxdialogex.h"
#include "PromptDlg.h"


// PromptDlg 对话框

IMPLEMENT_DYNAMIC(PromptDlg, CDialogEx)

PromptDlg::PromptDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_PROMPT_DIALOG, pParent)
{

}

PromptDlg::~PromptDlg()
{
}

void PromptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_Edit_Prompt);
}


BEGIN_MESSAGE_MAP(PromptDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &PromptDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// PromptDlg 消息处理程序


BOOL PromptDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// Edit上的提示文字
	m_Edit_Prompt.SetWindowText(m_strPrompt);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void PromptDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	// 获取用户输入
	m_Edit_Prompt.GetWindowText(m_strPrompt);

	// 检测是否为空
	if (m_strPrompt.IsEmpty())
	{
		MessageBox(TEXT("您的输入为空!"), TEXT("Error"), MB_ICONERROR);
		return;
	}

	// 关闭对话框
	CDialogEx::OnOK();
}
