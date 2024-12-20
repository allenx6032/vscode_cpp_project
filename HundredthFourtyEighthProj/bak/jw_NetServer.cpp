#include "jw_NetServer.h"
#include "jw_NetConnect.h"
#include "jw_NetEvent.h"

NetServer::NetServer(EventLoop * loop, NetEvent * nevent) :
	TcpServer(loop),
	_netevent(nevent)
{
}

TcpSocket * NetServer::createSocket()
{
	NetConnect * conn = _netevent->createConnect();
	return (TcpSocket *)conn;
}

void NetServer::onSocket(TcpSocket * connect)
{
	_netevent->onAccept((NetConnect *)connect);
}