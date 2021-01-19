#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
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
		DBBD::Serialize::write(buffer, SessionId);
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
		DBBD::Deserialize::read(buffer, SessionId);
	}
	virtual size_t getLength() {
		return DBBD::Request::getLength()sizeof(long);
	}

public:
	long getSessionId() { return SessionId; }
	void setSessionId(long value) { SessionId = value; }

protected:
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
		return DBBD::Response::getLength();
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
		return DBBD::Request::getLength();
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
		return DBBD::Response::getLength();
	}

};

