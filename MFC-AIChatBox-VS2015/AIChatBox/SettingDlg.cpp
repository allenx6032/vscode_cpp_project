// SettingDlg.cpp: 实现文件
//

#include "pch.h"
#include "AIChatBox.h"
#include "afxdialogex.h"
#include "SettingDlg.h"


// SettingDlg 对话框

IMPLEMENT_DYNAMIC(SettingDlg, CDialogEx)

SettingDlg::SettingDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_SETTING_DIALOG, pParent)
	, m_nPort(8080)
	, m_strModelPath(_T(""))
	, m_isBold(FALSE)
	, m_isItalic(FALSE)
	, m_strHistory(_T(""))
{

}

SettingDlg::~SettingDlg()
{
}

void SettingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PORT, m_nPort);
	DDX_Text(pDX, IDC_EDIT_MODEL_PATH, m_strModelPath);
	DDX_Control(pDX, IDC_COMBO_SELECT_MODEL, m_ComboBox_Models);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON1, m_ColorBtn_Background);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON2, m_ColorBtn_Text);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON3, m_ColorBtn_User);
	DDX_Control(pDX, IDC_MFCCOLORBUTTON4, m_ColorBtn_Assistant);
	DDX_Control(pDX, IDC_MFCFONTCOMBO1, m_FontComboBox);
	DDX_Control(pDX, IDC_COMBO_FONT_SIZE, m_ComboBox_FontSize);
	DDX_Check(pDX, IDC_CHECK_BOLD, m_isBold);
	DDX_Check(pDX, IDC_CHECK_ITALIC, m_isItalic);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPAddressCtrl);
	DDX_Text(pDX, IDC_EDIT_HISTORY_PATH, m_strHistory);
}


BEGIN_MESSAGE_MAP(SettingDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PATH, &SettingDlg::OnBnClickedButtonSelectPath)
	ON_BN_CLICKED(IDC_BUTTON_START_SERVER, &SettingDlg::OnBnClickedButtonStartServer)
	ON_BN_CLICKED(IDC_BUTTON_STOP_SERVER, &SettingDlg::OnBnClickedButtonStopServer)
	ON_BN_CLICKED(IDC_BUTTON_HISTORY_PATH, &SettingDlg::OnBnClickedButtonHistoryPath)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// SettingDlg 消息处理程序


BOOL SettingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 默认当前路径下的models文件夹
	// m_ModelPath = _T(".\\models");

	// 将文件夹下的模型文件添加到下拉框
	if (AddModelsToComboBox())
	{
		if (m_strModel.IsEmpty())
		{
			// 默认选择第一个模型
			m_ComboBox_Models.SetCurSel(0);
		}
		else
		{
			// 根据m_strModel设置下拉框的选择
			int index = m_ComboBox_Models.FindStringExact(0, m_strModel);
			if (index != CB_ERR)
			{
				m_ComboBox_Models.SetCurSel(index);
			}
			else
			{
				// 默认选择第一个模型
				m_ComboBox_Models.SetCurSel(0);
			}
		}
	}
	

	// 设置IP和端口
	m_IPAddressCtrl.SetAddress(127, 0, 0, 1);
	// m_nPort = 8080;

	// 设置字体
	m_FontComboBox.SelectFont(_T("Segoe UI Emoji"));

	// 设置字体大小
	m_ComboBox_FontSize.AddString(_T("1"));
	m_ComboBox_FontSize.AddString(_T("2"));
	m_ComboBox_FontSize.AddString(_T("3"));
	m_ComboBox_FontSize.AddString(_T("4"));
	m_ComboBox_FontSize.AddString(_T("5"));
	// 选择第二个字体大小
	m_ComboBox_FontSize.SetCurSel(m_nFontSize - 1);

	// 设置背景颜色
	m_ColorBtn_Background.SetColor(m_clrBackground);
	// 设置文本颜色
	m_ColorBtn_Text.SetColor(m_clrText);
	// 设置用户颜色
	m_ColorBtn_User.SetColor(m_clrUser);
	// 设置助手颜色
	m_ColorBtn_Assistant.SetColor(m_clrAssistant);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void SettingDlg::OnBnClickedButtonSelectPath()
{
	// TODO: 在此添加控件通知处理程序代码

	// 弹出文件夹选择对话框
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = this->m_hWnd;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = _T("请选择模型文件夹");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl != NULL)
	{
		TCHAR path[MAX_PATH];
		SHGetPathFromIDList(idl, path);
		m_strModelPath = path;
		SetDlgItemText(IDC_EDIT_MODEL_PATH, path);
	}
	else
	{
		m_strModelPath = _T("");
	}

	// 将文件夹下的模型文件添加到下拉框
	if (AddModelsToComboBox())
	{
		// 默认选择第一个模型
		m_ComboBox_Models.SetCurSel(0);
	}
}

