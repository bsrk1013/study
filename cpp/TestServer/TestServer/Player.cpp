#include "Player.h"
#include <iostream>

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
		return Request::getLength() + sizeof(size_t) + msg.size();
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
		return Request::getLength() + sizeof(msgSize);
	}

public:
	size_t getMsgSize() { return msgSize; }
	void setMsgSize(const size_t& value) { msgSize = value; }

private:
	size_t msgSize;
};

Player::Player(DBBD::TcpSession::pointer session)
: session(session){
	bindReadInternal(session->readInternal);
}

Player::~Player() {
}

bool Player::readInternal(const DBBD::Header& header, DBBD::Buffer& buffer)
{
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
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////
	return true;
}

//void Player::readInternal(DBBD::Header header)
//{
//	/////////////////////////////////////////////////////////////////////// readInternal에서 구현해야함
//	switch (header.typeId) {
//	case 1: {
//		ChattingReq chatReq;
//		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&chatReq);
//		std::cout << "[" << session->getSessionId() << "]CharringReq : " << chatReq.getMsg() << std::endl;
//		for (size_t i = 0; i < 1000; i++) {
//			// do nothing
//		}
//		break;
//	}
//	case 2: {
//		FooReq fooReq;
//		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&fooReq);
//		std::cout << "[" << session->getSessionId() << "]FooReq : " << fooReq.getMsgSize() << std::endl;
//		for (size_t i = 0; i < 2000; i++) {
//			// do nothing
//		}
//		break;
//	}
//	}
//	///////////////////////////////////////////////////////////////////////////////////////////////////
//}
