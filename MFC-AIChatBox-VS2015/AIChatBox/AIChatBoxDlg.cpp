
// AIChatBoxDlg.cpp: 实现文件
//

#include "pch.h"
#include "framework.h"
#include "AIChatBox.h"
#include "AIChatBoxDlg.h"
#include "afxdialogex.h"

#include "SettingDlg.h"
#include "NewChatDlg.h"
#include "PromptDlg.h"
#include "RenameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CAIChatBoxDlg 对话框



CAIChatBoxDlg::CAIChatBoxDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_AICHATBOX_DIALOG, pParent)
	, m_strModelPath(_T(".\\models"))
	, m_nPort(8080)
	, m_strModel(_T(""))
	, m_strFont(_T("Segoe UI Emoji"))
	, m_nFontSize(2)
	, m_clrBackground(RGB(255, 255, 255))
	, m_clrText(RGB(0, 0, 0))
	, m_clrUser(RGB(0, 0, 255))
	, m_clrAssistant(RGB(255, 0, 0))
	, m_isBold(TRUE)
	, m_isItalic(FALSE)
	, m_hProcess(NULL)
	, m_strHistory(_T(".\\history"))
	, m_currentHistory(_T(""))
	, isNeedSave(FALSE)
	, m_nSelectedItem(-1)
	, isFirstOpen(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	user_prompt = u8"AI助手";					// 存储用户输入的提示信息，从 prompts.json 中读取
	chat_template = "";							// 存储聊天模板，从 tempaltes.json 中读取
	param_profile = "creative_oai";				// 存储参数配置文件，从 params.json 中读取
	chat_mode = true;							// 是否启用聊天模式
	chat_guards = true;							// 是否启用聊天保护		
	debug = true;								// 是否开启调试模式
	onceAct = false;							// 控制角色的对话轮次	
}

CAIChatBoxDlg::~CAIChatBoxDlg()
{
	// 关闭服务端进程
	if (m_hProcess) {
		TerminateProcess(m_hProcess, 0);	// 强制关闭进程
		CloseHandle(m_hProcess);			// 关闭进程句柄
	}
}

void CAIChatBoxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_SYSTEM_PROMPT, m_RichEdit_SystemPrompt);
	DDX_Control(pDX, IDC_RICHEDIT_CHAT_HISTORY, m_RichEdit_Dialogue);
	DDX_Control(pDX, IDC_RICHEDIT_USER_INPUT, m_RichEdit_UserInput);
	DDX_Control(pDX, IDC_LIST_HISTORY, m_ListBox_History);
}

BEGIN_MESSAGE_MAP(CAIChatBoxDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_SETTING, &CAIChatBoxDlg::OnBnClickedButtonSetting)
	ON_WM_CLOSE()
	ON_LBN_SELCHANGE(IDC_LIST_HISTORY, &CAIChatBoxDlg::OnLbnSelchangeListHistory)
	ON_BN_CLICKED(IDC_BUTTON_SEND_MESSAGE, &CAIChatBoxDlg::OnBnClickedButtonSendMessage)
	ON_MESSAGE(WM_RESPONSE_RECEIVED, &CAIChatBoxDlg::OnThreadResponse)
	ON_BN_CLICKED(IDC_BUTTON_NEW_CHAT, &CAIChatBoxDlg::OnBnClickedButtonNewChat)
	ON_MESSAGE(WM_UPDATE_TOKEN_COUNT, &CAIChatBoxDlg::OnUpdateTokenCount)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_MESSAGE, &CAIChatBoxDlg::OnBnClickedButtonClearMessage)
	ON_COMMAND(ID_DELETE, &CAIChatBoxDlg::OnListboxDelete)
	ON_WM_CONTEXTMENU()
	ON_BN_CLICKED(IDC_BUTTON_WITHDRAW_MESSAGE, &CAIChatBoxDlg::OnBnClickedButtonWithdrawMessage)
	ON_BN_CLICKED(IDC_BUTTON_SET_PROMPT, &CAIChatBoxDlg::OnBnClickedButtonSetPrompt)
	ON_BN_CLICKED(IDC_BUTTON_ABOUT, &CAIChatBoxDlg::OnBnClickedButtonAbout)
	ON_COMMAND(ID_RENAME, &CAIChatBoxDlg::OnRename)
