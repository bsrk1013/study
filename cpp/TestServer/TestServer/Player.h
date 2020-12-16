#pragma once
#include "DBBD/TcpSession.h"

class Player
{
public:
	Player(DBBD::TcpSession::pointer session);
	~Player();

protected:

private:
	DBBD::TcpSession::pointer session;
};