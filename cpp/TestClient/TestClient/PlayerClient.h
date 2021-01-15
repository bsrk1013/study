#pragma once
#include "DBBD/TcpClientBase.h"

class PlayerClient : public DBBD::TcpClientBase
{
public:
	PlayerClient(const std::string& address, const short& port);
	virtual ~PlayerClient();

private:
	// TcpClientBase을(를) 통해 상속됨
	virtual void startInternal() override;
	virtual void stopInternal() override;
	virtual void readInternal(DBBD::Buffer) override;
};