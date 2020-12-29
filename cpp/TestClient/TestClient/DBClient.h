#pragma once
#include "DBBD/TcpClient.h"

class DBClient : public DBBD::TcpClient
{
public:
	DBClient(std::string address, int port);
	virtual ~DBClient();

protected:
	// TcpClient��(��) ���� ��ӵ�
	virtual void connectInternal(DBBD::TcpSession::pointer session) override;
};

