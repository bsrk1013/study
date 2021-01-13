#pragma once
#include "DBBD/TcpSession.h"

class BaseClientSession : public DBBD::ITcpSession
{
public:
	BaseClientSession();
	~BaseClientSession();

public:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void init(DBBD::TcpSession::pointer session) override;  
	virtual void send(DBBD::Cell*) override;

protected:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void bindReadInternal(DBBD::ReadInternalParam&) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	DBBD::TcpSession::pointer session;
};