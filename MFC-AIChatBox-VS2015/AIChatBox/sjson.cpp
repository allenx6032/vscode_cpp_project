#include "pch.h"
#include "sjson.h"

// ���캯����ʹ�����е�yyjson_doc��������ʼ��
sjson::sjson(yyjson_doc* json_doc) {
    // �������JSON�ĵ�ָ�븳ֵ����ǰ�ĵ�ָ��
    current_doc = json_doc;
    // ��ȡ���ĵ��ĸ��ڵ�
    current_doc_root = yyjson_doc_get_root(current_doc);
}

// ���캯������ָ���ļ������ļ��ж�ȡJSON��������ʼ��
sjson::sjson(const char* filename) {
    // �����ļ���
    doc_filename = filename;
    // ����open������ȡ�ļ�
    open(filename);
}

// ��̬��������ԭʼJSON�ַ���������yyjson_doc����
yyjson_doc* sjson::read(const char* raw_json) {
    // ����yyjson���read�������н���
    return yyjson_read(raw_json, strlen(raw_json), 0);
}

// ��ָ���ļ�����JSON�ļ���������
yyjson_doc* sjson::open(const char* filename) {
    // ʹ��yyjson���read_file������ȡ�ļ�����������ע�ͺ�β�涺�ŵı�־
    current_doc = yyjson_read_file(filename, flg, NULL, &err);
    // �����ȡʧ��
    if (!current_doc) {
        // ���������Ϣ�������ļ����������롢������Ϣ�Լ�����λ��
        std::cout << "Problem reading the file \"" << filename << "\": read error (" << err.code << "): " << err.msg << " at position:" << err.pos << std::endl;
        return NULL;
    }

    // ��ȡ�ĵ��ĸ��ڵ�
    current_doc_root = yyjson_doc_get_root(current_doc);
    return current_doc;
}

// ��ȡJSON�ļ��е�һ���ļ�
std::vector<std::string> sjson::get_top_level_keys() {
    std::vector<std::string> keys;
    if (!current_doc_root) {
        std::cerr << "Error: JSON document is not opened or root is null." << std::endl;
        return keys;
    }

    if (yyjson_is_obj(current_doc_root)) {
        // Iterate over object keys
        yyjson_obj_iter iter;
        yyjson_obj_iter_init(current_doc_root, &iter);
        while (yyjson_obj_iter_has_next(&iter)) {
            yyjson_val* key = yyjson_obj_iter_next(&iter);
            keys.push_back(yyjson_get_str(key));
        }
    } else {
        std::cerr << "Error: Root is not a JSON object." << std::endl;
    }

    return keys;
}

// ��ȡJSON�ļ������еļ�
std::vector<std::string> sjson::get_all_keys() {
    std::vector<std::string> keys;
    if (!current_doc_root) {
        std::cerr << "Error: JSON document is not opened or root is null." << std::endl;
        return keys;
    }

    // Helper function to recursively collect keys
    std::function<void(yyjson_val*, std::string)> collect_keys = [&](yyjson_val* val, std::string prefix) {
        if (yyjson_is_obj(val)) {
            // Iterate over object keys
            yyjson_obj_iter iter;
            yyjson_obj_iter_init(val, &iter);
            while (yyjson_obj_iter_has_next(&iter)) {
                yyjson_val* key = yyjson_obj_iter_next(&iter);
                yyjson_val* child = yyjson_obj_iter_get_val(key);
                std::string full_key = prefix.empty() ? yyjson_get_str(key) : prefix + "." + yyjson_get_str(key);
                keys.push_back(full_key);

                // Recursively collect keys from nested objects/arrays
                collect_keys(child, full_key);
            }
        }
        else if (yyjson_is_arr(val)) {
            // Iterate over array elements
            size_t arr_size = yyjson_arr_size(val);
            for (size_t i = 0; i < arr_size; ++i) {
                yyjson_val* child = yyjson_arr_get(val, i);
                std::string full_key = prefix.empty() ? std::to_string(i) : prefix + "." + std::to_string(i);
                collect_keys(child, full_key);
            }
        }
        };

    // Start collecting keys from the root
    collect_keys(current_doc_root, "");

    return keys;
}

