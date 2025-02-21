#include "pch.h"
#include "any.hpp"
#include "AIChatBoxDlg.h"
#include "completion.h"

// 全局变量，用于标记是否停止完成操作
bool stopCompletionFlag = false;
// 全局变量，用于标记完成操作是否正在进行
bool completionInProgress = false;

// Completion类的成员函数，处理完成操作的回调
// chunk: 接收到的数据块
// bus: 回调总线指针
bool Completion::completionCallback(const std::string& chunk, const CallbackBus* bus) {
    // 如果完成操作正在进行，更新终端标题
    if (completionInProgress) {
        //Terminal::setTitle("Completing...");
    }
    // 如果用户触发了停止标志
    if (stopCompletionFlag) {
        // 更新终端标题
        //Terminal::setTitle("Interrupted by user");
        // 重置停止标志
        stopCompletionFlag = false;
        // 标记完成操作已结束
        completionInProgress = false;
        // 返回false，停止后续流操作
        return false;
    }

    // 从数据块中提取json片段，初始化为整个chunk
    std::string completionData = chunk;
    // 如果是流式数据
    if (const_cast<CallbackBus*>(bus)->stream) {
        // 查找"data: "的位置
        size_t dataKeyPos = chunk.find("data: ");
        // 如果没找到，直接返回true，继续等待后续数据
        if (dataKeyPos == std::string::npos) return true;
        // 提取"data: "之后的数据
        completionData = chunk.substr(dataKeyPos + 6);
    }
    else {
        // 查找""content":"的位置
        size_t contentKeyPos = chunk.find("\"content\":");
        // 如果没找到，直接返回true
        if (contentKeyPos == std::string::npos) return true;
    }

    // 记录接收到的数据
    Logging::log("Received data: %s", completionData.c_str());

    // 解析接收到的json数据
    yyjson_doc* doc = yyjson_read(completionData.c_str(), strlen(completionData.c_str()), 0);

    // 如果解析失败，释放文档资源并返回false
    if (doc == NULL) {
        yyjson_doc_free(doc);
        return false;
    }

    // 获取json根节点
    yyjson_val* root = yyjson_doc_get_root(doc);

    // 用于存储生成的文本片段
    std::string token = "";
    // 标记完成操作是否结束
    bool endOfCompletation = false;
    // 每秒生成的token数
    double tokenPerSeconds = 0.0;

    // 如果使用的是某种特定的完成方式（oai_completion）
    if (using_oai_completion) {
        // 获取"choices"节点
        yyjson_val* choices = yyjson_obj_get(root, "choices");
        // 获取第一个选择的内容数组
        yyjson_val* content_array = yyjson_arr_get(choices, 0);
        // 获取完成原因节点
        yyjson_val* finish_reason = yyjson_obj_get(content_array, "finish_reason");

        // 如果完成原因不为空
        if (!yyjson_is_null(finish_reason)) {
            // 获取时间相关节点
            yyjson_val* timings = yyjson_obj_get(root, "timings");
            // 获取每秒预测的token数节点
            yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
            // 获取每秒生成的token数
            tokenPerSeconds = yyjson_get_real(predicted_per_second);
            // 标记完成操作结束
            endOfCompletation = true;
        }
        else {
            // 获取delta节点
            yyjson_val* delta = yyjson_obj_get(content_array, "delta");
            // 获取内容节点
            yyjson_val* content = yyjson_obj_get(delta, "content");
            // 获取文本内容
            token = yyjson_get_str(content);
        }
    }
    else {
        // 获取内容节点
        yyjson_val* content = yyjson_obj_get(root, "content");
        // 获取文本内容
        token = yyjson_get_str(content);

        // 获取时间相关节点
        yyjson_val* timings = yyjson_obj_get(root, "timings");
        // 获取每秒预测的token数节点
        yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
        // 获取每秒生成的token数
        tokenPerSeconds = yyjson_get_real(predicted_per_second);
        // 获取停止节点
        yyjson_val* stop = yyjson_obj_get(root, "stop");
        // 判断是否停止
        endOfCompletation = yyjson_get_bool(stop);
    }

    // 释放json文档资源
    yyjson_doc_free(doc);

    // 如果完成操作结束
    if (endOfCompletation) {
        // 如果不是流式数据
        if (!const_cast<CallbackBus*>(bus)->stream) {
            // 输出剩余的文本内容
            std::cout << token;
            // 将文本内容添加到缓冲区
            const_cast<CallbackBus*>(bus)->buffer += token;
        }

        // 格式化窗口标题，显示每秒的token数
        char windowTitle[30];
        std::snprintf(windowTitle, sizeof(windowTitle), "%.1f t/s\n", tokenPerSeconds);
        // 设置终端标题
        //Terminal::setTitle(windowTitle);
        // 标记完成操作已结束
        completionInProgress = false;
        // 返回false，停止后续流操作
        return false;
    }

    // 输出当前的文本片段，并刷新输出缓冲区
    std::cout << token << std::flush;
    // 将文本片段添加到缓冲区
    const_cast<CallbackBus*>(bus)->buffer += token;
    // 返回true，继续接收后续数据
    return true;
}

