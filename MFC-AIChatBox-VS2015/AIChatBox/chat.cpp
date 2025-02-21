#include "pch.h"
#include "chat.h"

// 向聊天记录中添加新消息
void Chat::addNewMessage(std::string actor_name, std::string content) {
    // 根据当前时间戳、演员信息和消息内容创建新的消息条目
    message_entry_t newEntry(getTimeStamp(), &actors[actor_name.data()], content.data());
    // 将新消息条目添加到消息列表中
    messages.push_back(newEntry);
}

// 删除最后指定数量的消息
bool Chat::removeLastMessage(int nmessages /*默认值为1*/) {
    size_t message_count = messages.size();

    // 如果不是聊天模式且只有一条消息，直接删除该消息
    if (!chat_mode && message_count == 1) {
        messages.pop_back();
        return true;
    }

    // 如果有两条消息，根据是否使用系统提示决定删除方式
    if (message_count == 2) {
        using_system_prompt ? messages.pop_back() : messages.clear();
        return true;
    }

    // 如果消息数量大于两条，确保删除数量不超过总消息数，然后删除相应消息
    if (message_count > 2) {
        nmessages = min(nmessages, static_cast<int>(message_count));
        messages.erase(messages.end() - nmessages, messages.end());
        return true;
    }
    return false;
}

// 更新指定编号消息的内容
void Chat::updateMessageContent(int number, std::string newContent) {
    // 直接给指定编号的消息内容赋值新内容
    messages[number].content.assign(newContent);
}

// 重置聊天历史记录
void Chat::resetChatHistory() {

    messages.clear();

    // 如果未使用系统提示，清空所有消息
    //if (!using_system_prompt) {
    //    messages.clear();
    //}
    //else {
    //    // 如果使用了系统提示，保留系统提示，删除其他消息
    //    if (messages.size() > 1)
    //        messages.erase(messages.begin() + 1, messages.end());
    //}
}

// 绘制聊天界面
void Chat::draw() {
    // 清空终端屏幕
    //Terminal::clear();
    for (const message_entry_t& entry : messages) {
        // 打印演员聊天标签
        printActorChaTag(entry.actor_info->name);
        std::string actor_name = entry.actor_info->name;

        // 如果是系统消息，限制显示长度为300字符
        if (actor_name == "System") {
            std::cout << (entry.content.length() > 300 ? entry.content.substr(0, 300) + "..." : entry.content);
            std::cout << "\n\n";
        }
        else {
            // 普通演员消息直接输出
            std::cout << entry.content << std::endl;
        }
    }
}

void Chat::drawSystem(CRichEditCtrl& edit)
{
    // 清空编辑框
	edit.SetWindowText(TEXT(""));
    for (const message_entry_t& entry : messages) {
        // 打印演员聊天标签
        //printActorChaTag(entry.actor_info->name, edit);
        std::string actor_name = entry.actor_info->name;

        // 如果是系统消息，限制显示长度为300字符
        if (actor_name == "System") {
            std::string content = entry.content;
            content += "\r\n";

			// 将UTF-8字符串转换为UTF-16字符串
            CStringW utf16String = ConvertUtf8ToUtf16(content);

			edit.ReplaceSel(utf16String);

        }
    }
}

void Chat::drawSystem(CEdit& edit)
{
	// 清空编辑框
	edit.SetWindowText(TEXT(""));
    for (const message_entry_t& entry : messages) {
        // 打印演员聊天标签
        //printActorChaTag(entry.actor_info->name, edit);
        std::string actor_name = entry.actor_info->name;

        // 如果是系统消息，限制显示长度为300字符
        if (actor_name == "System") {
            std::string content = entry.content;
            content += "\r\n";

			// 将UTF-8字符串转换为UTF-16字符串
            CStringW utf16String = ConvertUtf8ToUtf16(content);

			edit.ReplaceSel(utf16String);
        }
    }
}

