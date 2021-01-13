#pragma once
#include "DBBD/TcpSession.h"

class CommunityClientSession : public DBBD::ITcpSession
{
public:
	CommunityClientSession(DBBD::TcpServer* server);
	~CommunityClientSession();

public:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void init(DBBD::TcpSession::pointer session) override;
	virtual void send(DBBD::Cell*) override;

protected:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void bindReadInternal(DBBD::ReadInternalParam&) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	DBBD::TcpServer* server = nullptr;
	DBBD::TcpSession::pointer session;
};