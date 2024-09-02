
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