#include<io.h>
#include <iostream>
#include<string>
using namespace std;

void file_search(string path, int layer)
{
    //struct _finddata_t 是用来存储文件各种信息的结构体
    struct _finddata_t filefind;

    //当前文件夹
    string curr = path + "\\*.*";
    int done = 0, i, handle;

    //文件句柄初始化，找到当前文件夹 filefind.name equ "."
    if ((handle = _findfirst(curr.c_str(), &filefind)) == -1)return;

    //遍历当前文件夹下所有文件
    while (!(done = _findnext(handle, &filefind)))
    {
        //不检索父文件夹？（是否正确的解释）
        if (!strcmp(filefind.name, ".."))continue;

        //打印文件名
        cout << filefind.name << "this" << endl;

    }
    //关闭句柄
    _findclose(handle);
}


int main(){
	//遍历文件
    string path = "D:/VscodiumZone";
    file_search(path, 0);
	return 0;
}