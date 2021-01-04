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
	// ITcpSession��(��) ���� ��ӵ�
	virtual void send(DBBD::Cell*) override;
protected:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void bindReadInternal(DBBD::ReadInternalParam& dest) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	bool isDisposed = false;
	DBBD::TcpSession::pointer session;
};

