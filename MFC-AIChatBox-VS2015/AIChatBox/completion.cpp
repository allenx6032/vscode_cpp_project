#include "pch.h"
#include "any.hpp"
#include "AIChatBoxDlg.h"
#include "completion.h"

// ȫ�ֱ��������ڱ���Ƿ�ֹͣ��ɲ���
bool stopCompletionFlag = false;
// ȫ�ֱ��������ڱ����ɲ����Ƿ����ڽ���
bool completionInProgress = false;

// Completion��ĳ�Ա������������ɲ����Ļص�
// chunk: ���յ������ݿ�
// bus: �ص�����ָ��
bool Completion::completionCallback(const std::string& chunk, const CallbackBus* bus) {
    // �����ɲ������ڽ��У������ն˱���
    if (completionInProgress) {
        //Terminal::setTitle("Completing...");
    }
    // ����û�������ֹͣ��־
    if (stopCompletionFlag) {
        // �����ն˱���
        //Terminal::setTitle("Interrupted by user");
        // ����ֹͣ��־
        stopCompletionFlag = false;
        // �����ɲ����ѽ���
        completionInProgress = false;
        // ����false��ֹͣ����������
        return false;
    }

    // �����ݿ�����ȡjsonƬ�Σ���ʼ��Ϊ����chunk
    std::string completionData = chunk;
    // �������ʽ����
    if (const_cast<CallbackBus*>(bus)->stream) {
        // ����"data: "��λ��
        size_t dataKeyPos = chunk.find("data: ");
        // ���û�ҵ���ֱ�ӷ���true�������ȴ���������
        if (dataKeyPos == std::string::npos) return true;
        // ��ȡ"data: "֮�������
        completionData = chunk.substr(dataKeyPos + 6);
    }
    else {
        // ����""content":"��λ��
        size_t contentKeyPos = chunk.find("\"content\":");
        // ���û�ҵ���ֱ�ӷ���true
        if (contentKeyPos == std::string::npos) return true;
    }

    // ��¼���յ�������
    Logging::log("Received data: %s", completionData.c_str());

    // �������յ���json����
    yyjson_doc* doc = yyjson_read(completionData.c_str(), strlen(completionData.c_str()), 0);

    // �������ʧ�ܣ��ͷ��ĵ���Դ������false
    if (doc == NULL) {
        yyjson_doc_free(doc);
        return false;
    }

    // ��ȡjson���ڵ�
    yyjson_val* root = yyjson_doc_get_root(doc);

    // ���ڴ洢���ɵ��ı�Ƭ��
    std::string token = "";
    // �����ɲ����Ƿ����
    bool endOfCompletation = false;
    // ÿ�����ɵ�token��
    double tokenPerSeconds = 0.0;

    // ���ʹ�õ���ĳ���ض�����ɷ�ʽ��oai_completion��
    if (using_oai_completion) {
        // ��ȡ"choices"�ڵ�
        yyjson_val* choices = yyjson_obj_get(root, "choices");
        // ��ȡ��һ��ѡ�����������
        yyjson_val* content_array = yyjson_arr_get(choices, 0);
        // ��ȡ���ԭ��ڵ�
        yyjson_val* finish_reason = yyjson_obj_get(content_array, "finish_reason");

        // ������ԭ��Ϊ��
        if (!yyjson_is_null(finish_reason)) {
            // ��ȡʱ����ؽڵ�
            yyjson_val* timings = yyjson_obj_get(root, "timings");
            // ��ȡÿ��Ԥ���token���ڵ�
            yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
            // ��ȡÿ�����ɵ�token��
            tokenPerSeconds = yyjson_get_real(predicted_per_second);
            // �����ɲ�������
            endOfCompletation = true;
        }
        else {
            // ��ȡdelta�ڵ�
            yyjson_val* delta = yyjson_obj_get(content_array, "delta");
            // ��ȡ���ݽڵ�
            yyjson_val* content = yyjson_obj_get(delta, "content");
            // ��ȡ�ı�����
            token = yyjson_get_str(content);
        }
    }
    else {
        // ��ȡ���ݽڵ�
        yyjson_val* content = yyjson_obj_get(root, "content");
        // ��ȡ�ı�����
        token = yyjson_get_str(content);

        // ��ȡʱ����ؽڵ�
        yyjson_val* timings = yyjson_obj_get(root, "timings");
        // ��ȡÿ��Ԥ���token���ڵ�
        yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
        // ��ȡÿ�����ɵ�token��
        tokenPerSeconds = yyjson_get_real(predicted_per_second);
        // ��ȡֹͣ�ڵ�
        yyjson_val* stop = yyjson_obj_get(root, "stop");
        // �ж��Ƿ�ֹͣ
        endOfCompletation = yyjson_get_bool(stop);
    }

    // �ͷ�json�ĵ���Դ
    yyjson_doc_free(doc);

    // �����ɲ�������
    if (endOfCompletation) {
        // ���������ʽ����
        if (!const_cast<CallbackBus*>(bus)->stream) {
            // ���ʣ����ı�����
            std::cout << token;
            // ���ı�������ӵ�������
            const_cast<CallbackBus*>(bus)->buffer += token;
        }

        // ��ʽ�����ڱ��⣬��ʾÿ���token��
        char windowTitle[30];
        std::snprintf(windowTitle, sizeof(windowTitle), "%.1f t/s\n", tokenPerSeconds);
        // �����ն˱���
        //Terminal::setTitle(windowTitle);
        // �����ɲ����ѽ���
        completionInProgress = false;
        // ����false��ֹͣ����������
        return false;
    }

    // �����ǰ���ı�Ƭ�Σ���ˢ�����������
    std::cout << token << std::flush;
    // ���ı�Ƭ����ӵ�������
    const_cast<CallbackBus*>(bus)->buffer += token;
    // ����true���������պ�������
    return true;
}

