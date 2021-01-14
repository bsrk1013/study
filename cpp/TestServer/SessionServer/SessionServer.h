#pragma once
#include "DBBD/TcpServerBase.h"

class SessionServer : public DBBD::TcpServerBase
{
public:
	SessionServer(std::string name, std::string address, int port, size_t threadCount);
	virtual ~SessionServer();

protected:
	// TcpServerBase을(를) 통해 상속됨
	virtual void stopInternal() override;
	virtual void acceptInternal(DBBD::SocketSP, size_t) override;


};