#pragma once
#include "DBBD/TcpSession.h"

class CommunityClientSession : public DBBD::ITcpSession
{
public:
	CommunityClientSession(DBBD::TcpServer* server);
	~CommunityClientSession();

public:
	// ITcpSession을(를) 통해 상속됨
	virtual void init(DBBD::TcpSession::pointer session) override;
	virtual void send(DBBD::Cell*) override;

protected:
	// ITcpSession을(를) 통해 상속됨
	virtual void bindReadInternal(DBBD::ReadInternalParam&) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	DBBD::TcpServer* server = nullptr;
	DBBD::TcpSession::pointer session;
};