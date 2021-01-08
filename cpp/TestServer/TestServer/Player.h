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
	// TimerObject을(를) 통해 상속됨
	virtual void init(DBBD::IoContextSP context);
	virtual void dispose();
	virtual void reset();

	// ITcpSession을(를) 통해 상속됨
	virtual void send(DBBD::Cell*) override;

protected:
	// ITcpSession을(를) 통해 상속됨
	virtual void bindReadInternal(DBBD::ReadInternalParam& dest) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;
	// TimerObject을(를) 통해 상속됨
	virtual void registTimerEvent() override;

private:
	void update();
	void pingCheck();

private:
	DBBD::TcpSession::pointer session;
	size_t count = 0;
};