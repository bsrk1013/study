#pragma once
#include "DBBD/TcpSession.h"
#include "DBBD/TimerObject.h"
#include "DBBD/Define.h"

class PlayerClientSession 
	: public DBBD::TimerObject
	, public DBBD::ITcpSession
{
public:
	PlayerClientSession();
	virtual ~PlayerClientSession();

public:
	virtual void init(DBBD::IoContextSP context);
	virtual void dispose();
	virtual void reset();

public:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void init(DBBD::TcpSession::pointer) override;
	virtual void send(DBBD::Cell*) override;

protected:
	// ITcpSession��(��) ���� ��ӵ�public DBBD::TimerObject
	virtual void bindReadInternal(DBBD::ReadInternalParam&) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;
	// TimerObject��(��) ���� ��ӵ�
	virtual void registTimerEvent() override;

private:
	DBBD::TcpSession::pointer session;
};