void Chat::draw(CEdit& edit)
{
	// 清空编辑框
	edit.SetWindowText(TEXT(""));
	for (const message_entry_t& entry : messages) {
		// 打印演员聊天标签
		
		std::string actor_name = entry.actor_info->name;

		// 如果是系统消息，限制显示长度为300字符
		if (actor_name != "System") {
            // 普通演员消息直接输出
            printActorChaTag(entry.actor_info->name, edit);
            std::string content = entry.content;
            content += "\r\n";

            // 将UTF-8字符串转换为UTF-16字符串
            CStringW utf16String = ConvertUtf8ToUtf16(content);

			edit.ReplaceSel(utf16String);
		}
		//else {
		//	// 普通演员消息直接输出
		//	std::string content = entry.content;
		//	content += "\r\n";
		//	edit.ReplaceSel(CString(content.c_str()));
		//}
	}
}

void Chat::draw(CRichEditCtrl& edit)
{
    // 清空编辑框
    edit.SetWindowText(TEXT(""));
    for (const message_entry_t& entry : messages) {
        // 打印演员聊天标签

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

            // 将UTF-8字符串转换为UTF-16字符串
            CStringW utf16String = ConvertUtf8ToUtf16(content);

            edit.ReplaceSel(utf16String);
        }
        //else {
        //	// 普通演员消息直接输出
        //	std::string content = entry.content;
        //	content += "\r\n";
        //	edit.ReplaceSel(CString(content.c_str()));
        //}
    }
}

bool Chat::loadAllPrompts(std::vector<CString>& prompts)
{
    // 尝试打开用户提示文件
    sjson prompt_file = sjson(USER_PROMPT_FILE);
    if (!prompt_file.is_opened())
        return false;

	// 获取所有提示名称
	std::vector<std::string> prompt_names = prompt_file.get_top_level_keys();
	for (const std::string& prompt_name : prompt_names) {
		prompts.push_back(ConvertUtf8ToCString(prompt_name));
	}
	return true;
}

