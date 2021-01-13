#pragma once
#include <map>
#include "DBBD/TcpServer.h"
#include "BaseClientSession.h"

class CommunityServer : public DBBD::TcpServer
{
public:
	CommunityServer(std::string name, std::string address, int port);
	virtual ~CommunityServer();

	// TcpServer을(를) 통해 상속됨
	virtual void acceptInternal(DBBD::TcpSession::pointer session) override;
	virtual void disconnectInternal(size_t sessionId) override;

private:
	std::map<size_t, std::shared_ptr<BaseClientSession>> sessionMap;
};