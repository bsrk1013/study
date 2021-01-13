#pragma once
#include "DBBD/TcpClient.h"

class CommunityClient : public DBBD::TcpClient
{
	// TcpClient을(를) 통해 상속됨
	virtual void connectInternal(DBBD::TcpSession::pointer session) override;
	virtual void closeInternal() override;
};