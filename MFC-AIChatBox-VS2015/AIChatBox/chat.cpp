#include "pch.h"
#include "chat.h"

// �������¼���������Ϣ
void Chat::addNewMessage(std::string actor_name, std::string content) {
    // ���ݵ�ǰʱ�������Ա��Ϣ����Ϣ���ݴ����µ���Ϣ��Ŀ
    message_entry_t newEntry(getTimeStamp(), &actors[actor_name.data()], content.data());
    // ������Ϣ��Ŀ��ӵ���Ϣ�б���
    messages.push_back(newEntry);
}

// ɾ�����ָ����������Ϣ
bool Chat::removeLastMessage(int nmessages /*Ĭ��ֵΪ1*/) {
    size_t message_count = messages.size();

    // �����������ģʽ��ֻ��һ����Ϣ��ֱ��ɾ������Ϣ
    if (!chat_mode && message_count == 1) {
        messages.pop_back();
        return true;
    }

    // �����������Ϣ�������Ƿ�ʹ��ϵͳ��ʾ����ɾ����ʽ
    if (message_count == 2) {
        using_system_prompt ? messages.pop_back() : messages.clear();
        return true;
    }

    // �����Ϣ��������������ȷ��ɾ����������������Ϣ����Ȼ��ɾ����Ӧ��Ϣ
    if (message_count > 2) {
        nmessages = min(nmessages, static_cast<int>(message_count));
        messages.erase(messages.end() - nmessages, messages.end());
        return true;
    }
    return false;
}

// ����ָ�������Ϣ������
void Chat::updateMessageContent(int number, std::string newContent) {
    // ֱ�Ӹ�ָ����ŵ���Ϣ���ݸ�ֵ������
    messages[number].content.assign(newContent);
}

// ����������ʷ��¼
void Chat::resetChatHistory() {

    messages.clear();

    // ���δʹ��ϵͳ��ʾ�����������Ϣ
    //if (!using_system_prompt) {
    //    messages.clear();
    //}
    //else {
    //    // ���ʹ����ϵͳ��ʾ������ϵͳ��ʾ��ɾ��������Ϣ
    //    if (messages.size() > 1)
    //        messages.erase(messages.begin() + 1, messages.end());
    //}
}

// �����������
void Chat::draw() {
    // ����ն���Ļ
    //Terminal::clear();
    for (const message_entry_t& entry : messages) {
        // ��ӡ��Ա�����ǩ
        printActorChaTag(entry.actor_info->name);
        std::string actor_name = entry.actor_info->name;

        // �����ϵͳ��Ϣ��������ʾ����Ϊ300�ַ�
        if (actor_name == "System") {
            std::cout << (entry.content.length() > 300 ? entry.content.substr(0, 300) + "..." : entry.content);
            std::cout << "\n\n";
        }
        else {
            // ��ͨ��Ա��Ϣֱ�����
            std::cout << entry.content << std::endl;
        }
    }
}

void Chat::drawSystem(CRichEditCtrl& edit)
{
    // ��ձ༭��
	edit.SetWindowText(TEXT(""));
    for (const message_entry_t& entry : messages) {
        // ��ӡ��Ա�����ǩ
        //printActorChaTag(entry.actor_info->name, edit);
        std::string actor_name = entry.actor_info->name;

        // �����ϵͳ��Ϣ��������ʾ����Ϊ300�ַ�
        if (actor_name == "System") {
            std::string content = entry.content;
            content += "\r\n";

			// ��UTF-8�ַ���ת��ΪUTF-16�ַ���
            CStringW utf16String = ConvertUtf8ToUtf16(content);

			edit.ReplaceSel(utf16String);

        }
    }
}

void Chat::drawSystem(CEdit& edit)
{
	// ��ձ༭��
	edit.SetWindowText(TEXT(""));
    for (const message_entry_t& entry : messages) {
        // ��ӡ��Ա�����ǩ
        //printActorChaTag(entry.actor_info->name, edit);
        std::string actor_name = entry.actor_info->name;

        // �����ϵͳ��Ϣ��������ʾ����Ϊ300�ַ�
        if (actor_name == "System") {
            std::string content = entry.content;
            content += "\r\n";

			// ��UTF-8�ַ���ת��ΪUTF-16�ַ���
            CStringW utf16String = ConvertUtf8ToUtf16(content);

			edit.ReplaceSel(utf16String);
        }
    }
}

