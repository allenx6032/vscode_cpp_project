#ifndef __COMPLETION_H__
#define __COMPLETION_H__

#include <iostream>
#include <string>

#include "sjson.h"
#include "utils.h"
#include "minipost.h"
//#include "terminal.h"
#include <csignal>
#include <string>

// 默认的 IP 地址
#define DEFAULT_IP "127.0.0.1"
// 默认的端口号
#define DEFAULT_PORT 8080
// 完成操作的端点路径
#define COMPLETION_ENDPOINT     "/completion"
// OAI 完成操作的端点路径
#define OAI_COMPLETION_ENDPOINT "/v1/chat/completions"
// 参数配置文件的名称
#define PARAMS_FILENAME    ".\\config\\params.json"
// 模板文件的名称
#define TEMPLATES_FILENAME ".\\config\\templates.json"

// 外部声明的标志，用于停止完成操作
extern bool stopCompletionFlag;
// 外部声明的标志，用于表示完成操作正在进行中
extern bool completionInProgress;

// 存储聊天模板信息的结构体
struct chat_template_t {
    // 提示信息的类型
    std::string prompt_type;
    // 字符串的开始部分
    std::string bos;
    // 系统部分的开始
    std::string begin_system;
    // 系统部分的结束
    std::string end_system;
    // 用户部分的开始
    std::string begin_user;
    // 用户部分的结束
    std::string end_user;
    // 助手部分的开始
    std::string begin_assistant;
    // 助手部分的结束
    std::string end_assistant;
    // 字符串的结束部分
    std::string eos;
};

// 完成操作的信号处理函数
// 当接收到中断信号时，如果完成操作正在进行，则设置停止标志并重置信号处理方式
static void completionSignalHandler(int signum) {
    if (completionInProgress) {
        stopCompletionFlag = true;
        signal(SIGINT, SIG_DFL); // 重置信号处理方式
    }
}

class Completion {
public:
    // 构造函数，初始化完成操作的回调总线的缓冲区为空，并设置为流式处理
    Completion() {
        completionBus.buffer = "";
        completionBus.stream = true;

        cf2.dwMask |= CFM_UNDERLINE | CFM_BOLD | CFM_ITALIC;
        cf2.dwEffects |= CFE_BOLD;
        cf2.dwEffects &= 0xBFFFFFFF;
        _tcscpy_s(cf2.szFaceName, _countof(cf2.szFaceName), _T("Segoe UI Emoji"));
        cf2.yHeight = 200;
        cf2.crTextColor = RGB(190, 160, 250);
    };

    // 完成操作的回调函数，接收一个字符串块和回调总线指针，根据实现决定如何处理该字符串块
    bool completionCallback(const std::string& chunk, const CallbackBus* bus);
    bool completionCallback(const std::string& chunk, const CallbackBus* bus, CEdit& edit);
    bool completionCallback(const std::string& chunk, const CallbackBus* bus, CRichEditCtrl& edit);

    // 加载参数设置，根据提供的配置文件名称加载参数设置
    bool loadParametersSettings(std::string profile_name);

    // 将提示信息的 JSON 数据转储为字符串
    std::string dumpPayload(yyjson_mut_doc* prompt_json);

    // 发送完成请求，向指定的 IP 地址和端口发送请求，并携带提示信息的 JSON 数据
    Response requestCompletion(const char* ipaddr, const int16_t port, yyjson_mut_doc* prompt_json, CRichEditCtrl& edit);

    // 获取聊天模板
    chat_template_t& getChatTemplates();

    // 向停止词列表中添加一个停止词
    void addStopWord(std::string word);

    // 加载聊天模板，根据提供的模板名称加载相应的模板
    bool loadChatTemplates(std::string chat_template_name);

    // 完成操作的回调总线，用于存储和处理完成操作的结果
    CallbackBus completionBus;
    // 标记是否使用 OAI 完成风格
    bool using_oai_completion = false;

private:
    // HTTP 请求对象，用于发送 HTTP 请求
    httpRequest Req;
    // 存储聊天模板的实例
    chat_template_t chat_template = { "", "\n", "", "\n", "", "\n" };
    // 存储参数的字典，用于存储各种参数
    Dict<std::string> parameters;
    // 存储停止词的向量，用于存储停止词列表
    std::vector<std::string> stop_words;

    CHARFORMAT2 cf2;
};
#endif