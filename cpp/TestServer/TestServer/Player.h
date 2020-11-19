#pragma once
#include "DBBD\TcpSession.h"

class Player
{
public:
	Player(DBBD::TcpSession::pointer session);
	virtual ~Player();

public:

private:
	DBBD::TcpSession::pointer session;
};