void Chat::draw(CEdit& edit)
{
	// ��ձ༭��
	edit.SetWindowText(TEXT(""));
	for (const message_entry_t& entry : messages) {
		// ��ӡ��Ա�����ǩ
		
		std::string actor_name = entry.actor_info->name;

		// �����ϵͳ��Ϣ��������ʾ����Ϊ300�ַ�
		if (actor_name != "System") {
            // ��ͨ��Ա��Ϣֱ�����
            printActorChaTag(entry.actor_info->name, edit);
            std::string content = entry.content;
            content += "\r\n";

            // ��UTF-8�ַ���ת��ΪUTF-16�ַ���
            CStringW utf16String = ConvertUtf8ToUtf16(content);

			edit.ReplaceSel(utf16String);
		}
		//else {
		//	// ��ͨ��Ա��Ϣֱ�����
		//	std::string content = entry.content;
		//	content += "\r\n";
		//	edit.ReplaceSel(CString(content.c_str()));
		//}
	}
}

void Chat::draw(CRichEditCtrl& edit)
{
    // ��ձ༭��
    edit.SetWindowText(TEXT(""));
    for (const message_entry_t& entry : messages) {
        // ��ӡ��Ա�����ǩ

        std::string actor_name = entry.actor_info->name;

        if (actor_name != "System") {
			if (entry.actor_info->role == "user") {
				printActorChaTag(entry.actor_info->name, edit, user_color);
			}
			else {
				printActorChaTag(entry.actor_info->name, edit, assistant_color);
			}
			//printActorChaTag(entry.actor_info->name, edit, user_color);
            std::string content = entry.content;
            content += "\r\n";

            // ��UTF-8�ַ���ת��ΪUTF-16�ַ���
            CStringW utf16String = ConvertUtf8ToUtf16(content);

            edit.ReplaceSel(utf16String);
        }
        //else {
        //	// ��ͨ��Ա��Ϣֱ�����
        //	std::string content = entry.content;
        //	content += "\r\n";
        //	edit.ReplaceSel(CString(content.c_str()));
        //}
    }
}

bool Chat::loadAllPrompts(std::vector<CString>& prompts)
{
    // ���Դ��û���ʾ�ļ�
    sjson prompt_file = sjson(USER_PROMPT_FILE);
    if (!prompt_file.is_opened())
        return false;

	// ��ȡ������ʾ����
	std::vector<std::string> prompt_names = prompt_file.get_top_level_keys();
	for (const std::string& prompt_name : prompt_names) {
		prompts.push_back(ConvertUtf8ToCString(prompt_name));
	}
	return true;
}

