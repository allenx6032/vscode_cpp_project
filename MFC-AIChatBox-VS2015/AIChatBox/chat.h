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
// �����ɫ�ṹ��
struct actor_t {
    std::string name;           // ��ɫ����
    std::string role;           // ��ɫְλ
    std::string tag_color;      // ��ɫ��ǩ��ɫ
    std::string msg_preffix;    // ��ɫ��Ϣǰ׺
    std::string icon;           // ��ɫͼ��
};

// ������Ϣ��Ŀ�ṹ��
struct message_entry_t {
    message_entry_t(int64_t id, actor_t* actor_info, std::string content) : id(id), actor_info(actor_info), content(content) {}
    int64_t id;                 // ��ϢID
    actor_t* actor_info;        // ������Ϣ�Ľ�ɫ��Ϣ
    std::string content;        // ��Ϣ����
};

typedef std::vector<message_entry_t> messages_t;    // ��Ϣ�б�����

typedef std::map<std::string, actor_t> actors_t;    // ��ɫӳ�������


/* ������������ʾ��ص����ݶ���������� */
class Chat : public Completion {
public:
    Chat() {};

    // �������Ϣ
    void addNewMessage(std::string actor_name, std::string content);

    // ɾ�����n����Ϣ
    bool removeLastMessage(int nmessages);

    // ������Ϣ����
    void updateMessageContent(int number, std::string newContent);

    // ����������ʷ��¼
    void resetChatHistory();

    // �����������
    void draw();
    void drawSystem(CRichEditCtrl& edit);
    void drawSystem(CEdit& edit);
    void draw(CEdit& edit);
    void draw(CRichEditCtrl& edit);

    // �������е���ʾ
    bool loadAllPrompts(std::vector<CString>& prompts);

    // �����û���ʾ
    bool loadUserPrompt(std::string prompt_name);

	// ��ȡϵͳ��ʾ
	std::string getSystemPrompt();

    // ����ϵͳ��ʾ
    void setupSystemPrompt(std::string prompt);

    // ����ϵͳ��ʾ
    void updateSystemPrompt(std::string new_system_prompt);

    // ����Ĭ�Ͻ�ɫ
    void setupDefaultActors();

    // �����û�����������
    void setUserAndAssistantName(std::string userName, std::string assistantName);
	// �����û���������ɫ
	void setUserAndAssistantColor(COLORREF userColor, COLORREF assistantColor);

    // ���ر���ĶԻ�
    bool loadSavedConversation(std::string file_path);

    // ����Ի�
    bool saveConversation(std::string filename);

    // ��ӡ��ɫ�����ǩ
    void printActorChaTag(std::string actor_name);
    void printActorChaTag(std::string actor_name, CEdit& edit);
    void printActorChaTag(std::string actor_name, CRichEditCtrl& edit, COLORREF color);

    // ��ӽ�ɫ
    bool addActor(std::string name, const char* role, const char* tag_color, std::string preffix = "", std::string icon = "");

    // ��ȡ�û�����
    std::string& getUserName();
    // ��ȡ��������
    std::string& getAssistantName();
	// ��ȡ�û�ͼ��
	std::string& getUserIcon();
	// ��ȡ����ͼ��
	std::string& getAssistantIcon();

    // �Ƴ����н�ɫ
    void removeAllActors();

    // Ϊ���������Զ���ȫ
    void cureCompletionForChat();

    // ��������ͣ�ô�
    void setupChatStopWords();

    // ��ȡ��Ϣ����
    int messagesCount();

    // �г���ǰ��ɫ
    void listCurrentActors();

    // �����ʾ
    std::string composePrompt();

    // ��ȡ��ʾ�� JSON �ĵ�
    yyjson_mut_doc* getPromptJSON();

    // ��ȡ OAIPrompt �� JSON �ĵ�
    yyjson_mut_doc* getOAIPrompt();

    // ��������ģʽ
    bool setChatMode(bool value);

    // �ж��Ƿ�Ϊ����ģʽ
    bool isChatMode();

    // ������������
    bool setChatGuards(bool value);

    // ��ȡ��ǰ��ʾ�� JSON �ĵ�
    yyjson_mut_doc* getCurrentPrompt();

private:
    messages_t messages;                // ��Ϣ�б�

    bool chat_guards = true;            // ������������
    bool chat_mode = true;              // ����ģʽ����
    bool using_system_prompt = false;   // �Ƿ�ʹ��ϵͳ��ʾ
    std::string user_name;              // �û�����
    std::string assistant_name;         // ��������
	std::string user_icon;              // �û�ͼ��
	std::string assistant_icon;         // ����ͼ��
    actors_t actors;                    // ��ɫӳ���

	COLORREF user_color;                // �û���ɫ
	COLORREF assistant_color;           // ������ɫ
};
#endif