#pragma once
#include"conversion_utf8_gbk.h"
#include"json.hpp"
#include<algorithm>
#include<fstream>
#include<iostream>
#include<random>
#include<string>
#include<vector>
#include<Windows.h>
class _words_ {
public:
	std::string word;
	std::string sentence;
	std::string chinese;
	int error_number_of_dictation;
	inline bool operator < (const _words_& x)const {
		return error_number_of_dictation > x.error_number_of_dictation;
	}
};
void remove_endofline_spaces(string& s)
{
	while (s.size() && s.back() == ' ')s.pop_back();
}
std::string tolowercase(std::string s)
{
	for (int i = 0; i < s.size(); i++)s[i] = tolower(s[i]);
	return s;
}
void hcz_make_new_file(std::string s, std::string& file)
{
	std::ifstream fin(s);
	if (fin.good())std::cout << "已经存在该文件，请重试\n";
	else
	{
		std::ofstream fout(s);
		if (fout.is_open())
		{
			std::cout << "创建成功\n";
			file = s;
			fout.close();
		}
		else
		{
			std::cout << "无法创建\n";
		}
	}
	fin.close();
	Sleep(2000);
}
void hcz_1(std::string& file)
{
	std::cout << "请输入需要创建存档完整路径，包含文件名及后缀(路径请勿包含中文)，示例C:\\cundang\\hcz.json ,请先手动建好子文件夹，否则可能因权限不足导致创建失败\n";
	std::string s;
	std::cin >> s;
	hcz_make_new_file(s, file);
}
nlohmann::json hcz_2(std::string& file)
{
	nlohmann::json js;
	std::cout << "请输入需要创建存档完整路径，包含文件名及后缀(路径请勿包含中文)，示例C:\\cundang\\hcz.json ,请先手动建好子文件夹，否则可能因权限不足导致创建失败\n";
	std::string s;
	std::cin >> s;
	std::ifstream fin(s);
	if (fin.is_open())
	{
		fin >> js;
		file = s;
		std::cout << "读取成功\n";
	}
	else
	{
		std::cout << "读取失败\n";
	}
	fin.close();
	Sleep(2000);
	return js;
}
void hcz_5(nlohmann::json& js)
{
	if (js.empty())
	{
		std::cout << "未读取存档存档\n";
	}
	else
	{
		std::cout << "由于windows奇怪的中文编码问题让作者怀疑人生，故暂不支持控制台输入中文，请新建txt，并把完整路径输入,路径不要包括中文\n";
		std::cout << "请输入单词后换行，输入中文意思后换行，输入例句后换行\n";
		std::cout << "每三行为一组，各组之间不用加额外符号，结束后在最后一行写‘the end’即可\n";
		std::string file;
		std::cin >> file;
		std::ifstream fin(file);
		if (fin.is_open())
		{
			std::string word, chinese, sentence;
			nlohmann::json object;
			while (1)
			{
				getline(fin, word);
				remove_endofline_spaces(word);
				if (word == "the end")break;
				getline(fin, chinese);
				getline(fin, sentence);
				object.clear();
				object["word"] = word;
				object["chinese"] = chinese;
				object["sentence"] = sentence;
				object["error_number_of_dictation"] = 0;
				js[word] = object;
			}
			std::cout << "录入成功\n";
			fin.close();
		}
		else
		{
			std::cout << "路径错误\n";
		}
	}
	Sleep(2000);
}
void refresh(std::string file, nlohmann::json js)
{
	std::ofstream fout(file);
	if (fout.is_open())
	{
		fout << js.dump();
		fout.close();
		std::cout << "保存成功\n";
	}
	else
	{
		std::cout << "保存失败，请重新设置路径\n";
	}
	Sleep(2000);
}
std::vector<_words_>js_vector(nlohmann::json& js)
{
	std::vector< _words_>res;
	for (auto& x : js.items())
	{
		if (x.key() == "is_empty")continue;
		res.push_back({ js[x.key()]["word"].get<std::string>(),js[x.key()]["sentence"].get<std::string>() ,js[x.key()]["chinese"].get<std::string>() ,js[x.key()]["error_number_of_dictation"].get<int>() });
	}
	return res;
}
nlohmann::json vector_js(std::vector<_words_>& words)
{
	nlohmann::json js;
	js["is_empty"] = 0;
	for (_words_& word : words)
	{
		js[word.word]["chinese"] = word.chinese;
		js[word.word]["error_number_of_dictation"] = word.error_number_of_dictation;
		js[word.word]["sentence"] = word.sentence;
		js[word.word]["word"] = word.word;
	}
	return js;
}
void outputword(std::string s, std::string ss)
{
	for (int i = 0; i < s.size(); i++)
	{
		if (tolowercase(s.substr(i, ss.size())) == tolowercase(ss))
		{
			for (int j = i; j < i + ss.size(); j++)
			{
				s[j] = '_';
			}
			i = i + ss.size() - 1;
		}
	}
	std::cout << s << '\n';
}
void hcz_3(std::vector<_words_>& words)
{
	std::cout << "请根据中文意思写入单词或短语,输入esc以退出\n";
	if (!words.size())
	{
		std::cout << "存档中无单词\n";
		Sleep(2000);
		return;
	}
	std::mt19937 rnd(time(0));
	std::string s;
	getline(std::cin, s);
	while (1)
	{
		_words_& word = words[rnd() % words.size()];
		std::cout << word.chinese << '\n';
		outputword(word.sentence, word.word);
		getline(std::cin, s);
		if (s == "esc")return;
		if (s == word.word)
		{
			std::cout << "正确\n";
		}
		else
		{
			std::cout << "错误\n";
			std::cout << "正确写法" << word.word << '\n';
			word.error_number_of_dictation++;
		}
	}
}
void hcz_4(std::vector<_words_>& words)
{
	std::cout << "请根据中文意思写入单词或短语,输入esc以退出\n";
	if (!words.size())
	{
		std::cout << "存档中无单词\n";
		Sleep(2000);
		return;
	}
	stable_sort(words.begin(), words.end());
	std::string s;
	getline(std::cin, s);
	remove_endofline_spaces(s);
	for (int i = 0; i < words.size(); i++)
	{
		_words_& word = words[i];
		std::cout << word.chinese << '\n';
		outputword(word.sentence, word.word);
		getline(std::cin, s);
		remove_endofline_spaces(s);
		if (s == "esc")return;
		if (s == word.word)
		{
			std::cout << "正确\n";
		}
		else
		{
			std::cout << "错误\n";
			std::cout << "正确写法为 " << word.word << '\n';
			word.error_number_of_dictation++;
		}
	}
	std::cout << "全部完成了\n";
	Sleep(2000);
}
void hcz_7(nlohmann::json js)
{
	std::cout << "请输入需要查询的单词，然后按下回车,输入esc以退出\n";
	std::string s;
	getline(std::cin, s);
	while (1)
	{
		getline(std::cin, s);
		if (s == "esc")return;
		if (js.find(s) != js.end())
		{
			std::cout << js[s]["word"].get<std::string>() << '\n' << js[s]["sentence"].get<std::string>() << '\n' << js[s]["chinese"].get<std::string>() << '\n';
		}
		else
		{
			std::cout << "存档中无此单词\n";
		}
	}
}
void hcz_start_word_manager()
{
	nlohmann::json js;
	std::string file;
	std::vector<_words_>word;
	while (1)
	{
		system("cls");
		std::cout << "1：创建新单词存档 2：读取已经存在的存档 3：随机默写 4:按错误次数排序默写  5：添加单词 6：保存存档 7：查询已存在单词" << std::endl;
		std::string s;
		std::cin >> s;
		if (s == "1")
		{
			hcz_1(file);
			js["is_empty"] = 0;
		}
		else if (s == "2")
		{
			js = hcz_2(file);
		}
		else if (s == "3")
		{
			word = js_vector(js);
			hcz_3(word);
			js = vector_js(word);
		}
		else if (s == "4")
		{
			word = js_vector(js);
			hcz_4(word);
			js = vector_js(word);
		}
		else if (s == "5")
		{
			hcz_5(js);
		}
		else if (s == "6")
		{
			refresh(file, js);
		}
		else if (s == "7")
		{
			hcz_7(js);
		}
		else if (s == "esc")
		{
			return;
		}
	}
}