bool Completion::completionCallback(const std::string& chunk, const CallbackBus* bus, CEdit& edit)
{
    // �����ɲ������ڽ��У������ն˱���
    if (completionInProgress) {
        // Terminal::setTitle("Completing...");
    }
    // ����û�������ֹͣ��־
    if (stopCompletionFlag) {
        // �����ն˱���
        // Terminal::setTitle("Interrupted by user");
        // ����ֹͣ��־
        stopCompletionFlag = false;
        // �����ɲ����ѽ���
        completionInProgress = false;
        // ����false��ֹͣ����������
        return false;
    }

    // �����ݿ�����ȡjsonƬ�Σ���ʼ��Ϊ����chunk
    std::string completionData = chunk;
    // �������ʽ����
    if (const_cast<CallbackBus*>(bus)->stream) {
        // ����"data: "��λ��
        size_t dataKeyPos = chunk.find("data: ");
        // ���û�ҵ���ֱ�ӷ���true�������ȴ���������
        if (dataKeyPos == std::string::npos) return true;
        // ��ȡ"data: "֮�������
        completionData = chunk.substr(dataKeyPos + 6);
    }
    else {
        // ����""content":"��λ��
        size_t contentKeyPos = chunk.find("\"content\":");
        // ���û�ҵ���ֱ�ӷ���true
        if (contentKeyPos == std::string::npos) return true;
    }

    // ��¼���յ�������
    Logging::log("Received data: %s", completionData.c_str());

    // �������յ���json����
    yyjson_doc* doc = yyjson_read(completionData.c_str(), strlen(completionData.c_str()), 0);

    // �������ʧ�ܣ��ͷ��ĵ���Դ������false
    if (doc == NULL) {
        yyjson_doc_free(doc);
        return false;
    }

    // ��ȡjson���ڵ�
    yyjson_val* root = yyjson_doc_get_root(doc);

    // ���ڴ洢���ɵ��ı�Ƭ��
    std::string token = "";
    // �����ɲ����Ƿ����
    bool endOfCompletation = false;
    // ÿ�����ɵ�token��
    double tokenPerSeconds = 0.0;

    // ���ʹ�õ���ĳ���ض�����ɷ�ʽ��oai_completion��
    if (using_oai_completion) {
        // ��ȡ"choices"�ڵ�
        yyjson_val* choices = yyjson_obj_get(root, "choices");
        // ��ȡ��һ��ѡ�����������
        yyjson_val* content_array = yyjson_arr_get(choices, 0);
        // ��ȡ���ԭ��ڵ�
        yyjson_val* finish_reason = yyjson_obj_get(content_array, "finish_reason");

        // ������ԭ��Ϊ��
        if (!yyjson_is_null(finish_reason)) {
            // ��ȡʱ����ؽڵ�
            yyjson_val* timings = yyjson_obj_get(root, "timings");
            // ��ȡÿ��Ԥ���token���ڵ�
            yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
            // ��ȡÿ�����ɵ�token��
            tokenPerSeconds = yyjson_get_real(predicted_per_second);
            // �����ɲ�������
            endOfCompletation = true;
        }
        else {
            // ��ȡdelta�ڵ�
            yyjson_val* delta = yyjson_obj_get(content_array, "delta");
            // ��ȡ���ݽڵ�
            yyjson_val* content = yyjson_obj_get(delta, "content");
            // ��ȡ�ı�����
            token = yyjson_get_str(content);
        }
    }
    else {
        // ��ȡ���ݽڵ�
        yyjson_val* content = yyjson_obj_get(root, "content");
        // ��ȡ�ı�����
        token = yyjson_get_str(content);

        // ��ȡʱ����ؽڵ�
        yyjson_val* timings = yyjson_obj_get(root, "timings");
        // ��ȡÿ��Ԥ���token���ڵ�
        yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
        // ��ȡÿ�����ɵ�token��
        tokenPerSeconds = yyjson_get_real(predicted_per_second);
        // ��ȡֹͣ�ڵ�
        yyjson_val* stop = yyjson_obj_get(root, "stop");
        // �ж��Ƿ�ֹͣ
        endOfCompletation = yyjson_get_bool(stop);
    }

    // �ͷ�json�ĵ���Դ
    yyjson_doc_free(doc);

    // �����ɲ�������
    if (endOfCompletation) {
        // ���������ʽ����
        if (!const_cast<CallbackBus*>(bus)->stream) {
            // ���ʣ����ı�����
            // std::cout << token;
            
            // ��UTF-8�ַ���ת��ΪUTF-16�ַ���
            CStringW utf16String = ConvertUtf8ToUtf16(token);

            edit.ReplaceSel(utf16String);

            // ���ı�������ӵ�������
            const_cast<CallbackBus*>(bus)->buffer += token;
        }

        // ��ʽ�����ڱ��⣬��ʾÿ���token��
        char windowTitle[30];
        std::snprintf(windowTitle, sizeof(windowTitle), "%.1f t/s\n", tokenPerSeconds);
        // �����ն˱���
        //Terminal::setTitle(windowTitle);


        // �����ɲ����ѽ���
        completionInProgress = false;
        // ����false��ֹͣ����������
        return false;
    }

    // �����ǰ���ı�Ƭ�Σ���ˢ�����������
    std::cout << token << std::flush;

    // ����ǰ���ı�Ƭ����ӵ�edit��

    // ��UTF-8�ַ���ת��ΪUTF-16�ַ���
    CStringW utf16String = ConvertUtf8ToUtf16(token);

    edit.ReplaceSel(utf16String);

    // ���ı�Ƭ����ӵ�������
    const_cast<CallbackBus*>(bus)->buffer += token;

    // ����true���������պ�������
    return true;
}

