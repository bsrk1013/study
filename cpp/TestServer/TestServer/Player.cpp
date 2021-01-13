#include <iostream>
#include "Player.h"
#include "BaseProtocol.hpp"

Player::Player(){
}

Player::~Player() {
	dispose();
}

void Player::init(DBBD::TcpSession::pointer session) {
	this->session = session;
	bindReadInternal(this->session->readInternal);
	init(session->getContext());
}

void Player::init(DBBD::IoContextSP context) {
	TimerObject::init(context);
}

void Player::dispose() {
	if (isDisposed) { return; }
	TimerObject::dispose();
	session.reset();
}

void Player::reset() {
	if (!isDisposed) { return; }
	TimerObject::reset();
	session.reset();
}

void Player::send(DBBD::Cell* data) {
	session->write(data);
}

void Player::bindReadInternal(DBBD::ReadInternalParam& dest) {
	dest = READ_INTERNAL_BINDING(&Player::readInternal);
}

bool Player::readInternal(const DBBD::Header& header, DBBD::Buffer& buffer)
{
	if (isDisposed) { return false; }

	/////////////////////////////////////////////////////////////////////// readInternal에서 구현해야함
	switch ((ProtocolType::Value)header.typeId) {
	case ProtocolType::Value::PingCheckResp: {
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
	addTimerEvent(2, TIMER_BINDING(&Player::pingCheck), 1000, true);
}

void Player::update() {
	size_t sessionId = this->session->getSessionId();
}

void Player::pingCheck() {
	PingCheckReq req;
	send((DBBD::Cell*)&req);
}