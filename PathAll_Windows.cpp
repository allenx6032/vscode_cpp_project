#include<iostream>
#include<windows.h>
#include<io.h>
#include<vector>
void __GetFiles(std::string path,std::vector<std::string>& files )  
{  
    //文件句柄  
    long   hFile   =   0;  
    //文件信息  
    struct _finddata_t fileinfo;  
    std::string p;  
    if((hFile = _findfirst(p.assign(path).append("\\*").c_str(),&fileinfo)) !=  -1)  
    {  
        do  
        {  
            //如果是目录,迭代之  
            //如果不是,加入列表  
            if((fileinfo.attrib &  _A_SUBDIR))  
            {  
                if(strcmp(fileinfo.name,".") != 0  &&  strcmp(fileinfo.name,"..") != 0)  
                    __GetFiles( p.assign(path).append("\\").append(fileinfo.name), files );  
            }  
            else  
            {  
                files.push_back(p.assign(path).append("\\").append(fileinfo.name) );  
            }  
        }while(_findnext(hFile, &fileinfo)  == 0);  
        _findclose(hFile);  
    }  
}
std::vector<std::string> GetFiles(std::string path)
{
	std::vector<std::string>files;
	__GetFiles(path,files);
	return files;
}
using namespace std;
int main()
{
	string input_folder="D:\\";
	vector<string>input_files=GetFiles(input_folder);
	for(auto&it:input_files)
		cout<<it<<endl;
}