// 加载用户指定名称的提示
bool Chat::loadUserPrompt(std::string prompt_name) {
    // 尝试打开用户提示文件
    sjson prompt_file = sjson(USER_PROMPT_FILE);
    if (!prompt_file.is_opened())
        return false;

    const char* prompt_[] = { prompt_name.data(), "\0" };
    // 获取指定名称的提示内容
    yyjson_val* my_prompt = prompt_file.get_value(prompt_);
    if (my_prompt == NULL) {
        // 如果未找到提示，记录错误日志
        Logging::error("Prompt \"%s\" not found.", prompt_name.data());
        return false;
    }

    // 清空演员列表和消息列表
    actors.clear();
    messages.clear();

    // 获取系统提示内容
    yyjson_val* system = yyjson_obj_get(my_prompt, "system");
    if (system != NULL) {
        // 设置系统提示
        setupSystemPrompt(yyjson_get_str(system));
        using_system_prompt = true;
    }
    else {
        using_system_prompt = false;
    }

    // 获取演员列表
    yyjson_val* actors = yyjson_obj_get(my_prompt, "actors");
    if (actors == NULL) {
        // 如果没有演员列表，设置默认演员
        setupDefaultActors();
        return true;
    }

    size_t actor_count = yyjson_arr_size(actors);
	for (size_t i = 0; i < actor_count; ++i) {  // 遍历演员列表
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

                // 添加演员
                addActor(name_str, role_str, color_str, msg_preffix_str, icon_str);
				if (!strcmp(role_str, "user")) {    // 如果是用户
					user_name = name_str;   // 设置用户名称
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

// 设置系统提示
void Chat::setupSystemPrompt(std::string prompt) {
    // 添加系统角色
    addActor("System", "system", "green_ul");
    // 添加系统提示消息
    addNewMessage("System", prompt);
    using_system_prompt = true;
}

// 更新系统提示内容
void Chat::updateSystemPrompt(std::string new_system_prompt) {
    // 更新第一条消息（系统提示）的内容
    updateMessageContent(0, new_system_prompt);
}

// 设置默认演员
void Chat::setupDefaultActors() {
    // 添加用户角色
    addActor("User", "user", "blue");
    // 添加助手角色
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

// 加载指定路径的保存对话
bool Chat::loadSavedConversation(std::string file_path) {
    // 打开保存对话的文件
    sjson saved_file = sjson(file_path.c_str());
    if (!saved_file.is_opened())
        return false;

    // 清空演员列表和消息列表
    actors.clear();
    messages.clear();

	// 获取演员列表
	yyjson_val* actors = yyjson_obj_get(saved_file.get_current_root(), "actors");
	if (actors == NULL) {
		// 如果没有演员列表，设置默认演员
		setupDefaultActors();
		return true;
	}

	size_t actor_count = yyjson_arr_size(actors);
	for (size_t i = 0; i < actor_count; ++i) {  // 遍历演员列表
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

				// 添加演员
				addActor(name_str, role_str, color_str, msg_preffix_str, icon_str);
				if (!strcmp(role_str, "user")) {    // 如果是用户
					user_name = name_str;   // 设置用户名称
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
    // 遍历历史消息，逐个添加到聊天记录中
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

// 保存当前对话
/*bool Chat::saveConversation(std::string filename) {
    // 创建新的可变JSON文档
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    // 创建历史消息数组
    yyjson_mut_val* historyArray = yyjson_mut_arr(doc);

    for (const message_entry_t& entry : messages) {
        yyjson_mut_val* historyEntry = yyjson_mut_obj(doc);
        // 添加消息ID
        yyjson_mut_obj_add_int(doc, historyEntry, "id", entry.id);
        // 添加演员名字
        yyjson_mut_obj_add_str(doc, historyEntry, "name", entry.actor_info->name.c_str());
        // 添加演员角色
        yyjson_mut_obj_add_str(doc, historyEntry, "role", entry.actor_info->role.c_str());
        // 添加消息内容
        yyjson_mut_obj_add_str(doc, historyEntry, "content", entry.content.c_str());
        // 将消息条目添加到历史消息数组
        yyjson_mut_arr_add_val(historyArray, historyEntry);
    }
    // 将历史消息数组添加到根节点
    yyjson_mut_obj_add_val(doc, root, "history", historyArray);

    // 创建actors数组
	yyjson_mut_val* actorsArray = yyjson_mut_arr(doc);
	//for (const auto& [key, value] : actors) {
	//for (const auto& kvPair : myMap) {
		// 访问kvPair中的键值对
		// kvPair.first为键，kvPair.second为值
	//}
	//std::for_each(myMap.begin(), myMap.end(), [](const std::pair<const Key, Value>& kvPair) {
		// 访问kvPair中的键值对
		// kvPair.first为键，kvPair.second为值
	//});
	for (const std::map<std::string, actor_t>& kvPair : actorsArray) {
		yyjson_mut_val* actorEntry = yyjson_mut_obj(doc);
		// 添加演员名字
		yyjson_mut_obj_add_str(doc, actorEntry, "name", kvPair.first.name.c_str());
		// 添加演员角色
		yyjson_mut_obj_add_str(doc, actorEntry, "role", kvPair.second.role.c_str());
		// 添加演员标签颜色
		yyjson_mut_obj_add_str(doc, actorEntry, "color", kvPair.second.tag_color.c_str());
		// 添加演员消息前缀
		yyjson_mut_obj_add_str(doc, actorEntry, "msg_preffix", kvPair.second.msg_preffix.c_str());
		// 添加演员图标
		yyjson_mut_obj_add_str(doc, actorEntry, "icon", kvPair.second.icon.c_str());
		// 将演员条目添加到演员数组
		yyjson_mut_arr_add_val(actorsArray, actorEntry);
	}
	// 将演员数组添加到根节点
	yyjson_mut_obj_add_val(doc, root, "actors", actorsArray);

    yyjson_write_flag flg = YYJSON_WRITE_PRETTY;  // | YYJSON_WRITE_ESCAPE_UNICODE;
    yyjson_write_err err;
    // 将JSON文档写入文件
    yyjson_mut_write_file(filename.c_str(), doc, flg, NULL, &err);
    yyjson_mut_doc_free(doc);
    if (err.code) {
        // 如果写入出错，记录错误日志
        Logging::error("Error writing the file \"%s\": %s", filename.c_str(), err.msg);
        return false;
    }
    return true;
}*/
// 保存当前对话
bool Chat::saveConversation(std::string filename) {
	// 创建新的可变JSON文档
	yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
	yyjson_mut_val* root = yyjson_mut_obj(doc);
	yyjson_mut_doc_set_root(doc, root);

	// 创建历史消息数组
	yyjson_mut_val* historyArray = yyjson_mut_arr(doc);

	for (const auto& entry : messages) {
		yyjson_mut_val* historyEntry = yyjson_mut_obj(doc);
		// 添加消息ID
		yyjson_mut_obj_add_int(doc, historyEntry, "id", entry.id);
		// 添加演员名字
		yyjson_mut_obj_add_str(doc, historyEntry, "name", entry.actor_info->name.c_str());
		// 添加演员角色
		yyjson_mut_obj_add_str(doc, historyEntry, "role", entry.actor_info->role.c_str());
		// 添加消息内容
		yyjson_mut_obj_add_str(doc, historyEntry, "content", entry.content.c_str());
		// 将消息条目添加到历史消息数组
		yyjson_mut_arr_add_val(historyArray, historyEntry);
	}
	// 将历史消息数组添加到根节点
	yyjson_mut_obj_add_val(doc, root, "history", historyArray);

	// 创建actors数组
	yyjson_mut_val* actorsArray = yyjson_mut_arr(doc);
	for (const auto& pair : actors) {  // 修改点：使用pair代替结构化绑定
		const auto& key = pair.first;
		const auto& value = pair.second;
		yyjson_mut_val* actorEntry = yyjson_mut_obj(doc);
		// 添加演员名字
		yyjson_mut_obj_add_str(doc, actorEntry, "name", value.name.c_str());
		// 添加演员角色
		yyjson_mut_obj_add_str(doc, actorEntry, "role", value.role.c_str());
		// 添加演员标签颜色
		yyjson_mut_obj_add_str(doc, actorEntry, "color", value.tag_color.c_str());
		// 添加演员消息前缀
		yyjson_mut_obj_add_str(doc, actorEntry, "msg_preffix", value.msg_preffix.c_str());
		// 添加演员图标
		yyjson_mut_obj_add_str(doc, actorEntry, "icon", value.icon.c_str());
		// 将演员条目添加到演员数组
		yyjson_mut_arr_add_val(actorsArray, actorEntry);
	}
	// 将演员数组添加到根节点
	yyjson_mut_obj_add_val(doc, root, "actors", actorsArray);

	yyjson_write_flag flg = YYJSON_WRITE_PRETTY;
	yyjson_write_err err;
	// 将JSON文档写入文件
	yyjson_mut_write_file(filename.c_str(), doc, flg, nullptr, &err);
	yyjson_mut_doc_free(doc);
	if (err.code) {
		// 如果写入出错，记录错误日志
		Logging::error("Error writing the file \"%s\": %s", filename.c_str(), err.msg);
		return false;
	}
	return true;
}

// 打印演员聊天标签
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
	// 将tag添加到编辑框

    // 将UTF-8字符串转换为UTF-16字符串
    CStringW utf16String = ConvertUtf8ToUtf16(tag);

    edit.ReplaceSel(utf16String);
}

void Chat::printActorChaTag(std::string actor_name, CRichEditCtrl& edit, COLORREF color)
{
    actor_t actor = actors[actor_name.data()];
    std::string tag = actor.icon + " " + actor.name + ":";
    CStringW utf16String = ConvertUtf8ToUtf16(tag);

    // 保存原始格式
    CHARFORMAT2 oldFormat;
    oldFormat.cbSize = sizeof(CHARFORMAT2);
    edit.GetSelectionCharFormat(oldFormat);

    // 设置红色文本格式
    CHARFORMAT2 newFormat;
    ZeroMemory(&newFormat, sizeof(newFormat));
    newFormat.cbSize = sizeof(newFormat);
    newFormat.dwMask = CFM_COLOR;
    newFormat.crTextColor = color;
    edit.SetSelectionCharFormat(newFormat);

    // 插入文本
    edit.ReplaceSel(utf16String);

    // 恢复原始格式
    edit.SetSelectionCharFormat(oldFormat);
}

// 添加演员
bool Chat::addActor(
    std::string name,
    const char* role,
    const char* tag_color,
	std::string preffix,    // 默认值为空字符串
    std::string icon
) {
    // 检查演员名字是否已存在，若存在则返回false
    if (actors.find(name.data()) != actors.end())
        return false;
    actor_t new_actor = { name, role, tag_color, preffix, icon };
    // 将新演员添加到演员列表
    actors[new_actor.name] = new_actor;
    return true;
}

// 获取用户名
std::string& Chat::getUserName() {
    return user_name;
}

// 获取助手名
std::string& Chat::getAssistantName() {
    return assistant_name;
}

std::string& Chat::getUserIcon()
{
    // TODO: 在此处插入 return 语句
	return user_icon;
}

std::string& Chat::getAssistantIcon()
{
    // TODO: 在此处插入 return 语句
	return assistant_icon;
}

// 移除所有演员
void Chat::removeAllActors() {
    actors.clear();
}

// 清理聊天完成相关的缓冲区
void Chat::cureCompletionForChat() {
    if (chat_guards) {
        // 去除缓冲区开头的空格
        completionBus.buffer.erase(0, completionBus.buffer.find_first_not_of(" "));
        while (!completionBus.buffer.empty() && completionBus.buffer.back() == '\n') {
            // 去除缓冲区末尾的换行符
            completionBus.buffer.erase(completionBus.buffer.size() - 1);
        }
    }
}

// 设置聊天相关的停止词
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

// 获取消息数量
int Chat::messagesCount() {
    return messages.size();
}

// 列出当前所有演员
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

// 根据聊天模板组合新的提示
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

// 获取用于请求的提示JSON
yyjson_mut_doc* Chat::getPromptJSON() {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);
    std::string prompt = composePrompt();
    // 将组合好的提示添加到JSON对象中
    yyjson_mut_obj_add_strcpy(doc, root, "prompt", prompt.c_str());
    return doc;
}

// 获取用于OAI请求的提示JSON
yyjson_mut_doc* Chat::getOAIPrompt() {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(NULL);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_val* messagesArray = yyjson_mut_arr(doc);
    for (const auto& entry : messages) {
        yyjson_mut_val* messageObj = yyjson_mut_obj(doc);
        // 添加消息内容
        yyjson_mut_obj_add_str(doc, messageObj, "content", entry.content.c_str());
        // 添加消息角色
        yyjson_mut_obj_add_str(doc, messageObj, "role", entry.actor_info->role.c_str());
        // 添加消息ID
        yyjson_mut_obj_add_int(doc, messageObj, "id", entry.id);
        // 将消息对象添加到消息数组
        yyjson_mut_arr_add_val(messagesArray, messageObj);
    }

    // 将消息数组添加到根节点
    yyjson_mut_obj_add_val(doc, root, "messages", messagesArray);

    return doc;
}

// 设置聊天模式
bool Chat::setChatMode(bool value) {
    chat_mode = value;
    return chat_mode;
}

// 判断是否处于聊天模式
bool Chat::isChatMode() {
    return chat_mode;
}

// 设置聊天防护
bool Chat::setChatGuards(bool value) {
    chat_guards = value;
    return chat_guards;
}

// 获取当前用于请求的提示JSON
yyjson_mut_doc* Chat::getCurrentPrompt() {
    if (using_oai_completion) {
        return getOAIPrompt();
    }
    return getPromptJSON();
}