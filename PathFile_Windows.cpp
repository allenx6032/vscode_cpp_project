#include<Shlwapi.h>
#include<io.h>
#include<string>
#include<vector>
using namespace std;

#define FILE_FLODER 0X0001  //子文件夹
#define FILE_FILE     0X0002 //文件

int longGetPathArr(const string& ptStrPath,vector<string>& strPathArr,int type )
{
   if (ptStrPath.empty() || _access(ptStrPath.c_str(), 0) != 0)//判断路径是否存在
       return 0;
   if (!PathIsDirectory(ptStrPath.c_str()))//判断是否为文件夹
       return -1;
   strPathArr.clear();
   _finddata_t    fileInfo;
   string         strSearch = ptStrPath + "\\*";//搜索文件夹下所有文件
   long           handle  = 0;   
   handle =_findfirst(strSearch.c_str(), &fileInfo);
   if (-1 ==handle)
       return 0;
   while (0 ==_findnext(handle, &fileInfo))
   {
       if (stricmp(fileInfo.name,"..") == 0 || stricmp(fileInfo.name,".") == 0)//如果为上级目录或当前目录
           continue;
       if ((fileInfo.attrib&_A_SUBDIR) == _A_SUBDIR)//如果为文件夹
       {
           if(type == FILE_FLODER)
               strPathArr.push_back(ptStrPath + "\\" + fileInfo.name);
       }
       if ((fileInfo.attrib&_A_ARCH) == _A_ARCH)//如果为文档   
       {
           if(type == FILE_FILE)
               strPathArr.push_back(ptStrPath + "\\" + fileInfo.name);
       }
   }
   _findclose(handle);//关闭句柄
   return 1; 
}


int main()
{    

	//获取系统temp目录
	char  strTmpPath[MAX_PATH];
	GetTempPath(sizeof(strTmpPath), strTmpPath);
	printf("获取系统temp目录：%s\n",strTmpPath);

	// 获取自身的绝对路径
	char  strSelf[MAX_PATH];
	memset(strSelf, 0, sizeof(strSelf));
	GetModuleFileName(NULL, strSelf, sizeof(strSelf));
	printf("获取自身的绝对路径：%s\n", strSelf);

	//获取当前进程号
	printf("获取当前进程号：%d\n",GetCurrentThreadId());

	// 获取系统目录
	char strSysDir[100];
	GetSystemDirectory(strSysDir, sizeof(strSysDir));
	// char *bin = "%SystemRoot%\\System32\\svchost.exe -k netsvcs";
	printf("系统目录：%s\n",strSysDir);
	//采用fread、fwrite
	system("pause");
	
	
	vector<string>   files;    
	string path = "D:\\";
	// cout<<"请输入目录"<<endl;
	//输入需要遍历的路径
	// cin>>path;
	longGetPathArr(path,files,FILE_FLODER);
	system("PAUSE");
	return 0;
}