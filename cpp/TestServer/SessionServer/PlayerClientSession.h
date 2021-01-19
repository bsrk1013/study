#pragma once
#include "DBBD/TcpSessionBase.h"
#include "DBBD/TimerObject.h"

class PlayerClientSession 
	: public DBBD::TcpSessionBase
	, public DBBD::TimerObject
{
public:
	// context, socket, bufferSize
	PlayerClientSession(DBBD::IoContextSP, DBBD::SocketSP, size_t);
	virtual ~PlayerClientSession();

public:
	// TcpSessionBase을(를) 통해 상속됨
	virtual void readInternal(DBBD::Header) override;
	virtual void writeInternal() override;

private:
	void update();

private:
	virtual void startInternal() override;
};