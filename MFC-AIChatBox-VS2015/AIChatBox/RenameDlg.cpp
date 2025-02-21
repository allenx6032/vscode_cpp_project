// RenameDlg.cpp: 实现文件
//

#include "pch.h"
#include "AIChatBox.h"
#include "afxdialogex.h"
#include "RenameDlg.h"


// RenameDlg 对话框

IMPLEMENT_DYNAMIC(RenameDlg, CDialogEx)

RenameDlg::RenameDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_RENAME_DIALOG, pParent)
	, m_name(_T(""))
{

}

RenameDlg::~RenameDlg()
{
}

void RenameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, m_name);
}


BEGIN_MESSAGE_MAP(RenameDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &RenameDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// RenameDlg 消息处理程序


void RenameDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码

	if (m_name.IsEmpty())
	{
		MessageBox(TEXT("请输入名称！"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	CDialogEx::OnOK();
}
