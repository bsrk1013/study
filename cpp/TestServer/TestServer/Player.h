#pragma once
#include "DBBD/TcpSession.h"

class Player : DBBD::ITcpSession
{
public:
	Player(DBBD::TcpSession::pointer session);
	~Player();

private:
	DBBD::TcpSession::pointer session;

	// ITcpSession을(를) 통해 상속됨
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;
};