#include <Windows.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include "UIResponser.h"

UIResponser::UIResponser()
{

}

UIResponser::~UIResponser()
{

}

BOOL UIResponser::Event(UIEvent* event)
{
	return FALSE;
}

UIResponser* UIResponser::GetNextResponser() const
{
	return NULL;
}