bool Completion::completionCallback(const std::string& chunk, const CallbackBus* bus, CEdit& edit)
{
    // 如果完成操作正在进行，更新终端标题
    if (completionInProgress) {
        // Terminal::setTitle("Completing...");
    }
    // 如果用户触发了停止标志
    if (stopCompletionFlag) {
        // 更新终端标题
        // Terminal::setTitle("Interrupted by user");
        // 重置停止标志
        stopCompletionFlag = false;
        // 标记完成操作已结束
        completionInProgress = false;
        // 返回false，停止后续流操作
        return false;
    }

    // 从数据块中提取json片段，初始化为整个chunk
    std::string completionData = chunk;
    // 如果是流式数据
    if (const_cast<CallbackBus*>(bus)->stream) {
        // 查找"data: "的位置
        size_t dataKeyPos = chunk.find("data: ");
        // 如果没找到，直接返回true，继续等待后续数据
        if (dataKeyPos == std::string::npos) return true;
        // 提取"data: "之后的数据
        completionData = chunk.substr(dataKeyPos + 6);
    }
    else {
        // 查找""content":"的位置
        size_t contentKeyPos = chunk.find("\"content\":");
        // 如果没找到，直接返回true
        if (contentKeyPos == std::string::npos) return true;
    }

    // 记录接收到的数据
    Logging::log("Received data: %s", completionData.c_str());

    // 解析接收到的json数据
    yyjson_doc* doc = yyjson_read(completionData.c_str(), strlen(completionData.c_str()), 0);

    // 如果解析失败，释放文档资源并返回false
    if (doc == NULL) {
        yyjson_doc_free(doc);
        return false;
    }

    // 获取json根节点
    yyjson_val* root = yyjson_doc_get_root(doc);

    // 用于存储生成的文本片段
    std::string token = "";
    // 标记完成操作是否结束
    bool endOfCompletation = false;
    // 每秒生成的token数
    double tokenPerSeconds = 0.0;

    // 如果使用的是某种特定的完成方式（oai_completion）
    if (using_oai_completion) {
        // 获取"choices"节点
        yyjson_val* choices = yyjson_obj_get(root, "choices");
        // 获取第一个选择的内容数组
        yyjson_val* content_array = yyjson_arr_get(choices, 0);
        // 获取完成原因节点
        yyjson_val* finish_reason = yyjson_obj_get(content_array, "finish_reason");

        // 如果完成原因不为空
        if (!yyjson_is_null(finish_reason)) {
            // 获取时间相关节点
            yyjson_val* timings = yyjson_obj_get(root, "timings");
            // 获取每秒预测的token数节点
            yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
            // 获取每秒生成的token数
            tokenPerSeconds = yyjson_get_real(predicted_per_second);
            // 标记完成操作结束
            endOfCompletation = true;
        }
        else {
            // 获取delta节点
            yyjson_val* delta = yyjson_obj_get(content_array, "delta");
            // 获取内容节点
            yyjson_val* content = yyjson_obj_get(delta, "content");
            // 获取文本内容
            token = yyjson_get_str(content);
        }
    }
    else {
        // 获取内容节点
        yyjson_val* content = yyjson_obj_get(root, "content");
        // 获取文本内容
        token = yyjson_get_str(content);

        // 获取时间相关节点
        yyjson_val* timings = yyjson_obj_get(root, "timings");
        // 获取每秒预测的token数节点
        yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
        // 获取每秒生成的token数
        tokenPerSeconds = yyjson_get_real(predicted_per_second);
        // 获取停止节点
        yyjson_val* stop = yyjson_obj_get(root, "stop");
        // 判断是否停止
        endOfCompletation = yyjson_get_bool(stop);
    }

    // 释放json文档资源
    yyjson_doc_free(doc);

    // 如果完成操作结束
    if (endOfCompletation) {
        // 如果不是流式数据
        if (!const_cast<CallbackBus*>(bus)->stream) {
            // 输出剩余的文本内容
            // std::cout << token;
            
            // 将UTF-8字符串转换为UTF-16字符串
            CStringW utf16String = ConvertUtf8ToUtf16(token);

            edit.ReplaceSel(utf16String);

            // 将文本内容添加到缓冲区
            const_cast<CallbackBus*>(bus)->buffer += token;
        }

        // 格式化窗口标题，显示每秒的token数
        char windowTitle[30];
        std::snprintf(windowTitle, sizeof(windowTitle), "%.1f t/s\n", tokenPerSeconds);
        // 设置终端标题
        //Terminal::setTitle(windowTitle);


        // 标记完成操作已结束
        completionInProgress = false;
        // 返回false，停止后续流操作
        return false;
    }

    // 输出当前的文本片段，并刷新输出缓冲区
    std::cout << token << std::flush;

    // 将当前的文本片段添加到edit里

    // 将UTF-8字符串转换为UTF-16字符串
    CStringW utf16String = ConvertUtf8ToUtf16(token);

    edit.ReplaceSel(utf16String);

    // 将文本片段添加到缓冲区
    const_cast<CallbackBus*>(bus)->buffer += token;

    // 返回true，继续接收后续数据
    return true;
}