END_MESSAGE_MAP()


// CAIChatBoxDlg 消息处理程序

BOOL CAIChatBoxDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
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
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	// 设置本对话框图标 (大图标)
	HICON hIconLarge = AfxGetApp()->LoadIcon(IDI_ICON2);
	SendMessage(WM_SETICON, ICON_BIG, (LPARAM)hIconLarge);

	// 设置本对话框图标 (小图标)
	HICON hIconSmall = static_cast<HICON>(
		::LoadImage(AfxGetResourceHandle(),
			MAKEINTRESOURCE(IDI_ICON1),
			IMAGE_ICON,
			16, 16,
			LR_DEFAULTCOLOR)
		);
	SendMessage(WM_SETICON, ICON_SMALL, (LPARAM)hIconSmall);

	// 创建状态栏
	if (!m_StatusBar.Create(WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP, CRect(0, 0, 0, 0), this, 1234))
	{
		AfxMessageBox(L"状态栏创建失败");
		return FALSE;
	}

	// 设置状态栏分区
	int parts[] = { 250, 800, 1100, -1 }; // 2个分区
	m_StatusBar.SetParts(3, parts);
	m_StatusBar.SetText(L" 服务器: 未启动", 0, 0);
	m_StatusBar.SetText(L" 模型: 未选择", 1, 0);
	m_StatusBar.SetText(L" Tokens: 0 t/s", 2, 0);
	SetStatusBar();

	// 加载菜单
	m_Menu.LoadMenuW(IDR_MENU1);

	// 弹出设置对话框
	OnBnClickedButtonSetting();
	isFirstOpen = FALSE;

	if (m_strModel.IsEmpty())
	{
		MessageBox(L"请先到设置中选择模型！", L"错误", MB_ICONWARNING);
	}

	// 设置字体
	SetChatContext();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CAIChatBoxDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CAIChatBoxDlg::OnPaint()
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CAIChatBoxDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CAIChatBoxDlg::SetChatContext()
{
	// 重置聊天上下文
	//chatContext.removeAllActors();
	//chatContext.resetChatHistory();
	
	// 设置聊天保护是否启用
	chatContext.setChatGuards(chat_guards);
	// 设置是否处于聊天模式
	chatContext.setChatMode(chat_mode);

	// 根据调试模式的设置，启用或禁用文件日志记录
	if (!debug)
		Logging::disable_file_logging();

	// 加载聊天模板，如果为空则使用 OAI 完成风格
	// OAI是OpenAI的缩写，是一个人工智能公司
	if (chat_template.empty()) {
		chatContext.using_oai_completion = true;
	}
	else {
		// 尝试加载自定义聊天模板，如果失败则记录错误并暂停
		if (!chatContext.loadChatTemplates(chat_template)) {
			Logging::error("Failed to load the custom chat template '%s'! Using default empty.", chat_template.c_str());
		}
	}

	//// 加载用户提示信息，如果为空则设置系统提示信息和默认角色
	//if (user_prompt.empty()) {
	//	chatContext.setupSystemPrompt("You are a very helpful assistant");
	//	chatContext.setupDefaultActors();
	//}
	//else {

	//	std::string user_prompt_view(user_prompt);

	//	// 尝试加载用户提示信息，如果失败则退出程序
	//	if (!chatContext.loadUserPrompt(user_prompt))
	//	{
	//		MessageBox(TEXT("Failed to load the user prompt!"), TEXT("Error"), MB_ICONERROR);
	//		exit(1);
	//	}
	//}

	// 加载参数配置文件，如果失败则记录错误并退出程序
	if (!chatContext.loadParametersSettings(param_profile)) {
		Logging::error("Failed to load the param profile '%s'!", param_profile.c_str());
		//Terminal::pause();
		exit(1);
	}

	// 配置聊天停止词
	chatContext.setupChatStopWords();

	// 控制角色的对话轮次
	currentActor = chatContext.getAssistantName();
}

