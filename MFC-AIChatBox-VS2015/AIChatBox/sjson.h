#ifndef __SJSON_H__
#define __SJSON_H__

#include <iostream>
#include <vector>
#include <string>
// �����ⲿC���Ե�yyjson��
extern "C" {
#include "yyjson.h"  
}

// sjson�࣬�Ƕ�yyjson�Ļ�����װ��
class sjson {
public:
    // ���캯��������ͨ�ļ���ȡJSON����
    sjson(const char* filename);
    // ���캯������ԭʼJSON���ݹ��죬ͨ�� 'json::open(json::read(rawstr))' ��ʽʹ��
    sjson(yyjson_doc* json_doc);

    // ��ָ���ļ�����JSON�ļ�
    yyjson_doc* open(const char* filename);
    // ��̬��������ԭʼJSON�ַ�����ȡ������
    static yyjson_doc* read(const char* raw_json);
    // �жϵ�ǰJSON�ĵ��Ƿ�ɹ���
    bool is_opened();

	// ��ȡJSON�ļ��е�һ���ļ�
    std::vector<std::string> get_top_level_keys();
	// ��ȡJSON�ļ������еļ�
	std::vector<std::string> get_all_keys();
    // ��ȡ���Ƕ�׵�JSONֵ������ sjson::get_value({"id", "name"}) ��Ӧ file.json ['id']['name']
    yyjson_val* get_value(const char* keys[]);
	yyjson_val* get_value(std::string keys);
    // ��ȡ���Ƕ�׵�JSON�ַ���ֵ
    const char* get_str(const char* keys[]);
    // ��ȡ���Ƕ�׵�JSON����ֵ
    int64_t get_int(const char* keys[]);
    // ��ȡ���Ƕ�׵�JSON����ֵ
    bool get_bool(const char* keys[]);
    // ��ȡ���Ƕ�׵�JSONʵ��ֵ
    double get_real(const char* keys[]);

    // ��ȡ��ǰ��JSON�ĵ�ָ��
    yyjson_doc* get_current_doc();
    // ��ȡ��ǰJSON�ĵ��ĸ��ڵ�ָ��
    yyjson_val* get_current_root();
    // ��ȡ���Ƕ�׵�JSONֵ������
    const char* get_type(const char* keys[]);

    // ��������
    ~sjson();

private:
    // �洢JSON�ļ����ļ���
    std::string doc_filename;
    // ��ǰ��JSON�ĵ�ָ��
    yyjson_doc* current_doc = NULL;
    // ��ǰJSON�ĵ��ĸ��ڵ�ָ��
    yyjson_val* current_doc_root = NULL;

    // ��ȡJSON�ļ��ı�־������ע�ͺ�β�涺��
    yyjson_read_flag flg = YYJSON_READ_ALLOW_COMMENTS | YYJSON_READ_ALLOW_TRAILING_COMMAS;
    // ��ȡJSON�ļ�ʱ�Ĵ�����Ϣ
    yyjson_read_err err;
};

#endif