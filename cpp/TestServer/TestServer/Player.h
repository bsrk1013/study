#pragma once
#include "DBBD\TcpSession.h"

class Player
{
public:
	Player(DBBD::TcpSession::pointer session);
	~Player();

private:
	DBBD::TcpSession::pointer session;
};