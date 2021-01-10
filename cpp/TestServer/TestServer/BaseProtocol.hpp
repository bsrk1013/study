#pragma once
#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"
#include "ProtocolType.hpp"

class PingCheckReq : public DBBD::Request {
public:
	PingCheckReq() {
		typeId = (size_t)ProtocolType::Value::PingCheckReq;
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