BOOL SettingDlg::AddModelsToComboBox()
{
	// 获取文件夹下的模型文件
	std::vector<std::string> models;
	std::string folderPath = CT2A(m_strModelPath);
	if (folderPath.empty())
	{
		MessageBox(TEXT("请选择有效的文件夹!"), TEXT("错误"), MB_ICONERROR);
		return false;
	}

	// 获取文件夹下的所有文件
	std::vector<std::string> files = getFilesInFolder(folderPath);
	for (const std::string& file : files)
	{
		if (file.find(".gguf") != std::string::npos)
		{
			models.push_back(file);
		}
	}

	// 如果没有模型文件
	if (models.empty())
	{
		MessageBox(TEXT("文件夹下没有模型文件!"), TEXT("错误"), MB_ICONERROR);
		m_strModelPath = _T("");
		return false;
	}

	// 将模型文件添加到下拉框
	m_ComboBox_Models.ResetContent();
	for (const std::string& model : models)
	{
		CString cstr(model.c_str());
		m_ComboBox_Models.AddString(cstr);
	}

	return true;
}



void SettingDlg::OnBnClickedButtonStartServer()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(TRUE);

	// 查看端口号是否合法
	if (m_nPort < 1024 || m_nPort > 65535)
	{
		MessageBox(TEXT("端口号应在1024-65535之间!"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	// 先查看端口是否被占用
	if (!IsPortAvailable(m_nPort))
	{
		CString str;
		str.Format(TEXT("端口 %d 已被占用，请更换端口号"), m_nPort);
		MessageBox(str, L"错误", MB_ICONERROR);
		return;
	}

	// 查看m_strModelPath是否为空
	if (m_strModelPath.IsEmpty())
	{
		MessageBox(TEXT("请选择模型文件夹!"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	// 获取选择的模型
	int index = m_ComboBox_Models.GetCurSel();
	m_ComboBox_Models.GetLBText(index, m_strModel);

	// 查看选择的模型
	if (m_strModel.IsEmpty())
	{
		MessageBox(TEXT("请选择模型文件!"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	// 获取字体大小
	m_nFontSize = m_ComboBox_FontSize.GetCurSel() + 1;

	// 获取字体
	m_FontComboBox.GetLBText(m_FontComboBox.GetCurSel(), m_strFont);

	// 获取背景颜色
	m_clrBackground = m_ColorBtn_Background.GetColor();

	// 获取文本颜色
	m_clrText = m_ColorBtn_Text.GetColor();

	// 获取用户颜色
	m_clrUser = m_ColorBtn_User.GetColor();
	
	// 获取助手颜色
	m_clrAssistant = m_ColorBtn_Assistant.GetColor();

	OnOK();
}


void SettingDlg::OnBnClickedButtonStopServer()
{
	// TODO: 在此添加控件通知处理程序代码

	if (isFirstOpen)
	{
		exit(0);
	}

	OnCancel();
}



void SettingDlg::OnBnClickedButtonHistoryPath()
{
	// TODO: 在此添加控件通知处理程序代码
	// 弹出文件夹选择对话框
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(BROWSEINFO));
	bi.hwndOwner = this->m_hWnd;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = _T("请选择历史记录文件夹");
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if (idl != NULL)
	{
		TCHAR path[MAX_PATH];
		SHGetPathFromIDList(idl, path);
		// 将IDC_EDIT_HISTORY_PATH编辑框的内容设置为path
		m_strHistory = path;
		SetDlgItemText(IDC_EDIT_HISTORY_PATH, path);
	}
}


void SettingDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	if (isFirstOpen)
	{
		exit(0);
	}

	CDialogEx::OnClose();
}
