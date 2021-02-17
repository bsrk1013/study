#pragma once
	#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "DBBD/ProtocolType.hpp"
#include "DBBD/Common.hpp"
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
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if(fingerPrinter[0]) { DBBD::Serialize::write(buffer, Nickname); }
		if(fingerPrinter[1]) { DBBD::Serialize::write(buffer, Level); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Deserialize::readArray(buffer, fingerPrinter);
		if(fingerPrinter[0]) { DBBD::Deserialize::read(buffer, Nickname); }
		if(fingerPrinter[1]) { DBBD::Deserialize::read(buffer, Level); }
	}
	virtual size_t getLength() {
		size_t totalLength = sizeof(size_t) + sizeof(fingerPrinter);
		if(fingerPrinter[0]) { totalLength += sizeof(size_t) + Nickname.length(); }
		if(fingerPrinter[1]) { totalLength += sizeof(long); }
		return totalLength();
	}

public:
	std::string getNickname() { return Nickname; }
	void setNickname(std::string value) {
		Nickname = value;
		fingerPrinter[0] = true;
	}
	long getLevel() { return Level; }
	void setLevel(long value) {
		Level = value;
		fingerPrinter[1] = true;
	}

protected:
	bool fingerPrinter[2] = { false, };
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
		size_t totalLength = DBBD::Request::getLength();
		return totalLength;
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
		size_t totalLength = DBBD::Response::getLength();
		return totalLength;
	}
};

