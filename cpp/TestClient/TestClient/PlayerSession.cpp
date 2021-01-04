#include "PlayerSession.h"
#include "BaseProtocol.hpp"
#include "DBBD/Define.h"

PlayerSession::PlayerSession(DBBD::TcpSession::pointer session) :
	session(session){
	bindReadInternal(this->session->readInternal);
}

PlayerSession::~PlayerSession() {
	isDisposed = true;
}

void PlayerSession::send(DBBD::Cell* data) {
	session->write(data);
}

void PlayerSession::bindReadInternal(DBBD::ReadInternalParam& dest)
{
	dest = READ_INTERNAL_BINDING(&PlayerSession::readInternal);
}

bool PlayerSession::readInternal(const DBBD::Header& header, DBBD::Buffer& buffer)
{
	if (isDisposed) {
		return false;
	}

	/////////////////////////////////////////////////////////////////////// readInternal에서 구현해야함
	switch (header.typeId) {
	case ProtocolType::PingCheckReq: {
		PingCheckReq req;
		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&req);
		PingCheckResp resp;
		send((DBBD::Cell*)&resp);
		break;
	}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	return true;
}