void CAIChatBoxDlg::SetEditFont(CRichEditCtrl& edit)
{
	DWORD dwOldStyle = edit.GetExStyle();	//默认包含透明属性
	edit.ModifyStyleEx(dwOldStyle, WS_EX_CLIENTEDGE);	//去除透明属性
	//m_redit.SetBackgroundColor(TRUE, GetSysColor(COLOR_3DFACE));
	edit.SetBackgroundColor(FALSE, m_clrBackground);

	// 设置字体
	CHARFORMAT2 cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_FACE | CFM_SIZE | CFM_BOLD | CFM_ITALIC | CFM_COLOR;
	cf.dwEffects = 0;
	cf.yHeight = 100 * m_nFontSize;
	_tcscpy_s(cf.szFaceName, _countof(cf.szFaceName), m_strFont);
	cf.crTextColor = m_clrText;
	if (m_isBold)
		cf.dwEffects |= CFE_BOLD;
	if (m_isItalic)
		cf.dwEffects |= CFE_ITALIC;
	edit.SetSel(-1, -1);
	edit.SetDefaultCharFormat(cf);
}

void CAIChatBoxDlg::SetStatusBar()
{
	if (m_hProcess)
	{
		CString str;
		str.Format(TEXT(" 服务器已启动, 端口: %d"), m_nPort);
		m_StatusBar.SetText(str, 0, 0);
	}
	else
	{
		m_StatusBar.SetText(L" 服务器未启动", 0, 0);
	}

	if (!m_strModel.IsEmpty())
	{
		CString str;
		str.Format(TEXT(" 模型: %s"), m_strModel);
		m_StatusBar.SetText(str, 1, 0);
	}
	else
	{
		m_StatusBar.SetText(L" 模型: 未选择", 1, 0);
	}

	//CString tokenText;
	//tokenText.Format(L"Tokens: %.1f t/s", *(double*)&wParam);
	//m_StatusBar.SetText(tokenText, 2, 0);

	//return 0;
}

LRESULT CAIChatBoxDlg::OnUpdateTokenCount(WPARAM wParam, LPARAM lParam)
{
	CString tokenText;
	tokenText.Format(L" Tokens: %.1f t/s", *(double*)&wParam);
	m_StatusBar.SetText(tokenText, 2, 0);
	return 0;
}

bool CAIChatBoxDlg::StartServer(bool isModelChanged = false)
{
	if (isModelChanged)	// 模型文件夹路径改变，需要重新启动服务器
	{
		if (m_hProcess)	// 服务器已启动，先关闭服务器
		{
			StopServer();
		}
	}
	
	if (!m_hProcess)	// 服务器未启动
	{	
		// 改进代码：转义路径中的空格
		CString modelPath = m_strModelPath + TEXT("\\") + m_strModel;
		PathQuoteSpaces(modelPath.GetBuffer(MAX_PATH));  // 自动添加双引号, 防止路径中有空格
		modelPath.ReleaseBuffer();

		// 构建命令时直接使用转义后的路径
		CString command;
		command.Format(TEXT(".\\server\\llama-server.exe -m %s --port %d"), modelPath.GetString(), m_nPort);
		// std::wstring command = L"G:\\Desktop\\llama-cpp\\server\\llama-server.exe -m G:\\Desktop\\llama-cpp\\models\\Qwen2.5-7B-Instruct-Uncensored.Q8_0.gguf -c 2048";

		std::wstring cmd = command.GetBuffer();

		// 创建进程信息
		STARTUPINFO si = { sizeof(si) };
		PROCESS_INFORMATION pi = {};

		// 启动进程
		if (CreateProcess(
			nullptr,               // 应用程序名称
			&cmd[0],			   // 命令行
			nullptr,               // 默认进程安全属性
			nullptr,               // 默认线程安全属性
			FALSE,                 // 是否继承句柄
			CREATE_NO_WINDOW,      // 不创建控制台窗口
			nullptr,               // 使用父进程的环境
			nullptr,               // 使用父进程的当前目录
			&si,                   // 启动信息
			&pi                    // 进程信息
		))
		{	
			m_hProcess = pi.hProcess;  // 保存进程句柄以便后续关闭
			CloseHandle(pi.hThread);  // 关闭线程句柄

			MessageBox(L"服务器已启动", L"提示", MB_OK);
			return true;
		}
		else
		{
			DWORD error = GetLastError();
			CString msg;
			msg.Format(L"服务器启动失败 (错误码: %d)", error);
			MessageBox(msg, L"错误", MB_ICONERROR);
		}
	}
	else
	{
		MessageBox(L"服务器已启动", L"提示", MB_OK);
		return true;
	}
}

