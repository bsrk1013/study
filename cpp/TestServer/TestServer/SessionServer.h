#pragma once
#include <boost/asio.hpp>
#include <string>
#include <map>
#include "DBBD/TcpServer.h"
#include "DBBD/ObjectPoolManager.h"

using namespace boost::asio;

class Player;
class DBBD::TcpSession;
class SessionServer : public DBBD::TcpServer {
public:
	SessionServer(std::string name);
	SessionServer(std::string name, short port);
	SessionServer(std::string name, std::string address);
	SessionServer(std::string name, std::string address, short port);
	virtual ~SessionServer();
protected:
	virtual void acceptInternal(DBBD::TcpSession::pointer session) override;
	virtual void disconnectInternal(size_t sessionId) override;
	virtual void closeInternal() override;

private:
	DBBD::ObjectSPPoolManager<Player> playerSPPool;
	std::map<size_t, std::shared_ptr<Player>> playerMap;
};