// �����û�ָ�����Ƶ���ʾ
bool Chat::loadUserPrompt(std::string prompt_name) {
    // ���Դ��û���ʾ�ļ�
    sjson prompt_file = sjson(USER_PROMPT_FILE);
    if (!prompt_file.is_opened())
        return false;

    const char* prompt_[] = { prompt_name.data(), "\0" };
    // ��ȡָ�����Ƶ���ʾ����
    yyjson_val* my_prompt = prompt_file.get_value(prompt_);
    if (my_prompt == NULL) {
        // ���δ�ҵ���ʾ����¼������־
        Logging::error("Prompt \"%s\" not found.", prompt_name.data());
        return false;
    }

    // �����Ա�б����Ϣ�б�
    actors.clear();
    messages.clear();

    // ��ȡϵͳ��ʾ����
    yyjson_val* system = yyjson_obj_get(my_prompt, "system");
    if (system != NULL) {
        // ����ϵͳ��ʾ
        setupSystemPrompt(yyjson_get_str(system));
        using_system_prompt = true;
    }
    else {
        using_system_prompt = false;
    }

    // ��ȡ��Ա�б�
    yyjson_val* actors = yyjson_obj_get(my_prompt, "actors");
    if (actors == NULL) {
        // ���û����Ա�б�����Ĭ����Ա
        setupDefaultActors();
        return true;
    }

    size_t actor_count = yyjson_arr_size(actors);
	for (size_t i = 0; i < actor_count; ++i) {  // ������Ա�б�
        yyjson_val* actor = yyjson_arr_get(actors, i);
        if (actor && yyjson_get_type(actor) == YYJSON_TYPE_OBJ) {
            yyjson_val* name = yyjson_obj_get(actor, "name");
            yyjson_val* role = yyjson_obj_get(actor, "role");
            yyjson_val* color = yyjson_obj_get(actor, "color");
            yyjson_val* msg_preffix = yyjson_obj_get(actor, "msg_preffix");
            yyjson_val* icon = yyjson_obj_get(actor, "icon");
            if (name && role) {
                const char* name_str = yyjson_get_str(name);
                const char* role_str = yyjson_get_str(role);
                const char* color_str = "";
                const char* msg_preffix_str = "";
                const char* icon_str = "";

                if (icon != NULL) {
                    icon_str = yyjson_get_str(icon);
                }

                if (msg_preffix != NULL) {
                    msg_preffix_str = yyjson_get_str(msg_preffix);
                }
                if (color != NULL) {
                    color_str = yyjson_get_str(color);
                }
                else {
                    // color_str = ANSIColors::getRandColor();
                }

                // �����Ա
                addActor(name_str, role_str, color_str, msg_preffix_str, icon_str);
				if (!strcmp(role_str, "user")) {    // ������û�
					user_name = name_str;   // �����û�����
					user_icon = icon_str;
                }
                else if (!strcmp(role_str, "assistant")) {
                    assistant_name = name_str;
					assistant_icon = icon_str;
                }
            }
        }
    }

    return true;
}

std::string Chat::getSystemPrompt()
{
	return messages[0].content;
}

// ����ϵͳ��ʾ
void Chat::setupSystemPrompt(std::string prompt) {
    // ���ϵͳ��ɫ
    addActor("System", "system", "green_ul");
    // ���ϵͳ��ʾ��Ϣ
    addNewMessage("System", prompt);
    using_system_prompt = true;
}

// ����ϵͳ��ʾ����
void Chat::updateSystemPrompt(std::string new_system_prompt) {
    // ���µ�һ����Ϣ��ϵͳ��ʾ��������
    updateMessageContent(0, new_system_prompt);
}

// ����Ĭ����Ա
void Chat::setupDefaultActors() {
    // ����û���ɫ
    addActor("User", "user", "blue");
    // ������ֽ�ɫ
    addActor("Assistant", "assistant", "pink");
    user_name = "User";
    assistant_name = "Assistant";
}

void Chat::setUserAndAssistantName(std::string userName, std::string assistantName)
{
	user_name = userName;
	assistant_name = assistantName;
}

void Chat::setUserAndAssistantColor(COLORREF userColor, COLORREF assistantColor)
{
	user_color = userColor;
	assistant_color = assistantColor;
}

