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
	}	ServerConnectReq(long SessionId)
		: SessionId(SessionId)
	{}
	virtual ~ServerConnectReq() {}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Request::writeHeader(buffer, getLength());
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, SessionId); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
		DBBD::Serialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, SessionId); }
	}
	virtual size_t getLength() {
		size_t totalLength = DBBD::Request::getLength();
		totalLength += sizeof(size_t) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += sizeof(long); }
		return totalLength;
	}

public:
	long getSessionId() { return SessionId; }
	void setSessionId(long value) {
		SessionId = value;
		fingerPrinter[0] = true;
	}

protected:
	bool fingerPrinter[1] = { false, };
	long SessionId;
};

class ServerConnectResp : public DBBD::Response {
public:
	ServerConnectResp() {
		typeId = ProtocolType::ServerConnectResp;
	}	virtual ~ServerConnectResp() {}

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

// 핑 요청

};

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

// 핑 응답

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

