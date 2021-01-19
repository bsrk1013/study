#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "ProtocolType.hpp"

// 유저 정보
class UserInfo : public DBBD::Cell {
public:
	UserInfo() {}
	UserInfo(std::string Nickname, long Level)
		: Nickname(Nickname), Level(Level)
	{}
	virtual ~UserInfo() {}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Serialize::write(buffer, Nickname);
		DBBD::Serialize::write(buffer, Level);
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Deserialize::read(buffer, Nickname);
		DBBD::Deserialize::read(buffer, Level);
	}
	virtual size_t getLength() {
		return sizeof(size_t) + Nickname.length() + sizeof(long);
	}

public:
	std::string getNickname() { return Nickname; }
	void setNickname(std::string value) { Nickname = value; }
	long getLevel() { return Level; }
	void setLevel(long value) { Level = value; }

protected:
	// 닉네임
	std::string Nickname;
	// 레벨
	long Level;
}

class PingCheckReq : public DBBD::Request {
public:
	PingCheckReq() {
		typeId = ProtocolType::PingCheckReq;
	}	virtual ~PingCheckReq() {}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Request::writeHeader(buffer, getLength());
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
	}
	virtual size_t getLength() {
		return DBBD::Request::getLength();
	}

};

class PingCheckResp : public DBBD::Response {
public:
	PingCheckResp() {
		typeId = ProtocolType::PingCheckResp;
	}	virtual ~PingCheckResp() {}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Response::writeHeader(buffer, getLength());
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Response::readHeader(buffer);
	}
	virtual size_t getLength() {
		return DBBD::Response::getLength();
	}

};