bool CAIChatBoxDlg::StopServer()
{
	if (m_hProcess)	// 服务器已启动
	{
		// 终止服务器进程
		if (TerminateProcess(m_hProcess, 0))
		{
			CloseHandle(m_hProcess);  // 关闭进程句柄
			m_hProcess = nullptr;
			MessageBox(L"服务器已关闭", L"提示", MB_OK);
			return true;
		}
		else
		{
			MessageBox(L"关闭服务器失败", L"错误", MB_ICONERROR);
			return false;
		}
	}
	else
	{
		MessageBox(L"服务器未启动", L"提示", MB_OK);
		return true;
	}
}

void CAIChatBoxDlg::OnBnClickedButtonSetting()
{
	// TODO: 在此添加控件通知处理程序代码

	SettingDlg dlg;
	bool isModelChanged = false;	// 模型是否改变
	bool isHistoryChanged = false;	// 历史记录文件夹是否改变

	dlg.m_strModelPath = m_strModelPath;
	dlg.m_nPort = m_nPort;
	dlg.m_strModel = m_strModel;
	dlg.m_strFont = m_strFont;
	dlg.m_nFontSize = m_nFontSize;
	dlg.m_clrBackground = m_clrBackground;
	dlg.m_clrText = m_clrText;
	dlg.m_clrUser = m_clrUser;
	dlg.m_clrAssistant = m_clrAssistant;
	dlg.m_isBold = m_isBold;
	dlg.m_isItalic = m_isItalic;
	dlg.m_strHistory = m_strHistory;
	dlg.isFirstOpen = isFirstOpen;

	if (dlg.DoModal() == IDOK)
	{
		m_strModelPath = dlg.m_strModelPath;
		m_nPort = dlg.m_nPort;
		if (m_strModel != dlg.m_strModel) isModelChanged = true;	// 模型文件夹路径改变
		m_strModel = dlg.m_strModel;
		m_strFont = dlg.m_strFont;
		m_nFontSize = dlg.m_nFontSize;
		m_clrBackground = dlg.m_clrBackground;
		m_clrText = dlg.m_clrText;
		m_clrUser = dlg.m_clrUser;
		m_clrAssistant = dlg.m_clrAssistant;
		m_isBold = dlg.m_isBold;
		m_isItalic = dlg.m_isItalic;
		if (m_strHistory != dlg.m_strHistory) isHistoryChanged = true;	// 历史记录文件夹改变
		m_strHistory = dlg.m_strHistory;

		// 设置字体
		SetEditFont(m_RichEdit_SystemPrompt);
		SetEditFont(m_RichEdit_Dialogue);
		SetEditFont(m_RichEdit_UserInput);

		// 设置用户和助手颜色
		chatContext.setUserAndAssistantColor(m_clrUser, m_clrAssistant);

		// 启动服务器
		StartServer(isModelChanged);
		// 更新状态栏
		SetStatusBar();
		// 加载历史记录
		LoadHistoryFromFolder(isHistoryChanged);
		// 绘制对话
		DrawDialogue();		
	}
}