bool Completion::completionCallback(const std::string& chunk, const CallbackBus* bus, CRichEditCtrl& edit)
{
    // 如果完成操作正在进行，更新终端标题
    if (completionInProgress) {
        // Terminal::setTitle("Completing...");
    }
    // 如果用户触发了停止标志
    if (stopCompletionFlag) {
        // 更新终端标题
        // Terminal::setTitle("Interrupted by user");
        // 重置停止标志
        stopCompletionFlag = false;
        // 标记完成操作已结束
        completionInProgress = false;
        // 返回false，停止后续流操作
        return false;
    }

    // 从数据块中提取json片段，初始化为整个chunk
    std::string completionData = chunk;
    // 如果是流式数据
    if (const_cast<CallbackBus*>(bus)->stream) {
        // 查找"data: "的位置
        size_t dataKeyPos = chunk.find("data: ");
        // 如果没找到，直接返回true，继续等待后续数据
        if (dataKeyPos == std::string::npos) return true;
        // 提取"data: "之后的数据
        completionData = chunk.substr(dataKeyPos + 6);
    }
    else {
        // 查找""content":"的位置
        size_t contentKeyPos = chunk.find("\"content\":");
        // 如果没找到，直接返回true
        if (contentKeyPos == std::string::npos) return true;
    }

    // 记录接收到的数据
    Logging::log("Received data: %s", completionData.c_str());

    // 解析接收到的json数据
    yyjson_doc* doc = yyjson_read(completionData.c_str(), strlen(completionData.c_str()), 0);

    // 如果解析失败，释放文档资源并返回false
    if (doc == NULL) {
        yyjson_doc_free(doc);
        return false;
    }

    // 获取json根节点
    yyjson_val* root = yyjson_doc_get_root(doc);

    // 用于存储生成的文本片段
    std::string token = "";
    // 标记完成操作是否结束
    bool endOfCompletation = false;
    // 每秒生成的token数
    double tokenPerSeconds = 0.0;

    // 如果使用的是某种特定的完成方式（oai_completion）
    if (using_oai_completion) {
        // 获取"choices"节点
        yyjson_val* choices = yyjson_obj_get(root, "choices");
        // 获取第一个选择的内容数组
        yyjson_val* content_array = yyjson_arr_get(choices, 0);
        // 获取完成原因节点
        yyjson_val* finish_reason = yyjson_obj_get(content_array, "finish_reason");

        // 如果完成原因不为空
        if (!yyjson_is_null(finish_reason)) {
            // 获取时间相关节点
            yyjson_val* timings = yyjson_obj_get(root, "timings");
            // 获取每秒预测的token数节点
            yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
            // 获取每秒生成的token数
            tokenPerSeconds = yyjson_get_real(predicted_per_second);
            // 标记完成操作结束
            endOfCompletation = true;
        }
        else {
            // 获取delta节点
            yyjson_val* delta = yyjson_obj_get(content_array, "delta");
            // 获取内容节点
            yyjson_val* content = yyjson_obj_get(delta, "content");
            // 获取文本内容
            token = yyjson_get_str(content);
        }
    }
    else {
        // 获取内容节点
        yyjson_val* content = yyjson_obj_get(root, "content");
        // 获取文本内容
        token = yyjson_get_str(content);

        // 获取时间相关节点
        yyjson_val* timings = yyjson_obj_get(root, "timings");
        // 获取每秒预测的token数节点
        yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
        // 获取每秒生成的token数
        tokenPerSeconds = yyjson_get_real(predicted_per_second);
        // 获取停止节点
        yyjson_val* stop = yyjson_obj_get(root, "stop");
        // 判断是否停止
        endOfCompletation = yyjson_get_bool(stop);
    }

    // 释放json文档资源
    yyjson_doc_free(doc);

    // 如果完成操作结束
    if (endOfCompletation) {
        // 如果不是流式数据
        if (!const_cast<CallbackBus*>(bus)->stream) {
            // 输出剩余的文本内容
            // std::cout << token;

            // 将UTF-8字符串转换为UTF-16字符串
            CStringW utf16String = ConvertUtf8ToUtf16(token);

            edit.ReplaceSel(utf16String);

            // 将文本内容添加到缓冲区
            const_cast<CallbackBus*>(bus)->buffer += token;
        }

        // 格式化窗口标题，显示每秒的token数
        char windowTitle[30];
        std::snprintf(windowTitle, sizeof(windowTitle), "%.1f t/s\n", tokenPerSeconds);
        // 设置终端标题
        // Terminal::setTitle(windowTitle);


        CAIChatBoxDlg* pDialog = static_cast<CAIChatBoxDlg*>(AfxGetMainWnd()); // 假设 pDialog 是指向主窗口的指针

        // 更新状态栏
        if (pDialog != nullptr)
        {
            WPARAM wParam = *(WPARAM*)&tokenPerSeconds;
            pDialog->SendMessage(WM_UPDATE_TOKEN_COUNT, wParam, 0);
        }
        


        // 标记完成操作已结束
        completionInProgress = false;
        // 返回false，停止后续流操作
        return false;
    }

    CStringW utf16String = ConvertUtf8ToUtf16(token);
    edit.SetSel(-1, -1);
    // edit.SetSelectionCharFormat(cf2);
    edit.ReplaceSel(utf16String);

    // 滚动条自动滚动到最后
    edit.LineScroll(edit.GetLineCount());

    // 将文本片段添加到缓冲区
    const_cast<CallbackBus*>(bus)->buffer += token;
    // 返回true，继续接收后续数据
    return true;
}

