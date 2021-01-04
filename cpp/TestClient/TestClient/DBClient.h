#pragma once
#include "DBBD/TcpClient.h"
#include "PlayerSession.h"

class DBClient : public DBBD::TcpClient
{
public:
	DBClient(std::string address, int port);
	virtual ~DBClient();

protected:
	// TcpClient을(를) 통해 상속됨
	virtual void connectInternal(DBBD::TcpSession::pointer session) override;

private:
	std::shared_ptr<PlayerSession> playerSession;
};