bool Completion::completionCallback(const std::string& chunk, const CallbackBus* bus, CRichEditCtrl& edit)
{
    // �����ɲ������ڽ��У������ն˱���
    if (completionInProgress) {
        // Terminal::setTitle("Completing...");
    }
    // ����û�������ֹͣ��־
    if (stopCompletionFlag) {
        // �����ն˱���
        // Terminal::setTitle("Interrupted by user");
        // ����ֹͣ��־
        stopCompletionFlag = false;
        // �����ɲ����ѽ���
        completionInProgress = false;
        // ����false��ֹͣ����������
        return false;
    }

    // �����ݿ�����ȡjsonƬ�Σ���ʼ��Ϊ����chunk
    std::string completionData = chunk;
    // �������ʽ����
    if (const_cast<CallbackBus*>(bus)->stream) {
        // ����"data: "��λ��
        size_t dataKeyPos = chunk.find("data: ");
        // ���û�ҵ���ֱ�ӷ���true�������ȴ���������
        if (dataKeyPos == std::string::npos) return true;
        // ��ȡ"data: "֮�������
        completionData = chunk.substr(dataKeyPos + 6);
    }
    else {
        // ����""content":"��λ��
        size_t contentKeyPos = chunk.find("\"content\":");
        // ���û�ҵ���ֱ�ӷ���true
        if (contentKeyPos == std::string::npos) return true;
    }

    // ��¼���յ�������
    Logging::log("Received data: %s", completionData.c_str());

    // �������յ���json����
    yyjson_doc* doc = yyjson_read(completionData.c_str(), strlen(completionData.c_str()), 0);

    // �������ʧ�ܣ��ͷ��ĵ���Դ������false
    if (doc == NULL) {
        yyjson_doc_free(doc);
        return false;
    }

    // ��ȡjson���ڵ�
    yyjson_val* root = yyjson_doc_get_root(doc);

    // ���ڴ洢���ɵ��ı�Ƭ��
    std::string token = "";
    // �����ɲ����Ƿ����
    bool endOfCompletation = false;
    // ÿ�����ɵ�token��
    double tokenPerSeconds = 0.0;

    // ���ʹ�õ���ĳ���ض�����ɷ�ʽ��oai_completion��
    if (using_oai_completion) {
        // ��ȡ"choices"�ڵ�
        yyjson_val* choices = yyjson_obj_get(root, "choices");
        // ��ȡ��һ��ѡ�����������
        yyjson_val* content_array = yyjson_arr_get(choices, 0);
        // ��ȡ���ԭ��ڵ�
        yyjson_val* finish_reason = yyjson_obj_get(content_array, "finish_reason");

        // ������ԭ��Ϊ��
        if (!yyjson_is_null(finish_reason)) {
            // ��ȡʱ����ؽڵ�
            yyjson_val* timings = yyjson_obj_get(root, "timings");
            // ��ȡÿ��Ԥ���token���ڵ�
            yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
            // ��ȡÿ�����ɵ�token��
            tokenPerSeconds = yyjson_get_real(predicted_per_second);
            // �����ɲ�������
            endOfCompletation = true;
        }
        else {
            // ��ȡdelta�ڵ�
            yyjson_val* delta = yyjson_obj_get(content_array, "delta");
            // ��ȡ���ݽڵ�
            yyjson_val* content = yyjson_obj_get(delta, "content");
            // ��ȡ�ı�����
            token = yyjson_get_str(content);
        }
    }
    else {
        // ��ȡ���ݽڵ�
        yyjson_val* content = yyjson_obj_get(root, "content");
        // ��ȡ�ı�����
        token = yyjson_get_str(content);

        // ��ȡʱ����ؽڵ�
        yyjson_val* timings = yyjson_obj_get(root, "timings");
        // ��ȡÿ��Ԥ���token���ڵ�
        yyjson_val* predicted_per_second = yyjson_obj_get(timings, "predicted_per_second");
        // ��ȡÿ�����ɵ�token��
        tokenPerSeconds = yyjson_get_real(predicted_per_second);
        // ��ȡֹͣ�ڵ�
        yyjson_val* stop = yyjson_obj_get(root, "stop");
        // �ж��Ƿ�ֹͣ
        endOfCompletation = yyjson_get_bool(stop);
    }

    // �ͷ�json�ĵ���Դ
    yyjson_doc_free(doc);

    // �����ɲ�������
    if (endOfCompletation) {
        // ���������ʽ����
        if (!const_cast<CallbackBus*>(bus)->stream) {
            // ���ʣ����ı�����
            // std::cout << token;

            // ��UTF-8�ַ���ת��ΪUTF-16�ַ���
            CStringW utf16String = ConvertUtf8ToUtf16(token);

            edit.ReplaceSel(utf16String);

            // ���ı�������ӵ�������
            const_cast<CallbackBus*>(bus)->buffer += token;
        }

        // ��ʽ�����ڱ��⣬��ʾÿ���token��
        char windowTitle[30];
        std::snprintf(windowTitle, sizeof(windowTitle), "%.1f t/s\n", tokenPerSeconds);
        // �����ն˱���
        // Terminal::setTitle(windowTitle);


        CAIChatBoxDlg* pDialog = static_cast<CAIChatBoxDlg*>(AfxGetMainWnd()); // ���� pDialog ��ָ�������ڵ�ָ��

        // ����״̬��
        if (pDialog != nullptr)
        {
            WPARAM wParam = *(WPARAM*)&tokenPerSeconds;
            pDialog->SendMessage(WM_UPDATE_TOKEN_COUNT, wParam, 0);
        }
        


        // �����ɲ����ѽ���
        completionInProgress = false;
        // ����false��ֹͣ����������
        return false;
    }

    CStringW utf16String = ConvertUtf8ToUtf16(token);
    edit.SetSel(-1, -1);
    // edit.SetSelectionCharFormat(cf2);
    edit.ReplaceSel(utf16String);

    // �������Զ����������
    edit.LineScroll(edit.GetLineCount());

    // ���ı�Ƭ����ӵ�������
    const_cast<CallbackBus*>(bus)->buffer += token;
    // ����true���������պ�������
    return true;
}

