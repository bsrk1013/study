#pragma once
#include <boost/asio.hpp>
#include <string>
#include <map>
#include "DBBD/TcpServer.h"

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
	virtual void implementAccept(DBBD::TcpSession::pointer session);

private:
	std::map<size_t, Player*> playerMap;
};