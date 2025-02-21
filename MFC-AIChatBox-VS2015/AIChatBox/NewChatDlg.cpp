// NewChatDlg.cpp: 实现文件
//

#include "pch.h"
#include "AIChatBox.h"
#include "afxdialogex.h"
#include "NewChatDlg.h"


// NewChatDlg 对话框

IMPLEMENT_DYNAMIC(NewChatDlg, CDialogEx)

NewChatDlg::NewChatDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_NEWCHAT_DIALOG, pParent)
	, m_strChatName(_T(""))
	, m_strUserName(_T(""))
	, m_strAIName(_T(""))
	, m_strSystemPrompt(_T(""))
{

}

NewChatDlg::~NewChatDlg()
{
}

void NewChatDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_CHAT_NAME, m_strChatName);
	DDX_Control(pDX, IDC_COMBO_SELECT_PROMPT, m_ComboBox_LoadPrompt);
	DDX_Text(pDX, IDC_EDIT_USER_NAME, m_strUserName);
	DDX_Text(pDX, IDC_EDIT_AI_NAME, m_strAIName);
	DDX_Control(pDX, IDC_EDIT_SYSTEM_PROMPT, m_Edit_SystemPrompt);
}


BEGIN_MESSAGE_MAP(NewChatDlg, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBO_SELECT_PROMPT, &NewChatDlg::OnCbnSelchangeComboSelectPrompt)
	ON_BN_CLICKED(IDOK, &NewChatDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &NewChatDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// NewChatDlg 消息处理程序


BOOL NewChatDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	CTime currentTime = CTime::GetCurrentTime();
	CString defaultFileName = currentTime.Format(_T("%Y-%m-%d-%H-%M-%S"));
	m_strChatName = defaultFileName;

	if (chatContext->loadAllPrompts(m_Prompts))
	{
		for (const CString& prompt : m_Prompts)
		{
			m_ComboBox_LoadPrompt.AddString(prompt);
		}
		m_ComboBox_LoadPrompt.AddString(_T("自定义"));
		m_ComboBox_LoadPrompt.SetCurSel(0);
	}
	else
	{
		m_ComboBox_LoadPrompt.AddString(_T("未找到提示文件"));
	}

	//m_ComboBox_LoadPrompt.

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void NewChatDlg::LoadPrompts(CString prompt_name)
{
	chatContext->loadUserPrompt(ConvertCStringToUtf8(prompt_name));
	m_strUserName = ConvertUtf8ToCString(chatContext->getUserName());
	m_strAIName = ConvertUtf8ToCString(chatContext->getAssistantName());
	chatContext->drawSystem(m_Edit_SystemPrompt);
	UpdateData(FALSE);
}

void NewChatDlg::OnCbnSelchangeComboSelectPrompt()
{
	// TODO: 在此添加控件通知处理程序代码
	int index = m_ComboBox_LoadPrompt.GetCurSel();
	CString prompt_name;
	m_ComboBox_LoadPrompt.GetLBText(index, prompt_name);
	if (prompt_name == _T("自定义"))
	{
		m_strUserName = _T("");
		m_strAIName = _T("");
		m_strSystemPrompt = _T("");
		m_Edit_SystemPrompt.SetWindowText(_T(""));
		UpdateData(FALSE);
	}
	else
	{
		LoadPrompts(prompt_name);
	}

}


void NewChatDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);

	if (m_strChatName.IsEmpty())
	{
		MessageBox(TEXT("请输入对话名称！"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	if (m_strUserName.IsEmpty())
	{
		MessageBox(TEXT("请输入用户名称！"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	if (m_strAIName.IsEmpty())
	{
		MessageBox(TEXT("请输入AI名称！"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	// 将系统提示赋值给m_strSystemPrompt
	m_Edit_SystemPrompt.GetWindowText(m_strSystemPrompt);

	// 重置聊天上下文
	chatContext->removeAllActors();
	chatContext->resetChatHistory();

	// 添加演员
	chatContext->addActor(ConvertCStringToUtf8(m_strUserName), "user", "blue", "", chatContext->getUserIcon());
	chatContext->addActor(ConvertCStringToUtf8(m_strAIName), "assistant", "pink", "", chatContext->getAssistantIcon());
	chatContext->setUserAndAssistantName(ConvertCStringToUtf8(m_strUserName), ConvertCStringToUtf8(m_strAIName));


	// 设置系统提示
	chatContext->setupSystemPrompt(ConvertCStringToUtf8(m_strSystemPrompt));
	//chatContext->using_system_prompt = true;

	CDialogEx::OnOK();
}


void NewChatDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码

	CDialogEx::OnCancel();
}
