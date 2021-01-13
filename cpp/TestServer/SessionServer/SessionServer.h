#pragma once
#include <map>
#include "DBBD/TcpServer.h"
#include "CommunityClient.h"

class PlayerClientSession;
class DBBD::TcpSession;
class SessionServer : public DBBD::TcpServer
{
public:
	SessionServer(std::string name, std::string address, int port);
	virtual ~SessionServer();

protected:
	// TcpServer을(를) 통해 상속됨
	virtual void acceptInternal(DBBD::TcpSession::pointer session) override;
	virtual void disconnectInternal(size_t sessionId) override;
	virtual void closeInternal() override;

private:
	std::map<size_t, std::shared_ptr<PlayerClientSession>> sessionMap;
	std::shared_ptr<CommunityClient> communityClient;
};