// ��params.json�ļ���ȡ��������
// profile_name: �����ļ�������ͼ
bool Completion::loadParametersSettings(std::string profile_name) {
    // �򿪲����ļ�
    sjson parameters_file = sjson(PARAMS_FILENAME);
    // ����ļ���ʧ��
    if (!parameters_file.is_opened())
        return false;

    // �������������ļ��е�profile·��
    const char* PARAMETER_PROFILE_[] = { "params_profiles", profile_name.data(), "\0" };
    // ��ȡָ��profile�Ľڵ�
    yyjson_val* parameter_profile = parameters_file.get_value(PARAMETER_PROFILE_);
    // �����ȡʧ��
    if (parameter_profile == NULL) return false;

    yyjson_val* key, * val;
    // ��ȡprofile�ڵ�ĵ�����
    yyjson_obj_iter iter = yyjson_obj_iter_with(parameter_profile);

    // ��յ�ǰ����
    parameters.clear();

    // ����profile�ڵ��µļ�ֵ��
    while ((key = yyjson_obj_iter_next(&iter))) {
        // ��ȡ�����ַ�����ʾ
        const char* param_key = yyjson_get_str(key);
        // ��ȡֵ�ڵ�
        yyjson_val* param_value = yyjson_obj_iter_get_val(key);
        // ��ȡֵ����������
        std::string value_type = yyjson_get_type_desc(param_value);

        // ������޷�����������
        if (value_type == "uint") {
            // ����������Ϊ��������
            parameters.set<int>(param_key, yyjson_get_int(param_value));
        }
        else if (value_type == "real") {
            // ����������Ϊ˫���ȸ���������
            parameters.set<double>(param_key, yyjson_get_real(param_value));
        }
        else if (value_type == "string") {
            // ����������Ϊ�ַ������ͣ��������
            parameters.set<std::string>(param_key, "\"" + std::string(yyjson_get_str(param_value)) + "\"");
        }
        else if (value_type == "true") {
            // ����������Ϊ����true
            parameters.set<bool>(param_key, true);
        }
        else if (value_type == "false") {
            // ����������Ϊ����false
            parameters.set<bool>(param_key, false);
        }
        else if (value_type == "array") {
            // �����"stop"����
            if (!strcmp(param_key, "stop")) {
                size_t idx, max;
                yyjson_val* hit;
                // ��������Ԫ�أ���ӵ�ֹͣ���б�
                yyjson_arr_foreach(param_value, idx, max, hit)
                    stop_words.push_back(std::string(yyjson_get_str(hit)));
            }
        }
    }

    // ��������ļ���"stream"����Ϊfalse�����ûص����ߵ�������Ϊfalse
    if (this->parameters.get<bool>("stream") == false)
        completionBus.stream = false;

    return true;
}

