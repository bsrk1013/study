#pragma once
#include "DBBD/TcpClient.h"
#include "CommunityClientSession.h"

class CommunityClient : public DBBD::TcpClient
{
public:
	CommunityClient(std::string address, int port, DBBD::TcpServer* server);
	virtual ~CommunityClient();

protected:
	// TcpClient을(를) 통해 상속됨
	virtual void connectInternal(DBBD::TcpSession::pointer session) override;
	virtual void closeInternal() override;

private:
	DBBD::TcpServer* server;
	std::shared_ptr<CommunityClientSession> session;
};