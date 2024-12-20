#include "jw_WebSocketEvent.h"
#include "jw_WebSocketConnect.h"


WebSocketEvent::WebSocketEvent()
{
}


WebSocketEvent::~WebSocketEvent()
{
}


WebSocketConnect * WebSocketEvent::createConnect()
{
	return new WebSocketConnect(this);
}

void WebSocketEvent::destroyConnect(WebSocketConnect * conn)
{
	if (conn)
		delete conn;
}