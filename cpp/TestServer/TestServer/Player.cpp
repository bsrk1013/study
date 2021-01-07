#include <iostream>
#include "Player.h"
#include "DBBD/Common.hpp"
#include "BaseProtocol.hpp"

Player::Player(DBBD::TcpSession::pointer session)
	: TimerObject(session->getContext()), session(session) {
	bindReadInternal(this->session->readInternal);
}

Player::~Player() {
	isDisposed = true;
	//removeTimerEvent(1);
}

void Player::send(DBBD::Cell* data) {
	session->write(data);
}

void Player::bindReadInternal(DBBD::ReadInternalParam& dest) {
	dest = READ_INTERNAL_BINDING(&Player::readInternal);
}

bool Player::readInternal(const DBBD::Header& header, DBBD::Buffer& buffer)
{
	if (isDisposed) {
		return false;
	}

	/////////////////////////////////////////////////////////////////////// readInternal에서 구현해야함
	switch (header.typeId) {
	case ProtocolType::PingCheckResp: {
		PingCheckResp resp;
		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&resp);
		break;
	}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	return true;
}

void Player::registTimerEvent() {
	addTimerEvent(1, TIMER_BINDING(&Player::update), 1000, true);
	//addTimerEvent(2, TIMER_BINDING(&Player::pingCheck), 1000, true);
}

void Player::update() {
	std::cout << "Player::update" << std::endl;
	size_t sessionId = this->session->getSessionId();
}

void Player::pingCheck() {
	std::cout << "Player::pingCheck" << std::endl;
	PingCheckReq req;
	send((DBBD::Cell*)&req);
}