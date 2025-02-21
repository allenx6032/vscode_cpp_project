
// AIChatBoxDlg.h: 头文件
//

#pragma once
#include "chat.h"
#define WM_UPDATE_TOKEN_COUNT (WM_USER + 100)	// 自定义消息，用于更新token数
#define WM_RESPONSE_RECEIVED (WM_USER + 101)	// 自定义消息，用于处理服务器响应

// CAIChatBoxDlg 对话框
class CAIChatBoxDlg : public CDialogEx
{
// 构造
public:
	CAIChatBoxDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 析构
	virtual ~CAIChatBoxDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AICHATBOX_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

private:

	Chat chatContext;						// 聊天上下文
	void SetChatContext();					// 设置聊天上下文
	std::string user_prompt;				// 存储用户输入的提示信息，从 prompts.json 中读取
	std::string chat_template;				// 存储聊天模板，从 tempaltes.json 中读取
	std::string param_profile;				// 存储参数配置文件，从 params.json 中读取
	bool chat_mode;							// 是否启用聊天模式
	bool chat_guards;						// 是否启用聊天保护
	bool debug;								// 是否开启调试模式
	bool onceAct;							// 控制角色的对话轮次
	std::string previousActingActor;		// 上一个角色
	std::string currentActor;				// 当前角色

	CString m_strModelPath;					// 模型文件夹路径
	int m_nPort;							// 端口号
	CString m_strModel;						// 模型文件名

	CString m_strFont;						// 字体
	int m_nFontSize;						// 字体大小
	COLORREF m_clrBackground;				// 背景颜色
	COLORREF m_clrText;						// 文本颜色
	COLORREF m_clrUser;						// 用户颜色
	COLORREF m_clrAssistant;				// 助手颜色
	BOOL m_isBold;							// 是否加粗
	BOOL m_isItalic;						// 是否斜体
	void SetEditFont(CRichEditCtrl& edit);	// 设置编辑框字体

	CStatusBarCtrl m_StatusBar;				// 状态栏
	void SetStatusBar();					// 设置状态栏
	afx_msg LRESULT OnUpdateTokenCount(WPARAM wParam, LPARAM lParam);	// 更新token数

	HANDLE m_hProcess;						// 服务端进程句柄
	bool StartServer(bool isModelChanged);	// 启动服务端
	bool StopServer();						// 停止服务端

	bool isFirstOpen;						// 是否第一次打开

public:

	CRichEditCtrl m_RichEdit_SystemPrompt;					// 系统提示
	CRichEditCtrl m_RichEdit_Dialogue;						// 对话	
	CRichEditCtrl m_RichEdit_UserInput;						// 用户输入
	CString userInput;										// 用户输入
	void DrawSystemPrompt();								// 绘制系统提示
	void DrawDialogue();									// 绘制对话

	CString m_strHistory;									// 历史记录
	CListBox m_ListBox_History;								// 历史记录列表框
	std::vector<CString> m_History;							// 历史记录
	bool LoadHistoryFromFolder(bool isHistoryChanged);		// 从文件夹加载历史记录
	afx_msg void OnLbnSelchangeListHistory();				// 历史记录列表框选择改变
	CString m_currentHistory;								// 当前历史记录
	bool isNeedSave;										// 是否需要保存历史记录
	bool SaveHistory();										// 保存历史记录
	int m_nSelectedItem;									// 选中的历史记录
	CMenu m_Menu;											// 右键菜单
	afx_msg void OnListboxDelete();							// 删除历史记录
	afx_msg void OnRename();								// 重命名
	
	CWinThread* m_pThread;									// 用于服务器通信的工作线程
	bool m_threadRunning;									// 标记线程是否正在运行
	static UINT ThreadFunction(LPVOID pParam);				// 线程函数
	void HandleServerResponse(const Response& res);			// 处理服务器响应
	void HandleThreadCompletion();							// 线程完成后更新UI
	LRESULT OnThreadResponse(WPARAM wParam, LPARAM lParam); // 线程消息处理函数
	
	virtual BOOL PreTranslateMessage(MSG* pMsg);			// 拦截回车键
	
	// 消息映射
	afx_msg void OnBnClickedButtonSetting();				// 设置按钮	
	afx_msg void OnClose();									// 关闭窗口
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);	// 右键菜单
	afx_msg void OnBnClickedButtonSendMessage();			// 发送消息
	afx_msg void OnBnClickedButtonNewChat();				// 新建对话
	afx_msg void OnBnClickedButtonClearMessage();			// 清空消息
	afx_msg void OnBnClickedButtonWithdrawMessage();		// 撤回消息
	afx_msg void OnBnClickedButtonSetPrompt();				// 设置提示
	afx_msg void OnBnClickedButtonAbout();					// 关于按钮

};