// ����ָ��·���ı���Ի�
bool Chat::loadSavedConversation(std::string file_path) {
    // �򿪱���Ի����ļ�
    sjson saved_file = sjson(file_path.c_str());
    if (!saved_file.is_opened())
        return false;

    // �����Ա�б����Ϣ�б�
    actors.clear();
    messages.clear();

	// ��ȡ��Ա�б�
	yyjson_val* actors = yyjson_obj_get(saved_file.get_current_root(), "actors");
	if (actors == NULL) {
		// ���û����Ա�б�����Ĭ����Ա
		setupDefaultActors();
		return true;
	}

	size_t actor_count = yyjson_arr_size(actors);
	for (size_t i = 0; i < actor_count; ++i) {  // ������Ա�б�
		yyjson_val* actor = yyjson_arr_get(actors, i);
		if (actor && yyjson_get_type(actor) == YYJSON_TYPE_OBJ) {
			yyjson_val* name = yyjson_obj_get(actor, "name");
			yyjson_val* role = yyjson_obj_get(actor, "role");
			yyjson_val* color = yyjson_obj_get(actor, "color");
			yyjson_val* msg_preffix = yyjson_obj_get(actor, "msg_preffix");
			yyjson_val* icon = yyjson_obj_get(actor, "icon");
			if (name && role) {
				const char* name_str = yyjson_get_str(name);
				const char* role_str = yyjson_get_str(role);
				const char* color_str = "";
				const char* msg_preffix_str = "";
				const char* icon_str = "";

				if (icon != NULL) {
					icon_str = yyjson_get_str(icon);
				}

				if (msg_preffix != NULL) {
					msg_preffix_str = yyjson_get_str(msg_preffix);
				}
				if (color != NULL) {
					color_str = yyjson_get_str(color);
				}
				else {
					// color_str = ANSIColors::getRandColor();
				}

				// �����Ա
				addActor(name_str, role_str, color_str, msg_preffix_str, icon_str);
				if (!strcmp(role_str, "user")) {    // ������û�
					user_name = name_str;   // �����û�����
					user_icon = icon_str;
				}
				else if (!strcmp(role_str, "assistant")) {
					assistant_name = name_str;
					assistant_icon = icon_str;
				}
			}
		}
	}

    yyjson_val* hits = yyjson_obj_get(saved_file.get_current_root(), "history");
    size_t idx, max;
    yyjson_val* hit;
    // ������ʷ��Ϣ�������ӵ������¼��
    yyjson_arr_foreach(hits, idx, max, hit) {
        int64_t    id = yyjson_get_int(yyjson_obj_get(hit, "id"));
        const char* name_str = yyjson_get_str(yyjson_obj_get(hit, "name"));
        const char* role_str = yyjson_get_str(yyjson_obj_get(hit, "role"));
        const char* content = yyjson_get_str(yyjson_obj_get(hit, "content"));

        //addActor(name_str, role_str, "");
        addNewMessage(name_str, content);
        if (!strcmp(role_str, "user")) {
            user_name = name_str;
        }
        else if (!strcmp(role_str, "assistant")) {
            assistant_name = name_str;
        }
    }

    return true;
}

