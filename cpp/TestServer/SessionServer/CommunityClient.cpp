#include "CommunityClient.h"

CommunityClient::CommunityClient(std::string address, int port, DBBD::TcpServer* server)
	: TcpClient(address, port)
{
	this->server = server;
}

CommunityClient::~CommunityClient() {}

void CommunityClient::connectInternal(DBBD::TcpSession::pointer session)
{
	this->session = std::make_shared<CommunityClientSession>(server);
	this->session->init(session);
}

void CommunityClient::closeInternal()
{
	server = nullptr;
}
