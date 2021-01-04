#pragma once
#include "DBBD/TcpClient.h"
#include "PlayerSession.h"

class DBClient : public DBBD::TcpClient
{
public:
	DBClient(std::string address, int port);
	virtual ~DBClient();

protected:
	// TcpClient��(��) ���� ��ӵ�
	virtual void connectInternal(DBBD::TcpSession::pointer session) override;

private:
	std::shared_ptr<PlayerSession> playerSession;
};

