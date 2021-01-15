#pragma once
#include "DBBD/TcpServerBase.h"
#include <map>

class PlayerClientSession;
class SessionServer : public DBBD::TcpServerBase
{
public:
	SessionServer(std::string name, std::string address, int port, size_t threadCount);
	virtual ~SessionServer();

protected:
	// TcpServerBase을(를) 통해 상속됨
	virtual void stopInternal() override;
	virtual void acceptInternal(DBBD::SocketSP, size_t) override;
	virtual void disconnectInternal(size_t) override;

private:
	std::map<size_t, std::shared_ptr<PlayerClientSession>> sessionMap;
};