// 从params.json文件读取参数设置
// profile_name: 配置文件名称视图
bool Completion::loadParametersSettings(std::string profile_name) {
    // 打开参数文件
    sjson parameters_file = sjson(PARAMS_FILENAME);
    // 如果文件打开失败
    if (!parameters_file.is_opened())
        return false;

    // 构建参数配置文件中的profile路径
    const char* PARAMETER_PROFILE_[] = { "params_profiles", profile_name.data(), "\0" };
    // 获取指定profile的节点
    yyjson_val* parameter_profile = parameters_file.get_value(PARAMETER_PROFILE_);
    // 如果获取失败
    if (parameter_profile == NULL) return false;

    yyjson_val* key, * val;
    // 获取profile节点的迭代器
    yyjson_obj_iter iter = yyjson_obj_iter_with(parameter_profile);

    // 清空当前参数
    parameters.clear();

    // 遍历profile节点下的键值对
    while ((key = yyjson_obj_iter_next(&iter))) {
        // 获取键的字符串表示
        const char* param_key = yyjson_get_str(key);
        // 获取值节点
        yyjson_val* param_value = yyjson_obj_iter_get_val(key);
        // 获取值的类型描述
        std::string value_type = yyjson_get_type_desc(param_value);

        // 如果是无符号整数类型
        if (value_type == "uint") {
            // 将参数设置为整数类型
            parameters.set<int>(param_key, yyjson_get_int(param_value));
        }
        else if (value_type == "real") {
            // 将参数设置为双精度浮点数类型
            parameters.set<double>(param_key, yyjson_get_real(param_value));
        }
        else if (value_type == "string") {
            // 将参数设置为字符串类型，添加引号
            parameters.set<std::string>(param_key, "\"" + std::string(yyjson_get_str(param_value)) + "\"");
        }
        else if (value_type == "true") {
            // 将参数设置为布尔true
            parameters.set<bool>(param_key, true);
        }
        else if (value_type == "false") {
            // 将参数设置为布尔false
            parameters.set<bool>(param_key, false);
        }
        else if (value_type == "array") {
            // 如果是"stop"数组
            if (!strcmp(param_key, "stop")) {
                size_t idx, max;
                yyjson_val* hit;
                // 遍历数组元素，添加到停止词列表
                yyjson_arr_foreach(param_value, idx, max, hit)
                    stop_words.push_back(std::string(yyjson_get_str(hit)));
            }
        }
    }

    // 如果配置文件中"stream"参数为false，设置回调总线的流属性为false
    if (this->parameters.get<bool>("stream") == false)
        completionBus.stream = false;

    return true;
}

