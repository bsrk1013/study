#include <iostream>
#include "Player.h"
#include "DBBD/Common.hpp"
#include "BaseProtocol.hpp"

class ChattingReq : DBBD::Request {
public:
	ChattingReq() { typeId = 1; }
	virtual ~ChattingReq() {}

	// Request을(를) 통해 상속됨
	virtual void serialize(DBBD::Buffer& buffer)
	{
		writeHeader(buffer, getLength());
		DBBD::Serialize::write(buffer, msg);
	}
	virtual void deserialize(DBBD::Buffer& buffer)
	{
		readHeader(buffer);
		DBBD::Deserialize::read(buffer, msg);
	}

	virtual size_t getLength() {
		return Request::getLength() + DBBD::GetPacketLength(msg);
	}

public:
	std::string getMsg() { return msg; }
	void setMsg(std::string value) { msg = value; }

private:
	std::string msg;
};

class FooReq : DBBD::Request {
public:
	FooReq() { typeId = 2; }
	virtual ~FooReq() {}

	// Request을(를) 통해 상속됨
	virtual void serialize(DBBD::Buffer& buffer)
	{
		writeHeader(buffer, getLength());
		DBBD::Serialize::write(buffer, msgSize);
	}
	virtual void deserialize(DBBD::Buffer& buffer)
	{
		readHeader(buffer);
		DBBD::Deserialize::read(buffer, msgSize);
	}

	virtual size_t getLength() {
		return Request::getLength() + DBBD::GetPacketLength(msgSize);
	}

public:
	size_t getMsgSize() { return msgSize; }
	void setMsgSize(const size_t& value) { msgSize = value; }

private:
	size_t msgSize;
};

Player::Player(DBBD::TcpSession::pointer session)
	: TimerObject(session->getContext()), session(session) {
	bindReadInternal(this->session->readInternal);
	std::cout << "Player[" << session->getSessionId() << "] call..." << std::endl;
}

Player::~Player() {
	std::cout << "~Player[" << session->getSessionId() << "] call..." << std::endl;
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
	case 1: {
		ChattingReq chatReq;
		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&chatReq);
		std::cout << "[" << session->getSessionId() << "]CharringReq : " << chatReq.getMsg() << std::endl;
		for (size_t i = 0; i < 1000; i++) {
			// do nothing
		}
		break;
	}
	case 2: {
		FooReq fooReq;
		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&fooReq);
		std::cout << "[" << session->getSessionId() << "]FooReq : " << fooReq.getMsgSize() << std::endl;
		for (size_t i = 0; i < 2000; i++) {
			// do nothing
		}
		break;
	}
	case ProtocolType::PingCheckResp: {
		PingCheckResp resp;
		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&resp);
		std::cout << "[" << session->getSessionId() << "]" << "pong" << std::endl;
		break;
	}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	return true;
}

void Player::registTimerEvent() {
	addTimerEvent(1, TIMER_BINDING(&Player::update), 1000, true);
	addTimerEvent(2, TIMER_BINDING(&Player::pingCheck), 1000, true);
	//addTimerEvent(1, TIMER_BINDING(&Player::update), 1000);
}

void Player::update() {
	size_t sessionId = this->session->getSessionId();
	//std::cout << "[" << std::this_thread::get_id() << "]Player[" << sessionId << "] updated..." << std::endl;
	//addTimerEvent(1, TIMER_BINDING(&Player::update), 1000);
}

void Player::pingCheck() {
	std::cout << "[" << session->getSessionId() << "]" << "ping" << std::endl;
	PingCheckReq req;
	send((DBBD::Cell*)&req);
}