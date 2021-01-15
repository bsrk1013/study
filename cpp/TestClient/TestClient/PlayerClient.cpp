#include "PlayerClient.h"
#include "BaseProtocol.hpp"
//#include "DBBD/Cell.h"

PlayerClient::PlayerClient(const std::string& address, const short& port)
	: TcpClientBase(address, port)
{
}

PlayerClient::~PlayerClient()
{
}

void PlayerClient::startInternal()
{
}

void PlayerClient::stopInternal()
{
}

void PlayerClient::readInternal(DBBD::Buffer buffer)
{
	auto headerBlock = buffer.viewByteBlock(DBBD::HeaderSize);
	DBBD::Header header(headerBlock);

	switch (header.typeId) {
	case ProtocolType::Value::PingCheckReq:
		break;
	}
}