// 从params.json文件加载聊天模板
// chat_template_name: 聊天模板名称视图
bool Completion::loadChatTemplates(std::string chat_template_name) {
    // 打开模板文件
    sjson templates_fp = sjson(TEMPLATES_FILENAME);
    // 如果文件打开失败
    if (!templates_fp.is_opened()) return false;

    // 构建模板文件中的主路径
    const char* main_key[] = { "chat_templates", chat_template_name.data(), "\0" };

    // 如果获取主路径对应的节点失败
    if (templates_fp.get_value(main_key) == NULL) return false;

    // 定义lambda表达式，用于加载序列token
    const auto loadSeqToken = [&](const char* key, const char* field_key) {
        const char* keys[] = { "chat_templates", chat_template_name.data(), key, field_key, "\0" };
        const char* key_value = templates_fp.get_str(keys);
        return key_value ? key_value : "";
        };

    // 加载各种聊天模板标记
    chat_template.bos = loadSeqToken("SEQ", "BOS");
    chat_template.begin_system = loadSeqToken("SEQ", "B_SYS");
    chat_template.end_system = loadSeqToken("SEQ", "E_SYS");
    chat_template.begin_user = loadSeqToken("SEQ", "B_USER");
    chat_template.end_user = loadSeqToken("SEQ", "E_USER");
    chat_template.begin_assistant = loadSeqToken("SEQ", "B_ASSISTANT");
    chat_template.end_assistant = loadSeqToken("SEQ", "E_ASSISTANT");
    chat_template.eos = loadSeqToken("SEQ", "EOS");

    return true;
}

