#include <stdio.h>
#include <io.h>
#include <string>
#include <fstream>
#include <vector> 
#include <iostream>

using namespace std;
//声明替换文件中的字符串
int ReplaceData(string filepath, const string &pattern, string targetStr);
//声明分割字符串函数
vector<string> split(const string &str,const string &pattern);

int main()
{
    //目标文件夹路径
    string inPath = "D:\\*";//遍历文件夹下的所有文件
    //用于查找的句柄
    long handle;
    struct _finddata_t fileinfo;
    //第一次查找
    handle = _findfirst(inPath.c_str(),&fileinfo);
    if(handle == -1)
        return -1;
    do
    {
        //找到的非.flg文件的文件名
        if(NULL == strstr(fileinfo.name, ".flg"))
        {
        	cout << fileinfo.name << endl;
		}
       
    } while (!_findnext(handle,&fileinfo));

	handle = _findfirst(inPath.c_str(),&fileinfo); 
	do
    {
        //找到的.flg文件的文件名
        if(NULL != strstr(fileinfo.name, ".flg"))
        {
        	//打开.flg文件，替换其中的校验码
			const string s1("|");
        	string s2("1231425536");
        	int ret = ReplaceData(fileinfo.name, s1, s2);
        	if(ret != 0)
        	{
        		cout <<"error open" << fileinfo.name << endl;
			}
        	printf("%s\n", fileinfo.name);
		}
       
    } while (!_findnext(handle,&fileinfo));

    _findclose(handle);
    system("pause");
    return 0;
}
int ReplaceData(string filepath, const string &pattern, string targetStr)
{
	string::size_type pos = 0;
	vector<string> stringTag;
	ifstream instream ;
	ofstream outstream;
	string str;
	instream.open(filepath, ios::in);
	if (!instream)
	{
		cout << "error" << endl;
		return -1;
	}
	getline(instream, str);
	cout << 1 << str <<endl;
	while (getline(instream, str))
	{
		cout<<str<<endl;
		stringTag = split(str,pattern);
		if(stringTag.size() >= 5)
		{
			stringTag[4] = targetStr;
		}
	}
	instream.clear();
	instream.close();
	
	outstream.open(filepath, ios::out | ios::binary);
	vector<string>::iterator iter;
	for (iter = stringTag.begin(); iter < stringTag.end(); iter++)
	{
	  outstream << (*iter) << pattern << endl;
	}
	stringTag.clear();
	outstream.close();
	return 0;
}

vector<string> split(const string &str,const string &pattern)
{
    //const char* convert to char*
    char * strc = new char[strlen(str.c_str())+1];
    strcpy(strc, str.c_str());
    vector<string> resultVec;
    char* tmpStr = strtok(strc, pattern.c_str());
    while (tmpStr != NULL)
    {
        resultVec.push_back(string(tmpStr));
        tmpStr = strtok(NULL, pattern.c_str());
    }

    delete[] strc;

    return resultVec;
}