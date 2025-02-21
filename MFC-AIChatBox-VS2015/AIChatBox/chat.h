#ifndef __CHAT_H__
#define __CHAT_H__
#include <iostream>
#include <string>
#include <vector>
#include <csignal>
#include <filesystem>
#include <algorithm>

#include "sjson.h" 
#include "completion.h"
#include "logging.h"
#include "utils.h"

#define DEFAULT_FILE_EXTENSION ".json"
#define USER_PROMPT_FILE ".\\config\\prompts.json"

//////////////////////////////////////////////
// 定义角色结构体
struct actor_t {
    std::string name;           // 角色名称
    std::string role;           // 角色职位
    std::string tag_color;      // 角色标签颜色
    std::string msg_preffix;    // 角色消息前缀
    std::string icon;           // 角色图标
};

// 定义消息条目结构体
struct message_entry_t {
    message_entry_t(int64_t id, actor_t* actor_info, std::string content) : id(id), actor_info(actor_info), content(content) {}
    int64_t id;                 // 消息ID
    actor_t* actor_info;        // 发送消息的角色信息
    std::string content;        // 消息内容
};

typedef std::vector<message_entry_t> messages_t;    // 消息列表类型

typedef std::map<std::string, actor_t> actors_t;    // 角色映射表类型


/* 所有与聊天提示相关的内容都在这个类中 */
class Chat : public Completion {
public:
    Chat() {};

    // 添加新消息
    void addNewMessage(std::string actor_name, std::string content);

    // 删除最后n条消息
    bool removeLastMessage(int nmessages);

    // 更新消息内容
    void updateMessageContent(int number, std::string newContent);

    // 重置聊天历史记录
    void resetChatHistory();

    // 绘制聊天界面
    void draw();
    void drawSystem(CRichEditCtrl& edit);
    void drawSystem(CEdit& edit);
    void draw(CEdit& edit);
    void draw(CRichEditCtrl& edit);

    // 加载所有的提示
    bool loadAllPrompts(std::vector<CString>& prompts);

    // 加载用户提示
    bool loadUserPrompt(std::string prompt_name);

	// 获取系统提示
	std::string getSystemPrompt();

    // 设置系统提示
    void setupSystemPrompt(std::string prompt);

    // 更新系统提示
    void updateSystemPrompt(std::string new_system_prompt);

    // 设置默认角色
    void setupDefaultActors();

    // 设置用户和助手名称
    void setUserAndAssistantName(std::string userName, std::string assistantName);
	// 设置用户和助手颜色
	void setUserAndAssistantColor(COLORREF userColor, COLORREF assistantColor);

    // 加载保存的对话
    bool loadSavedConversation(std::string file_path);

    // 保存对话
    bool saveConversation(std::string filename);

    // 打印角色聊天标签
    void printActorChaTag(std::string actor_name);
    void printActorChaTag(std::string actor_name, CEdit& edit);
    void printActorChaTag(std::string actor_name, CRichEditCtrl& edit, COLORREF color);

    // 添加角色
    bool addActor(std::string name, const char* role, const char* tag_color, std::string preffix = "", std::string icon = "");

    // 获取用户名称
    std::string& getUserName();
    // 获取助手名称
    std::string& getAssistantName();
	// 获取用户图标
	std::string& getUserIcon();
	// 获取助手图标
	std::string& getAssistantIcon();

    // 移除所有角色
    void removeAllActors();

    // 为聊天设置自动补全
    void cureCompletionForChat();

    // 设置聊天停用词
    void setupChatStopWords();

    // 获取消息数量
    int messagesCount();

    // 列出当前角色
    void listCurrentActors();

    // 组合提示
    std::string composePrompt();

    // 获取提示的 JSON 文档
    yyjson_mut_doc* getPromptJSON();

    // 获取 OAIPrompt 的 JSON 文档
    yyjson_mut_doc* getOAIPrompt();

    // 设置聊天模式
    bool setChatMode(bool value);

    // 判断是否为聊天模式
    bool isChatMode();

    // 设置聊天守卫
    bool setChatGuards(bool value);

    // 获取当前提示的 JSON 文档
    yyjson_mut_doc* getCurrentPrompt();

private:
    messages_t messages;                // 消息列表

    bool chat_guards = true;            // 聊天守卫开关
    bool chat_mode = true;              // 聊天模式开关
    bool using_system_prompt = false;   // 是否使用系统提示
    std::string user_name;              // 用户名称
    std::string assistant_name;         // 助手名称
	std::string user_icon;              // 用户图标
	std::string assistant_icon;         // 助手图标
    actors_t actors;                    // 角色映射表

	COLORREF user_color;                // 用户颜色
	COLORREF assistant_color;           // 助手颜色
};
#endif