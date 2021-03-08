#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "DBBD/Common.hpp"
#include "ProtocolType.hpp"

// 유저 정보
class UserInfo : public DBBD::Cell {
public:
	UserInfo() {}

	virtual ~UserInfo() {}


public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, nickname); }
		if (fingerPrinter[1]) { DBBD::Serialize::write(buffer, level); }
	}

	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Deserialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, nickname); }
		if (fingerPrinter[1]) { DBBD::Deserialize::read(buffer, level); }
	}

	virtual unsigned int getLength() {
		unsigned int totalLength = 0;
		totalLength += sizeof(unsigned int) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += sizeof(unsigned int) + (nickname.size() * sizeof(wchar_t)); }
		if (fingerPrinter[1]) { totalLength += sizeof(__int64); }
		return totalLength;
	}

	virtual std::string toString() { 
		return "[UserInfo] {  }";
	}

	std::string toJson() {
		nlohmann::json j;
		if (fingerPrinter[0]) { j["nickname"] = DBBD::uniToUtf8(nickname); }
		if (fingerPrinter[1]) { j["level"] = level; }
		return j.dump();
	}

	void fromJson(std::string rawJson) {
		nlohmann::json j = nlohmann::json::parse(rawJson);
		if (!j["nickname"].is_null()) {
			std::string utf8 = j["nickname"].get<std::string>();
			nickname = DBBD::utf8ToUni(utf8);
		}
		if (!j["level"].is_null()) {
			level = j["level"].get<__int64>();
		}
	}

public:
	std::wstring getNickname() { return nickname; }
	void setNickname(std::wstring value) {
		nickname = value;
		fingerPrinter[0] = true;
	}
	__int64 getLevel() { return level; }
	void setLevel(__int64 value) {
		level = value;
		fingerPrinter[1] = true;
	}

protected:
	bool fingerPrinter[2] = { false, };
	// 닉네임
	std::wstring nickname;
	// 레벨
	__int64 level;
};

class PingCheckReq : public DBBD::Request {
public:
	PingCheckReq() {
		typeId = ProtocolType::PingCheckReq;
	}

	virtual ~PingCheckReq() {}


public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Request::writeHeader(buffer, getLength());
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = DBBD::Request::getLength();
		return totalLength;
	}
};

class PingCheckResp : public DBBD::Response {
public:
	PingCheckResp() {
		typeId = ProtocolType::PingCheckResp;
	}

	virtual ~PingCheckResp() {}


public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Response::writeHeader(buffer, getLength());
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Response::readHeader(buffer);
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = DBBD::Response::getLength();
		return totalLength;
	}
};