// ���浱ǰ�Ի�
/*bool Chat::saveConversation(std::string filename) {
    // �����µĿɱ�JSON�ĵ�
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    // ������ʷ��Ϣ����
    yyjson_mut_val* historyArray = yyjson_mut_arr(doc);

    for (const message_entry_t& entry : messages) {
        yyjson_mut_val* historyEntry = yyjson_mut_obj(doc);
        // �����ϢID
        yyjson_mut_obj_add_int(doc, historyEntry, "id", entry.id);
        // �����Ա����
        yyjson_mut_obj_add_str(doc, historyEntry, "name", entry.actor_info->name.c_str());
        // �����Ա��ɫ
        yyjson_mut_obj_add_str(doc, historyEntry, "role", entry.actor_info->role.c_str());
        // �����Ϣ����
        yyjson_mut_obj_add_str(doc, historyEntry, "content", entry.content.c_str());
        // ����Ϣ��Ŀ��ӵ���ʷ��Ϣ����
        yyjson_mut_arr_add_val(historyArray, historyEntry);
    }
    // ����ʷ��Ϣ������ӵ����ڵ�
    yyjson_mut_obj_add_val(doc, root, "history", historyArray);

    // ����actors����
	yyjson_mut_val* actorsArray = yyjson_mut_arr(doc);
	//for (const auto& [key, value] : actors) {
	//for (const auto& kvPair : myMap) {
		// ����kvPair�еļ�ֵ��
		// kvPair.firstΪ����kvPair.secondΪֵ
	//}
	//std::for_each(myMap.begin(), myMap.end(), [](const std::pair<const Key, Value>& kvPair) {
		// ����kvPair�еļ�ֵ��
		// kvPair.firstΪ����kvPair.secondΪֵ
	//});
	for (const std::map<std::string, actor_t>& kvPair : actorsArray) {
		yyjson_mut_val* actorEntry = yyjson_mut_obj(doc);
		// �����Ա����
		yyjson_mut_obj_add_str(doc, actorEntry, "name", kvPair.first.name.c_str());
		// �����Ա��ɫ
		yyjson_mut_obj_add_str(doc, actorEntry, "role", kvPair.second.role.c_str());
		// �����Ա��ǩ��ɫ
		yyjson_mut_obj_add_str(doc, actorEntry, "color", kvPair.second.tag_color.c_str());
		// �����Ա��Ϣǰ׺
		yyjson_mut_obj_add_str(doc, actorEntry, "msg_preffix", kvPair.second.msg_preffix.c_str());
		// �����Աͼ��
		yyjson_mut_obj_add_str(doc, actorEntry, "icon", kvPair.second.icon.c_str());
		// ����Ա��Ŀ��ӵ���Ա����
		yyjson_mut_arr_add_val(actorsArray, actorEntry);
	}
	// ����Ա������ӵ����ڵ�
	yyjson_mut_obj_add_val(doc, root, "actors", actorsArray);

    yyjson_write_flag flg = YYJSON_WRITE_PRETTY;  // | YYJSON_WRITE_ESCAPE_UNICODE;
    yyjson_write_err err;
    // ��JSON�ĵ�д���ļ�
    yyjson_mut_write_file(filename.c_str(), doc, flg, NULL, &err);
    yyjson_mut_doc_free(doc);
    if (err.code) {
        // ���д�������¼������־
        Logging::error("Error writing the file \"%s\": %s", filename.c_str(), err.msg);
        return false;
    }
    return true;
}*/
// ���浱ǰ�Ի�
bool Chat::saveConversation(std::string filename) {
	// �����µĿɱ�JSON�ĵ�
	yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
	yyjson_mut_val* root = yyjson_mut_obj(doc);
	yyjson_mut_doc_set_root(doc, root);

	// ������ʷ��Ϣ����
	yyjson_mut_val* historyArray = yyjson_mut_arr(doc);

	for (const auto& entry : messages) {
		yyjson_mut_val* historyEntry = yyjson_mut_obj(doc);
		// �����ϢID
		yyjson_mut_obj_add_int(doc, historyEntry, "id", entry.id);
		// �����Ա����
		yyjson_mut_obj_add_str(doc, historyEntry, "name", entry.actor_info->name.c_str());
		// �����Ա��ɫ
		yyjson_mut_obj_add_str(doc, historyEntry, "role", entry.actor_info->role.c_str());
		// �����Ϣ����
		yyjson_mut_obj_add_str(doc, historyEntry, "content", entry.content.c_str());
		// ����Ϣ��Ŀ��ӵ���ʷ��Ϣ����
		yyjson_mut_arr_add_val(historyArray, historyEntry);
	}
	// ����ʷ��Ϣ������ӵ����ڵ�
	yyjson_mut_obj_add_val(doc, root, "history", historyArray);

	// ����actors����
	yyjson_mut_val* actorsArray = yyjson_mut_arr(doc);
	for (const auto& pair : actors) {  // �޸ĵ㣺ʹ��pair����ṹ����
		const auto& key = pair.first;
		const auto& value = pair.second;
		yyjson_mut_val* actorEntry = yyjson_mut_obj(doc);
		// �����Ա����
		yyjson_mut_obj_add_str(doc, actorEntry, "name", value.name.c_str());
		// �����Ա��ɫ
		yyjson_mut_obj_add_str(doc, actorEntry, "role", value.role.c_str());
		// �����Ա��ǩ��ɫ
		yyjson_mut_obj_add_str(doc, actorEntry, "color", value.tag_color.c_str());
		// �����Ա��Ϣǰ׺
		yyjson_mut_obj_add_str(doc, actorEntry, "msg_preffix", value.msg_preffix.c_str());
		// �����Աͼ��
		yyjson_mut_obj_add_str(doc, actorEntry, "icon", value.icon.c_str());
		// ����Ա��Ŀ��ӵ���Ա����
		yyjson_mut_arr_add_val(actorsArray, actorEntry);
	}
	// ����Ա������ӵ����ڵ�
	yyjson_mut_obj_add_val(doc, root, "actors", actorsArray);

	yyjson_write_flag flg = YYJSON_WRITE_PRETTY;
	yyjson_write_err err;
	// ��JSON�ĵ�д���ļ�
	yyjson_mut_write_file(filename.c_str(), doc, flg, nullptr, &err);
	yyjson_mut_doc_free(doc);
	if (err.code) {
		// ���д�������¼������־
		Logging::error("Error writing the file \"%s\": %s", filename.c_str(), err.msg);
		return false;
	}
	return true;
}

