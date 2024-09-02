// luaband.cpp : 定义控制台应用程序的入口点。
//


#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <intsafe.h>
#include "lua.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <tchar.h>
#include "LuaVarint.h"
#include "iLuaAcitveScriptImpl.h"
#include "ScriptLibStorage.h"
#include "lua.hpp"
#include "LuaEngineMgr.h"

using namespace std;
using namespace binding::lua;

lua_State *L;

int getBuffer(LPCSTR lpFilePath, std::vector<char>& vecBuff) {
	std::fstream file;
	file.open(lpFilePath, std::ios::in|std::ios::binary|std::ios::_Nocreate);
	if(file.fail())
	{
		file.close();
		return 1;
	}

	file.seekg(0, std::ios::end);
	std::ios::pos_type ps = file.tellg();
	unsigned int uSize = (unsigned int)ps;

	file.seekg(0, std::ios::beg);
	vecBuff.resize(uSize,0);
	file.read(&vecBuff[0],uSize);
	file.close();
	return 0;
}

int LuaManager() {
	std::vector<char> vecBuff;
	getBuffer(("test.lua"), vecBuff);
	bool bRet = CLuaEngineMgr::GetInstance()->Excute(vecBuff, NULL, _T("0000"), _T("Main"));
	return bRet;
}


int LuaEngineTest()
{
	DISP_PARAMS rdp;
	CLuaVariant rvarRes;
	CLuaAcitveScriptImpl* lEngineTest = CLuaAcitveScriptImpl::GetInstance();
	CScriptLibStorage* lpLuaStorge = new CScriptLibStorage;
	lEngineTest->SetStorage(lpLuaStorge);
	lEngineTest->Open();
	lEngineTest->Load(_T("test.lua"));
	lEngineTest->Invoke(_T("add"), rdp, rvarRes);

	int i = rvarRes.getValue<int>();

	lEngineTest->Release();

	return 0;
}


int _tmain(int argc, _TCHAR* argv[])
{
	LuaManager();
	//system("pause");
	return 0;
}

