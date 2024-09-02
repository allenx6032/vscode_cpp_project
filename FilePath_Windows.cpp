
#include<iostream>
#include<string>
#include<io.h>
using namespace std;
/*************************************************
Function: file_search
Description: 遍历文件夹下所有的文件，包含子文件夹下的文件
Input1: string path：遍历文件夹的绝对路径
Input2: int layer：打印时前放填补空格数
Output: void
Return: void
Others: // 其它说明
*************************************************/
void file_search(string path,int layer)
{
	//struct _finddata_t 是用来存储文件各种信息的结构体
	struct _finddata_t filefind;
	
	//当前文件夹
	string curr=path+"\\*.*";
	int done=0,i,handle;
 
	//文件句柄初始化，找到当前文件夹 filefind.name equ "."
	if((handle=_findfirst(curr.c_str(),&filefind))==-1)return;
 
	//遍历当前文件夹下所有文件
	while(!(done=_findnext(handle,&filefind)))
	{
		//不检索父文件夹？（是否正确的解释）
		if(!strcmp(filefind.name,".."))continue;
		//格式对齐用
		for(i=0;i<layer;i++) cout<<"  ";
		//递归遍历
		if ((_A_SUBDIR==filefind.attrib))
		{      
			cout<<filefind.name<<"(dir)"<<endl;
			curr=path+"\\"+filefind.name;
				file_search(curr,layer+1);
		}
		else 
		{
			//打印文件名
			cout<<filefind.name<<endl;
		}
	} 
	//关闭句柄
	_findclose(handle);      
}
int main()
{    
	string path = "D:\\";
	// cout<<"请输入目录"<<endl;
	//输入需要遍历的路径
	// cin>>path;
	file_search(path,0);
	system("PAUSE");
	return 0;
}