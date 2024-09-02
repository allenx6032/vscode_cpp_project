#include <iostream>
#include <string>
#include <windows.h>

// FILE_ATTRIBUTE_READONLY 1 只读
// FILE_ATTRIBUTE_HIDDEN 2 隐藏
// FILE_ATTRIBUTE_SYSTEM 4 系统
// FILE_ATTRIBUTE_DIRECTORY 16 目录
// FILE_ATTRIBUTE_ARCHIVE 32 存档
// FILE_ATTRIBUTE_DEVICE 64 保留
// FILE_ATTRIBUTE_NORMAL 128 正常
// FILE_ATTRIBUTE_TEMPORARY 256 临时
// FILE_ATTRIBUTE_SPARSE_FILE 512 稀疏文件
// FILE_ATTRIBUTE_REPARSE_POINT 1024 超链接或快捷方式
// FILE_ATTRIBUTE_COMPRESSED 2048 压缩
// FILE_ATTRIBUTE_OFFLINE 4096 脱机
// FILE_ATTRIBUTE_NOT_CONTENT_INDEXED 8192 索引
// FILE_ATTRIBUTE_ENCRYPTED 16384 加密
// FILE_ATTRIBUTE_VIRTUAL 65536 虚拟

int find_path(const char *);

char save_path[MAX_PATH] = { 0 };//结果输出路径;
char szPath[MAX_PATH] = { 0 };
char result[MAX_PATH] = { 0 };
FILE* pf_path_file;int find_path(const char *);

int find_path_save_file(const char* lp_path, const char* out_file_name)
{
    pf_path_file = fopen(out_file_name, "w");
    int cnt = find_path(lp_path);
    fclose(pf_path_file);
    return cnt;
}

void str_tok(char *str) //分离文件目录
{
    char *p = NULL;
    char delims[] = "\\";
    p = strtok(str, delims);
    while (p != NULL)
    {
        strcpy(result, p);
        p = strtok(NULL, delims);
    };
}

void str_extension(char *str) //分离文件扩展名;
{
    char *p = NULL;
    char delims[] = ".";
    p = strtok(str, delims);
    while (p != NULL)
    {
        strcpy(result, p);
        p = strtok(NULL, delims);
    };
}

int find_path(const char* lp_path)// 遍历搜索目录
{
    static int cnt = 0;
    HANDLE hFile;

    WIN32_FIND_DATAA wfd; //数据结构;
    char sz_path[MAX_PATH] = { 0 };
    char buf[MAX_PATH * 2] = { 0 };
    char fileName[MAX_PATH] = { 0 };
    char temp[MAX_PATH] = { 0 };//临时数组;
    char parseFileName[MAX_PATH] = { 0 };

    strcpy(sz_path, lp_path);
    strcat(sz_path, "\\*.*"); //匹配任何文件包括文件夹目录;
    hFile = FindFirstFile(sz_path, &wfd);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        do
        {
            if (wfd.cFileName[0] == '.') //如果是.或..则过滤; 
                continue;
            else if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) //如果是目录则递归;
            {
                strcpy(sz_path, lp_path);
                strcat(sz_path, "\\");
                if (!strcmp(wfd.cFileName, "test1"))
                {
                    fprintf(pf_path_file, "%s:%s\n", lp_path, wfd.cFileName);
                }
                if (!strcmp(wfd.cFileName, "test2"))
                {
                    fprintf(pf_path_file, "%s:%s\n", lp_path, wfd.cFileName);
                }
                strcat(sz_path, wfd.cFileName);    //拼接目录全路径名;

                find_path(sz_path);               //调用递归;
            }
        } while (FindNextFile(hFile, &wfd));
    }

    return cnt;
}

int main(int argc, CHAR* argv[])
{
    GetCurrentDirectory(MAX_PATH, szPath); //当前目录
    strcpy(save_path, szPath);//结果result.txt放在当前目录中;
    strcat(save_path, "\\result.txt");
    find_path_save_file("C:\\Users\\Administrator", save_path);

    system("pause");

    return 0;
}