void CAIChatBoxDlg::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// 保存历史记录
	if (isNeedSave)
	{
		SaveHistory();
		//MessageBox(TEXT("Conversation saved!"), TEXT("Info"), MB_ICONINFORMATION);
	}

	// 关闭服务器
	StopServer();

	CDialogEx::OnClose();
}

bool CAIChatBoxDlg::LoadHistoryFromFolder(bool isHistoryChanged)
{
	// 获取文件夹下的所有文件
	//if (!isHistoryChanged)
	//{
	//	return false;
	//}

	m_History.clear();	// 清空历史记录
	m_ListBox_History.ResetContent();	// 清空列表框

	CFileFind finder;
	BOOL bWorking = finder.FindFile(m_strHistory + TEXT("\\*.json"));
	while (bWorking)
	{
		bWorking = finder.FindNextFile();
		CString filename = finder.GetFileName();
		m_History.push_back(filename);
	}
	finder.Close();
	// 将文件名添加到列表框
	for (const CString& file : m_History)
	{
		m_ListBox_History.AddString(file);
	}

	return true;
}


void CAIChatBoxDlg::OnLbnSelchangeListHistory()
{
	// TODO: 在此添加控件通知处理程序代码
	
	if (isNeedSave)
	{
		SaveHistory();
		//MessageBox(TEXT("Conversation saved!"), TEXT("Info"), MB_ICONINFORMATION);
	}


	// 获取选中的文件名
	int index = m_ListBox_History.GetCurSel();
	CString filename;
	m_ListBox_History.GetText(index, filename);
	// 读取文件内容
	CString filepath = m_strHistory + TEXT("\\") + filename;

	std::string strfilename = CT2A(filepath);

	// 使用loadSavedConversation
	if (chatContext.loadSavedConversation(strfilename))
	{
		SetChatContext();
		Logging::success("Conversation from '%s' loaded.", strfilename.c_str());
		chatContext.completionBus.buffer.clear();   // 清空完成操作的回调总线缓冲区
		DrawSystemPrompt();	// 绘制系统提示
		DrawDialogue();		// 绘制对话
		m_currentHistory = filename;	// 设置当前历史记录
		isNeedSave = false;	// 不需要保存
	}
	else
	{
		Logging::error("Failed to load conversation!");
		MessageBox(TEXT("Failed to load conversation!"), TEXT("Error"), MB_ICONERROR);
	}
}

bool CAIChatBoxDlg::SaveHistory()
{
	// 获取当前时间
	SYSTEMTIME sysTime;
	// 生成文件名
	CString filename = m_strHistory + TEXT("\\") + m_currentHistory;	// 保存为当前历史记录的文件名
	std::string strfilename = CT2A(filename);
	// 保存对话
	if (chatContext.saveConversation(strfilename))
	{
		Logging::success("Conversation saved to '%s'.", strfilename.c_str());
		// 重新加载历史记录
		// LoadHistoryFromFolder(true);
		return true;
	}
	else
	{
		Logging::error("Failed to save conversation!");
		MessageBox(TEXT("Failed to save conversation!"), TEXT("Error"), MB_ICONERROR);
		return false;
	}
}

void CAIChatBoxDlg::DrawSystemPrompt()
{
	chatContext.drawSystem(m_RichEdit_SystemPrompt);	// 绘制系统提示
}

void CAIChatBoxDlg::DrawDialogue()
{
	chatContext.draw(m_RichEdit_Dialogue);	// 绘制对话
}