// ��params.json�ļ���������ģ��
// chat_template_name: ����ģ��������ͼ
bool Completion::loadChatTemplates(std::string chat_template_name) {
    // ��ģ���ļ�
    sjson templates_fp = sjson(TEMPLATES_FILENAME);
    // ����ļ���ʧ��
    if (!templates_fp.is_opened()) return false;

    // ����ģ���ļ��е���·��
    const char* main_key[] = { "chat_templates", chat_template_name.data(), "\0" };

    // �����ȡ��·����Ӧ�Ľڵ�ʧ��
    if (templates_fp.get_value(main_key) == NULL) return false;

    // ����lambda���ʽ�����ڼ�������token
    const auto loadSeqToken = [&](const char* key, const char* field_key) {
        const char* keys[] = { "chat_templates", chat_template_name.data(), key, field_key, "\0" };
        const char* key_value = templates_fp.get_str(keys);
        return key_value ? key_value : "";
        };

    // ���ظ�������ģ����
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

// ���Ҫ���͵�json����
// prompt_json: ������ʾ��Ϣ��json�ĵ�ָ��
std::string Completion::dumpPayload(yyjson_mut_doc* prompt_json) {
    yyjson_mut_doc* doc = yyjson_mut_doc_new(nullptr);
    yyjson_mut_val* root = yyjson_mut_obj(doc);
    yyjson_mut_doc_set_root(doc, root);

    yyjson_mut_val* prompt_root = yyjson_mut_doc_get_root(prompt_json);

    size_t idx, max;
    yyjson_mut_val* key, * val;
    yyjson_mut_obj_foreach(prompt_root, idx, max, key, val) {
        if (!yyjson_mut_is_str(key)) continue; // �������ַ�����
        const char* keyStr = yyjson_mut_get_str(key);
        if (!keyStr) continue; // ��ֹ��ָ��
        yyjson_mut_obj_add_val(doc, root, keyStr, yyjson_mut_val_mut_copy(doc, val));
    }

    // �޸�ѭ����������
    const char** stop_word_arr = new const char* [stop_words.size()];
    for (size_t i = 0; i < stop_words.size(); i++) {
        stop_word_arr[i] = stop_words[i].c_str();
    }
    yyjson_mut_val* hits = yyjson_mut_arr_with_str(doc, stop_word_arr, stop_words.size());
    yyjson_mut_obj_add_val(doc, root, "stop", hits);

    // ��������������������ӵ���json
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
    if (!json) { // ����ָ��
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
    // �����ɲ������ڽ���
    completionInProgress = true;
    // ���Ҫ���͵�json����
    std::string send_json = dumpPayload(prompt_json);
    // ��¼Ҫ���͵�����
    Logging::log("Sending: %s", send_json.c_str());

    // ����POST���󣬴���ص�����������Ӧ
    return Req.post(
        ipaddr,
        port,
        using_oai_completion ? OAI_COMPLETION_ENDPOINT : COMPLETION_ENDPOINT,
        send_json,
        [this](std::string response, const CallbackBus* bus, CRichEditCtrl& edit) -> bool {  // �ص�����
            return this->completionCallback(response, bus, edit);
        },
        &completionBus,
        edit
    );
}

// ��ȡ����ģ��
chat_template_t& Completion::getChatTemplates() {
    return chat_template;
}

// ���ֹͣ��
void Completion::addStopWord(std::string word) {
    stop_words.push_back(word);
}