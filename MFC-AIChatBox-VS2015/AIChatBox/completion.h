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

// Ĭ�ϵ� IP ��ַ
#define DEFAULT_IP "127.0.0.1"
// Ĭ�ϵĶ˿ں�
#define DEFAULT_PORT 8080
// ��ɲ����Ķ˵�·��
#define COMPLETION_ENDPOINT     "/completion"
// OAI ��ɲ����Ķ˵�·��
#define OAI_COMPLETION_ENDPOINT "/v1/chat/completions"
// ���������ļ�������
#define PARAMS_FILENAME    ".\\config\\params.json"
// ģ���ļ�������
#define TEMPLATES_FILENAME ".\\config\\templates.json"

// �ⲿ�����ı�־������ֹͣ��ɲ���
extern bool stopCompletionFlag;
// �ⲿ�����ı�־�����ڱ�ʾ��ɲ������ڽ�����
extern bool completionInProgress;

// �洢����ģ����Ϣ�Ľṹ��
struct chat_template_t {
    // ��ʾ��Ϣ������
    std::string prompt_type;
    // �ַ����Ŀ�ʼ����
    std::string bos;
    // ϵͳ���ֵĿ�ʼ
    std::string begin_system;
    // ϵͳ���ֵĽ���
    std::string end_system;
    // �û����ֵĿ�ʼ
    std::string begin_user;
    // �û����ֵĽ���
    std::string end_user;
    // ���ֲ��ֵĿ�ʼ
    std::string begin_assistant;
    // ���ֲ��ֵĽ���
    std::string end_assistant;
    // �ַ����Ľ�������
    std::string eos;
};

// ��ɲ������źŴ�����
// �����յ��ж��ź�ʱ�������ɲ������ڽ��У�������ֹͣ��־�������źŴ���ʽ
static void completionSignalHandler(int signum) {
    if (completionInProgress) {
        stopCompletionFlag = true;
        signal(SIGINT, SIG_DFL); // �����źŴ���ʽ
    }
}

class Completion {
public:
    // ���캯������ʼ����ɲ����Ļص����ߵĻ�����Ϊ�գ�������Ϊ��ʽ����
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

    // ��ɲ����Ļص�����������һ���ַ�����ͻص�����ָ�룬����ʵ�־�����δ�����ַ�����
    bool completionCallback(const std::string& chunk, const CallbackBus* bus);
    bool completionCallback(const std::string& chunk, const CallbackBus* bus, CEdit& edit);
    bool completionCallback(const std::string& chunk, const CallbackBus* bus, CRichEditCtrl& edit);

    // ���ز������ã������ṩ�������ļ����Ƽ��ز�������
    bool loadParametersSettings(std::string profile_name);

    // ����ʾ��Ϣ�� JSON ����ת��Ϊ�ַ���
    std::string dumpPayload(yyjson_mut_doc* prompt_json);

    // �������������ָ���� IP ��ַ�Ͷ˿ڷ������󣬲�Я����ʾ��Ϣ�� JSON ����
    Response requestCompletion(const char* ipaddr, const int16_t port, yyjson_mut_doc* prompt_json, CRichEditCtrl& edit);

    // ��ȡ����ģ��
    chat_template_t& getChatTemplates();

    // ��ֹͣ���б������һ��ֹͣ��
    void addStopWord(std::string word);

    // ��������ģ�壬�����ṩ��ģ�����Ƽ�����Ӧ��ģ��
    bool loadChatTemplates(std::string chat_template_name);

    // ��ɲ����Ļص����ߣ����ڴ洢�ʹ�����ɲ����Ľ��
    CallbackBus completionBus;
    // ����Ƿ�ʹ�� OAI ��ɷ��
    bool using_oai_completion = false;

private:
    // HTTP ����������ڷ��� HTTP ����
    httpRequest Req;
    // �洢����ģ���ʵ��
    chat_template_t chat_template = { "", "\n", "", "\n", "", "\n" };
    // �洢�������ֵ䣬���ڴ洢���ֲ���
    Dict<std::string> parameters;
    // �洢ֹͣ�ʵ����������ڴ洢ֹͣ���б�
    std::vector<std::string> stop_words;

    CHARFORMAT2 cf2;
};
#endif