// 组合要发送的json数据
// prompt_json: 包含提示信息的json文档指针
std::string Completion::dumpPayload(yyjson_mut_doc* prompt_json) {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_val* prompt_root = yyjson_mut_doc_get_root(prompt_json);

    size_t idx, max;
    yyjson_mut_val* key, * val;
    yyjson_mut_obj_foreach(prompt_root, idx, max, key, val) {
        if (!yyjson_mut_is_str(key)) continue; // 跳过非字符串键
        const char* keyStr = yyjson_mut_get_str(key);
        if (!keyStr) continue; // 防止空指针
        yyjson_mut_obj_add_val(doc, root, keyStr, yyjson_mut_val_mut_copy(doc, val));
    }

    // 修复循环变量类型
    const char** stop_word_arr = new const char* [stop_words.size()];
    for (size_t i = 0; i < stop_words.size(); i++) {
        stop_word_arr[i] = stop_words[i].c_str();
    }
    yyjson_mut_val* hits = yyjson_mut_arr_with_str(doc, stop_word_arr, stop_words.size());
    yyjson_mut_obj_add_val(doc, root, "stop", hits);

    // 遍历参数，根据类型添加到主json
	/*parameters.forEach([&doc, &root](const std::string& key, const std::any& value) {
		if (value.type() == typeid(int)) {
			yyjson_mut_obj_add_int(doc, root, key.c_str(), std::any_cast<int>(value));
		}
		else if (value.type() == typeid(double)) {
			yyjson_mut_obj_add_real(doc, root, key.c_str(), std::any_cast<double>(value));
		}
		else if (value.type() == typeid(bool)) {
			yyjson_mut_obj_add_bool(doc, root, key.c_str(), std::any_cast<bool>(value));
		}
		else if (value.type() == typeid(std::string)) {
			yyjson_mut_obj_add_str(doc, root, key.c_str(), std::any_cast<std::string>(value).c_str());
		}
	});*/
	parameters.forEach([&doc, &root](const std::string& key, const Any& value) {
		if (value.type() == typeid(int)) {
			yyjson_mut_obj_add_int(doc, root, key.c_str(), any_cast<int>(value));
		}
		else if (value.type() == typeid(double)) {
			yyjson_mut_obj_add_real(doc, root, key.c_str(), any_cast<double>(value));
		}
		else if (value.type() == typeid(bool)) {
			yyjson_mut_obj_add_bool(doc, root, key.c_str(), any_cast<bool>(value));
		}
		else if (value.type() == typeid(std::string)) {
			yyjson_mut_obj_add_str(doc, root, key.c_str(), any_cast<std::string>(value).c_str());
		}
	});

    const char* json = yyjson_mut_write(doc, 0, nullptr);
    if (!json) { // 检查空指针
        delete[] stop_word_arr;
        yyjson_mut_doc_free(doc);
        return "";
    }
    std::string result(json);

    delete[] stop_word_arr;
    yyjson_mut_doc_free(doc);
    free(const_cast<char*>(json));

    return result;
}

Response Completion::requestCompletion(const char* ipaddr, const int16_t port, yyjson_mut_doc* prompt_json, CRichEditCtrl& edit)
{
    // 标记完成操作正在进行
    completionInProgress = true;
    // 组合要发送的json数据
    std::string send_json = dumpPayload(prompt_json);
    // 记录要发送的数据
    Logging::log("Sending: %s", send_json.c_str());

    // 发起POST请求，传入回调函数处理响应
    return Req.post(
        ipaddr,
        port,
        using_oai_completion ? OAI_COMPLETION_ENDPOINT : COMPLETION_ENDPOINT,
        send_json,
        [this](std::string response, const CallbackBus* bus, CRichEditCtrl& edit) -> bool {  // 回调函数
            return this->completionCallback(response, bus, edit);
        },
        &completionBus,
        edit
    );
}

// 获取聊天模板
chat_template_t& Completion::getChatTemplates() {
    return chat_template;
}

// 添加停止词
void Completion::addStopWord(std::string word) {
    stop_words.push_back(word);
}