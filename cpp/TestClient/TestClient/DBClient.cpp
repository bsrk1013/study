#include "DBClient.h"

DBClient::DBClient(std::string address, int port)
: TcpClient(address, port){

}

DBClient::~DBClient() {
}

void DBClient::connectInternal(DBBD::TcpSession::pointer session)
{
}