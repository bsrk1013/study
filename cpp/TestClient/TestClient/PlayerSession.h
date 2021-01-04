#pragma once
#include "DBBD/TcpSession.h"
#include "DBBD/Define.h"

class PlayerSession :
	DBBD::ITcpSession
{
public:
	PlayerSession(DBBD::TcpSession::pointer session);
	~PlayerSession();
public:
	// ITcpSession을(를) 통해 상속됨
	virtual void send(DBBD::Cell*) override;
protected:
	// ITcpSession을(를) 통해 상속됨
	virtual void bindReadInternal(DBBD::ReadInternalParam& dest) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	bool isDisposed = false;
	DBBD::TcpSession::pointer session;
};

