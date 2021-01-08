#pragma once
#include "DBBD/TcpSession.h"
#include "DBBD/TimerObject.h"
#include "DBBD/Define.h"

class Player : 
	public DBBD::TimerObject,
	DBBD::ITcpSession
{
public:
	Player();
	virtual ~Player();

public:
	void init(DBBD::TcpSession::pointer session);
	// TimerObject��(��) ���� ��ӵ�
	virtual void init(DBBD::IoContextSP context);
	virtual void dispose();
	virtual void reset();

	// ITcpSession��(��) ���� ��ӵ�
	virtual void send(DBBD::Cell*) override;

protected:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void bindReadInternal(DBBD::ReadInternalParam& dest) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;
	// TimerObject��(��) ���� ��ӵ�
	virtual void registTimerEvent() override;

private:
	void update();
	void pingCheck();

private:
	DBBD::TcpSession::pointer session;
	size_t count = 0;
};