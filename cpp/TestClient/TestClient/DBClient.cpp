#include "DBClient.h"

DBClient::DBClient(std::string address, int port)
: TcpClient(address, port){

}

DBClient::~DBClient() {
}

void DBClient::connectInternal(DBBD::TcpSession::pointer session)
{
	playerSession = std::make_shared<PlayerSession>(session);
}

void DBClient::closeInternal() {

}