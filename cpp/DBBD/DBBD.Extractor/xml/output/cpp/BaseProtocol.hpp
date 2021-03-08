#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "DBBD/Common.hpp"
#include "ProtocolType.hpp"

class ServerConnectReq : public DBBD::Request {
public:
	ServerConnectReq() {
		typeId = ProtocolType::ServerConnectReq;
	}

	virtual ~ServerConnectReq() {}


public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Request::writeHeader(buffer, getLength());
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, sessionId); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
		DBBD::Serialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, sessionId); }
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = DBBD::Request::getLength();
		totalLength += sizeof(unsigned int) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += sizeof(__int64); }
		return totalLength;
	}

public:
	__int64 getSessionId() { return sessionId; }
	void setSessionId(__int64 value) {
		sessionId = value;
		fingerPrinter[0] = true;
	}

protected:
	bool fingerPrinter[1] = { false, };
	__int64 sessionId;
};

class ServerConnectResp : public DBBD::Response {
public:
	ServerConnectResp() {
		typeId = ProtocolType::ServerConnectResp;
	}

	virtual ~ServerConnectResp() {}


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

// 핑 요청

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

// 핑 응답

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