// ��ӡ��Ա�����ǩ
void Chat::printActorChaTag(std::string actor_name) {
    actor_t actor = actors[actor_name.data()];
    //std::cout << actor.icon << " " << ANSIColors::getColorCode(actor.tag_color) << actor.name
    //    << ANSI_COLOR_RESET << ":";
    std::cout << actor.icon << " " << actor.name << ":";
}

void Chat::printActorChaTag(std::string actor_name, CEdit& edit)
{
	actor_t actor = actors[actor_name.data()];
	//std::string tag = actor.icon + " " + actor.name + ":";
    std::string tag = actor.name + ":";
	// ��tag��ӵ��༭��

    // ��UTF-8�ַ���ת��ΪUTF-16�ַ���
    CStringW utf16String = ConvertUtf8ToUtf16(tag);

    edit.ReplaceSel(utf16String);
}

void Chat::printActorChaTag(std::string actor_name, CRichEditCtrl& edit, COLORREF color)
{
    actor_t actor = actors[actor_name.data()];
    std::string tag = actor.icon + " " + actor.name + ":";
    CStringW utf16String = ConvertUtf8ToUtf16(tag);

    // ����ԭʼ��ʽ
    CHARFORMAT2 oldFormat;
    oldFormat.cbSize = sizeof(CHARFORMAT2);
    edit.GetSelectionCharFormat(oldFormat);

    // ���ú�ɫ�ı���ʽ
    CHARFORMAT2 newFormat;
    ZeroMemory(&newFormat, sizeof(newFormat));
    newFormat.cbSize = sizeof(newFormat);
    newFormat.dwMask = CFM_COLOR;
    newFormat.crTextColor = color;
    edit.SetSelectionCharFormat(newFormat);

    // �����ı�
    edit.ReplaceSel(utf16String);

    // �ָ�ԭʼ��ʽ
    edit.SetSelectionCharFormat(oldFormat);
}

// �����Ա
bool Chat::addActor(
    std::string name,
    const char* role,
    const char* tag_color,
	std::string preffix,    // Ĭ��ֵΪ���ַ���
    std::string icon
) {
    // �����Ա�����Ƿ��Ѵ��ڣ��������򷵻�false
    if (actors.find(name.data()) != actors.end())
        return false;
    actor_t new_actor = { name, role, tag_color, preffix, icon };
    // ������Ա��ӵ���Ա�б�
    actors[new_actor.name] = new_actor;
    return true;
}

// ��ȡ�û���
std::string& Chat::getUserName() {
    return user_name;
}

// ��ȡ������
std::string& Chat::getAssistantName() {
    return assistant_name;
}

std::string& Chat::getUserIcon()
{
    // TODO: �ڴ˴����� return ���
	return user_icon;
}

std::string& Chat::getAssistantIcon()
{
    // TODO: �ڴ˴����� return ���
	return assistant_icon;
}

// �Ƴ�������Ա
void Chat::removeAllActors() {
    actors.clear();
}

// �������������صĻ�����
void Chat::cureCompletionForChat() {
    if (chat_guards) {
        // ȥ����������ͷ�Ŀո�
        completionBus.buffer.erase(0, completionBus.buffer.find_first_not_of(" "));
        while (!completionBus.buffer.empty() && completionBus.buffer.back() == '\n') {
            // ȥ��������ĩβ�Ļ��з�
            completionBus.buffer.erase(completionBus.buffer.size() - 1);
        }
    }
}

