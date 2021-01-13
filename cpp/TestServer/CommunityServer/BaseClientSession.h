#pragma once
#include "DBBD/TcpSession.h"

class BaseClientSession : public DBBD::ITcpSession
{
public:
	BaseClientSession();
	~BaseClientSession();

public:
	// ITcpSession을(를) 통해 상속됨
	virtual void init(DBBD::TcpSession::pointer session) override;  
	virtual void send(DBBD::Cell*) override;

protected:
	// ITcpSession을(를) 통해 상속됨
	virtual void bindReadInternal(DBBD::ReadInternalParam&) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	DBBD::TcpSession::pointer session;
};