void CAIChatBoxDlg::OnBnClickedButtonSendMessage()
{
	// TODO: 在此添加控件通知处理程序代码
	
	// 在函数开头添加等待光标
	//CWaitCursor wait; // 自动显示沙漏图标
	//Sleep(5000); // 模拟耗时操作

	// 检查线程是否正在运行
	if (m_threadRunning)
	{
		MessageBox(TEXT("请求已在进行中，请稍候"), TEXT("提示"), MB_ICONINFORMATION);
		return;
	}

	// 检查是否启动了服务器
	if (!m_hProcess)
	{
		MessageBox(TEXT("请先启动服务器"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	// 检查m_currentHistory是否为空
	if (m_currentHistory.IsEmpty())
	{
		MessageBox(TEXT("请先选择一条记录"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	// 控制当前说话的角色，如果 onceAct 为真，则使用之前的角色
	if (onceAct) {
		currentActor = previousActingActor;
		onceAct = false;
	}
	else {
		previousActingActor = currentActor;
	}

	// 获取用户输入
	m_RichEdit_UserInput.GetWindowText(userInput);
	if (userInput.IsEmpty())
	{
		MessageBox(TEXT("Please enter a message!"), TEXT("Error"), MB_ICONERROR);
		return;
	}
	else
	{
		// 清空用户输入
		m_RichEdit_UserInput.SetWindowText(TEXT(""));
	}

	// 禁用按钮，避免重复操作
	GetDlgItem(IDC_BUTTON_SEND_MESSAGE)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_WITHDRAW_MESSAGE)->EnableWindow(FALSE);
	m_threadRunning = true;

	// 打印用户的聊天标签
	chatContext.printActorChaTag(chatContext.getUserName(), m_RichEdit_Dialogue, m_clrUser);

	// 显示用户输入
	m_RichEdit_Dialogue.ReplaceSel(userInput + TEXT("\r\n"));
	chatContext.addNewMessage(chatContext.getUserName(), ConvertCStringToUtf8(userInput));

	// 打印当前角色的聊天标签
	chatContext.printActorChaTag(currentActor, m_RichEdit_Dialogue, m_clrAssistant);

	UpdateData(FALSE);

	chatContext.completionBus.buffer.clear();   // 清空完成操作的回调总线缓冲区

	// 启动线程
	m_pThread = AfxBeginThread(ThreadFunction, this);

	// 保存对话
	isNeedSave = true;
}

UINT CAIChatBoxDlg::ThreadFunction(LPVOID pParam)
{
	auto* pThis = reinterpret_cast<CAIChatBoxDlg*>(pParam);

	// 从界面获取用户输入和其他信息
	const char* ip = "127.0.0.1";	// 这里是本地地址
	int16_t port = pThis->m_nPort;	// 从设置对话框获取端口号

	// 发送完成请求到服务器
	Response res = pThis->chatContext.requestCompletion(ip, port, pThis->chatContext.getCurrentPrompt(), pThis->m_RichEdit_Dialogue);

	// 在主线程中更新UI
	pThis->PostMessage(WM_RESPONSE_RECEIVED, reinterpret_cast<WPARAM>(new Response(res))); // 自定义消息

	return 0;
}

void CAIChatBoxDlg::HandleServerResponse(const Response& res)
{
	if (res.Status != HTTP_OK)
	{
		// HandleServerError(res); // 错误处理
		MessageBox(TEXT("网络错误！"));
	}
	else
	{
		chatContext.cureCompletionForChat();	// 清理聊天完成相关的缓冲区
		if (chatContext.using_oai_completion)
		{
			chatContext.addNewMessage(currentActor, "");
		}
		// 将服务器响应添加到聊天上下文
		chatContext.updateMessageContent(chatContext.messagesCount() - 1, chatContext.completionBus.buffer);
	}
	HandleThreadCompletion();	// 处理线程完成
	m_RichEdit_Dialogue.ReplaceSel(TEXT("\r\n"));	// 换行
}

void CAIChatBoxDlg::HandleThreadCompletion()
{
	m_threadRunning = false; // 线程完成

	GetDlgItem(IDC_BUTTON_SEND_MESSAGE)->EnableWindow(TRUE); // 启用按钮
	GetDlgItem(IDC_BUTTON_WITHDRAW_MESSAGE)->EnableWindow(TRUE); // 启用按钮
}

LRESULT CAIChatBoxDlg::OnThreadResponse(WPARAM wParam, LPARAM lParam)
{
	// 获取线程传递的响应对象
	std::unique_ptr<Response> pResponse(reinterpret_cast<Response*>(wParam));	// 转换为Response对象
	if (pResponse)
	{
		HandleServerResponse(*pResponse); // 处理服务器响应
	}
	return 0;
}


void CAIChatBoxDlg::OnBnClickedButtonNewChat()
{
	// TODO: 在此添加控件通知处理程序代码

	// 先保存当前对话
	if (isNeedSave)
	{
		SaveHistory();
		//MessageBox(TEXT("Conversation saved!"), TEXT("Info"), MB_ICONINFORMATION);
	}

	NewChatDlg dlg;
	dlg.chatContext = &chatContext;
	if (dlg.DoModal() == IDOK)
	{
		// 在m_strHistory路径下创建一个新的json文件
		CString filename = m_strHistory + TEXT("\\") + dlg.m_strChatName + TEXT(".json");
		m_currentHistory = dlg.m_strChatName + TEXT(".json");
		std::string strfilename = CT2A(filename);

		// 保存对话
		if (chatContext.saveConversation(strfilename))
		{
			Logging::success("Conversation saved to '%s'.", strfilename.c_str());
			// 重新加载历史记录
			LoadHistoryFromFolder(true);
		}
		else
		{
			Logging::error("Failed to save conversation!");
			MessageBox(TEXT("Failed to save conversation!"), TEXT("Error"), MB_ICONERROR);
		}

		// ListBox中选择filename
		for (int i = 0; i < m_History.size(); i++)
		{
			if (m_History[i] == dlg.m_strChatName + TEXT(".json"))
			{
				m_ListBox_History.SetCurSel(i);
				OnLbnSelchangeListHistory();
				break;
			}
		}

		// 设置聊天上下文
		currentActor = chatContext.getAssistantName();
	}
}


BOOL CAIChatBoxDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	// 按下回车键发送消息
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		// 如果焦点在编辑控件上，处理回车键
		if (GetFocus() == &m_RichEdit_UserInput)
		{
			OnBnClickedButtonSendMessage();	// 发送消息
		}
		return TRUE; // 阻止消息继续传递
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


void CAIChatBoxDlg::OnBnClickedButtonClearMessage()
{
	// TODO: 在此添加控件通知处理程序代码
	m_RichEdit_UserInput.SetWindowText(TEXT(""));
}


void CAIChatBoxDlg::OnListboxDelete()
{
	// TODO: 在此添加命令处理程序代码
	// 删除历史记录
	if (m_nSelectedItem != -1)
	{
		// 弹出确认对话框
		if (MessageBox(TEXT("确定要删除吗？"), TEXT("提示"), MB_YESNO | MB_ICONQUESTION) == IDNO)
		{
			return;
		}
		
		CString filename;
		m_ListBox_History.GetText(m_nSelectedItem, filename);
		CString filepath = m_strHistory + TEXT("\\") + filename;

		if (!PathFileExists(filepath)) {
			MessageBox(L"文件不存在！", L"错误", MB_ICONERROR);
			return;
		}

		DeleteFile(filepath);
		// 重新加载历史记录
		LoadHistoryFromFolder(true);
	}
}


void CAIChatBoxDlg::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: 在此处添加消息处理程序代码
	// 判断是否在我们的 ListBox 中右键
	if (pWnd->GetDlgCtrlID() == IDC_LIST_HISTORY) // 你的 ListBox ID
	{
		// 先把屏幕坐标转换为 ListBox 客户区坐标
		CPoint ptClient = point;
		m_ListBox_History.ScreenToClient(&ptClient);

		// 判断点击到了哪一行
		BOOL bOutside = FALSE;
		int nIndex = m_ListBox_History.ItemFromPoint(ptClient, bOutside);

		// 如果在某行上（而不是空白区域），则弹出菜单
		if (!bOutside && nIndex != LB_ERR)
		{
			// 记住要删除的行，用成员变量或其他方式保存
			m_nSelectedItem = nIndex;

			// 弹出菜单
			CMenu* pMenu = m_Menu.GetSubMenu(0); // 你的菜单
			pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);

			// 使得右键菜单消失后，ListBox 不再选中任何项
			m_ListBox_History.SetCurSel(-1);

			return;
		}
	}
	else
	{
		// 如果并不是在 ListBox 上右键，那就交给默认处理
		CDialogEx::OnContextMenu(pWnd, point);
	}
}


void CAIChatBoxDlg::OnBnClickedButtonWithdrawMessage()
{
	// TODO: 在此添加控件通知处理程序代码
	
	// 检查m_currentHistory是否为空
	if (m_currentHistory.IsEmpty())
	{
		MessageBox(TEXT("请先选择一条记录"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	// 撤销上一次完成和用户消息
	chatContext.removeLastMessage(2);
	DrawDialogue();

	// 保存对话
	isNeedSave = true;
}

void CAIChatBoxDlg::OnBnClickedButtonSetPrompt()
{
	// TODO: 在此添加控件通知处理程序代码

	// 检查m_currentHistory是否为空
	if (m_currentHistory.IsEmpty())
	{
		MessageBox(TEXT("请先选择一条记录"), TEXT("错误"), MB_ICONERROR);
		return;
	}

	PromptDlg dlg;
	std::string systemPrompt = chatContext.getSystemPrompt();
	dlg.m_strPrompt = ConvertUtf8ToCString(systemPrompt);
	if (dlg.DoModal() == IDOK)
	{
		// 更新系统提示
		systemPrompt = ConvertCStringToUtf8(dlg.m_strPrompt);
		chatContext.updateSystemPrompt(systemPrompt);
		DrawSystemPrompt();
		// 保存对话
		isNeedSave = true;
	}
}


void CAIChatBoxDlg::OnBnClickedButtonAbout()
{
	// TODO: 在此添加控件通知处理程序代码
	// 弹出关于对话框
	CAboutDlg dlg;
	dlg.DoModal();
}


void CAIChatBoxDlg::OnRename()
{
	// TODO: 在此添加命令处理程序代码
	// 重命名历史记录
	if (m_nSelectedItem != -1)
	{
		// 获取选中的文件名
		CString filename;
		m_ListBox_History.GetText(m_nSelectedItem, filename);
		CString filepath = m_strHistory + TEXT("\\") + filename;

		if (!PathFileExists(filepath)) {
			MessageBox(L"文件不存在！", L"错误", MB_ICONERROR);
			return;
		}

		// 获取不带后缀的文件名
		filename = filename.Left(filename.GetLength() - 5);

		// 弹出重命名对话框
		RenameDlg dlg;
		dlg.m_name = filename;
		if (dlg.DoModal() == IDOK)
		{
            // 获取新的文件名
            CString newFilename = dlg.m_name + TEXT(".json");
            CString newFilepath = m_strHistory + TEXT("\\") + newFilename;

            // 重命名文件
            if (MoveFile(filepath, newFilepath))
            {
				// 更新列表框中的文件名
				m_ListBox_History.DeleteString(m_nSelectedItem);
				m_ListBox_History.InsertString(m_nSelectedItem, newFilename);
				m_ListBox_History.SetCurSel(m_nSelectedItem);

				// 更新当前历史记录文件名
				if (m_currentHistory == filename + TEXT(".json"))
				{
					m_currentHistory = newFilename;
				}
				MessageBox(TEXT("重命名成功"), TEXT("提示"), MB_OK);
            }
            else
            {
				MessageBox(TEXT("重命名失败"), TEXT("错误"), MB_ICONERROR);
            }
		}
	}
}
