#include "CommunityClientSession.h"

CommunityClientSession::CommunityClientSession(DBBD::TcpServer* server) 
{
	this->server = server;
}
CommunityClientSession::~CommunityClientSession() 
{
	server = nullptr;
}

void CommunityClientSession::init(DBBD::TcpSession::pointer session)
{
	this->session = session;
	bindReadInternal(this->session->readInternal);
}

void CommunityClientSession::send(DBBD::Cell* data)
{
	session->write(data);
}

void CommunityClientSession::bindReadInternal(DBBD::ReadInternalParam& dest)
{
	dest = READ_INTERNAL_BINDING(&CommunityClientSession::readInternal);
}

bool CommunityClientSession::readInternal(const DBBD::Header&, DBBD::Buffer&)
{
	return true;
}