// ��ȡ���Ƕ�׵�JSONֵ
yyjson_val* sjson::get_value(const char* keys[]) {
    yyjson_val* tmp = NULL;
    // ���������飬�𲽻�ȡǶ�׵�ֵ
    for (int i = 0; *keys[i] != '\0'; i++) {
        // ������״β��ң��Ӹ��ڵ㿪ʼ
        if (tmp == NULL)
            tmp = yyjson_obj_get(current_doc_root, keys[i]);
        else
            // �������м�����������
            tmp = yyjson_obj_get(tmp, keys[i]);
        // ���û�ҵ���Ӧ��ֵ
        if (tmp == NULL) {
            // ���������Ϣ��ָ�����ĸ��ļ��л�ȡ�ĸ���ֵʧ��
            std::cerr << "Error getting the value \"" << keys[i] << "\" from \"" << doc_filename << "\"" << std::endl;
            return NULL;
        }
    }
    return tmp;
}


yyjson_val* sjson::get_value(std::string keys) {
    yyjson_val* tmp = NULL;
    std::istringstream key_stream(keys);
    std::string key;
    
    // �������ַ������𲽻�ȡǶ�׵�ֵ
    while (std::getline(key_stream, key, '.')) {
        // ������״β��ң��Ӹ��ڵ㿪ʼ
        if (tmp == NULL)
            tmp = yyjson_obj_get(current_doc_root, key.c_str());
        else
            // �������м�����������
            tmp = yyjson_obj_get(tmp, key.c_str());
        // ���û�ҵ���Ӧ��ֵ
        if (tmp == NULL) {
            // ���������Ϣ��ָ�����ĸ��ļ��л�ȡ�ĸ���ֵʧ��
            std::cerr << "Error getting the value \"" << key << "\" from \"" << doc_filename << "\"" << std::endl;
            return NULL;
        }
    }
    return tmp;
}

// ��ȡ���Ƕ�׵�JSON�ַ���ֵ
const char* sjson::get_str(const char* keys[]) {
    // �Ȼ�ȡ��Ӧ��ֵ
    yyjson_val* value = get_value(keys);
    // �ٳ��Ի�ȡ���ַ�����ʾ�����Ϊ���򷵻ؿ��ַ���
    const char* string = yyjson_get_str(value);
    return string ? string : "";
}

// ��ȡ���Ƕ�׵�JSON����ֵ
int64_t sjson::get_int(const char* keys[]) {
    // ��ȡ��Ӧ��ֵ��ת��Ϊ�з�������
    return yyjson_get_sint(get_value(keys));
}

// �жϵ�ǰJSON�ĵ��Ƿ�ɹ���
bool sjson::is_opened() {
    // �����ǰ�ĵ�ָ��Ϊ�գ�˵��δ�ɹ��򿪣�����false�����򷵻�true
    return current_doc == NULL ? false : true;
}

// ��ȡ���Ƕ�׵�JSON����ֵ
bool sjson::get_bool(const char* keys[]) {
    // ��ȡ��Ӧ��ֵ��ת��Ϊ����ֵ
    return yyjson_get_bool(get_value(keys));
}

// ��ȡ���Ƕ�׵�JSONʵ��ֵ
double sjson::get_real(const char* keys[]) {
    // ��ȡ��Ӧ��ֵ��ת��Ϊʵ��
    return yyjson_get_real(get_value(keys));
}

// ��ȡ��ǰ��JSON�ĵ�ָ��
yyjson_doc* sjson::get_current_doc() {
    return current_doc;
}

// ��ȡ��ǰJSON�ĵ��ĸ��ڵ�ָ��
yyjson_val* sjson::get_current_root() {
    return current_doc_root;
}

// ��ȡ���Ƕ�׵�JSONֵ������
const char* sjson::get_type(const char* keys[]) {
    // �Ȼ�ȡ��Ӧ��ֵ
    yyjson_val* value = get_value(keys);
    if (value) {
        // ��ȡ����������
        return yyjson_get_type_desc(value);
    }
    else {
        // �����ȡʧ�ܣ����������Ϣ
        std::cerr << "Failed to get type" << std::endl;
        return NULL;
    }
}

// �����������ͷŵ�ǰ��JSON�ĵ��ڴ�
sjson::~sjson() { yyjson_doc_free(current_doc); }