// ����������ص�ֹͣ��
void Chat::setupChatStopWords() {
    if (chat_guards) {
        chat_template_t& templates = getChatTemplates();
        auto addStop = [this](std::string token) {
            if (token == "") {}
            else {
                if (token != "\n") addStopWord(normalizeText(token));
            }
            };

        addStop(templates.begin_user);
        addStop(templates.end_user);
        addStop(templates.begin_system);
        addStop(templates.end_system);
        addStop(templates.eos);
    }
}

// ��ȡ��Ϣ����
int Chat::messagesCount() {
    return messages.size();
}

// �г���ǰ������Ա
/*void Chat::listCurrentActors() {
    for (const auto& [key, value] : actors) {
        std::cout << "\t*" << key << "\n";
    }
}*/
void Chat::listCurrentActors() {
	for (const auto& pair : actors) {
		std::cout << "\t*" << pair.first << "\n";
	}
}

// ��������ģ������µ���ʾ
std::string Chat::composePrompt() {
    std::string newPrompt;
    chat_template_t& chat_template = getChatTemplates();
    newPrompt += chat_template.bos;
    for (size_t i = 0; i < messages.size(); ++i) {
        const message_entry_t& entry = messages[i];
        bool is_last = i == messages.size() - 1;
        std::string tag = entry.actor_info->name + ":";
        if (entry.actor_info->role == "user") {
            newPrompt += chat_template.begin_user;
            if (chat_mode) newPrompt += tag;
            newPrompt += entry.actor_info->msg_preffix;
            newPrompt += entry.content;
            if (!is_last) newPrompt += chat_template.end_user;
        }
        else if (entry.actor_info->role == "system") {
            newPrompt += chat_template.begin_system;
            newPrompt += entry.actor_info->msg_preffix;
            newPrompt += entry.content;
            newPrompt += chat_template.end_system;
        }
        else {
            newPrompt += chat_template.begin_assistant;
            if (chat_mode) newPrompt += tag;
            newPrompt += entry.actor_info->msg_preffix;
            newPrompt += entry.content;
            if (!is_last) newPrompt += chat_template.end_assistant;
            if (!is_last) newPrompt += chat_template.eos;
        }
    };

    return newPrompt;
}

// ��ȡ�����������ʾJSON
yyjson_mut_doc* Chat::getPromptJSON() {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);
    std::string prompt = composePrompt();
    // ����Ϻõ���ʾ��ӵ�JSON������
    yyjson_mut_obj_add_strcpy(doc, root, "prompt", prompt.c_str());
    return doc;
}

// ��ȡ����OAI�������ʾJSON
yyjson_mut_doc* Chat::getOAIPrompt() {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_val* messagesArray = yyjson_mut_arr(doc);
    for (const auto& entry : messages) {
        yyjson_mut_val* messageObj = yyjson_mut_obj(doc);
        // �����Ϣ����
        yyjson_mut_obj_add_str(doc, messageObj, "content", entry.content.c_str());
        // �����Ϣ��ɫ
        yyjson_mut_obj_add_str(doc, messageObj, "role", entry.actor_info->role.c_str());
        // �����ϢID
        yyjson_mut_obj_add_int(doc, messageObj, "id", entry.id);
        // ����Ϣ������ӵ���Ϣ����
        yyjson_mut_arr_add_val(messagesArray, messageObj);
    }

    // ����Ϣ������ӵ����ڵ�
    yyjson_mut_obj_add_val(doc, root, "messages", messagesArray);

    return doc;
}

// ��������ģʽ
bool Chat::setChatMode(bool value) {
    chat_mode = value;
    return chat_mode;
}

// �ж��Ƿ�������ģʽ
bool Chat::isChatMode() {
    return chat_mode;
}

// �����������
bool Chat::setChatGuards(bool value) {
    chat_guards = value;
    return chat_guards;
}

// ��ȡ��ǰ�����������ʾJSON
yyjson_mut_doc* Chat::getCurrentPrompt() {
    if (using_oai_completion) {
        return getOAIPrompt();
    }
    return getPromptJSON();
}