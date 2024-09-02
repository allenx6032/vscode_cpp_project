#include<stdio.h>
#include<stdlib.h>
#include<io.h>

// struct _finddata_t
// {
//     unsigned attrib;//文件属性 _A_ARCH（存档）、_A_HIDDEN（隐藏）、_A_NORMAL（正常）、_A_RDONLY（只 读）、_A_SUBDIR（文件夹）、_A_SYSTEM（系统）
//     time_t time_create;//文件创建时间
//     time_t time_access;//文件最后一次被访问的时间。
//     time_t time_write;//文件最后一次被修改的时间
//     _fsize_t size;//文件的大小
//     char name[_MAX_FNAME];
// };

const char* to_search="D:\\nhNHWJ\\Nanhewujie\\*.log"; //欲查找的文件，支持通配符

int main()
{
    long handle; //用于查找的句柄
    struct _finddata_t fileinfo; //文件信息的结构体
    handle =_findfirst(to_search,&fileinfo); //第一次查找
    if(-1==handle)
        return -1;
    printf("%s\n",fileinfo.name);
    while(!_findnext(handle,&fileinfo)) //循环查找其他符合的文件，知道找不到其他的为止
    {
        printf("%s\n",fileinfo.name);
    }
    _findclose(handle);
    system("pause");
    return 0;
}