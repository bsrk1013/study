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
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = DBBD::Request::getLength();
		return totalLength;
	}
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
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, sessionId); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Response::readHeader(buffer);
		DBBD::Serialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, sessionId); }
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = DBBD::Response::getLength();
		totalLength += sizeof(unsigned int) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += sizeof(int); }
		return totalLength;
	}

public:
	int getSessionId() { return sessionId; }
	void setSessionId(int value) {
		sessionId = value;
		fingerPrinter[0] = true;
	}

protected:
	bool fingerPrinter[1] = { false, };
	int sessionId;
};

class RelayNoti : public DBBD::Request {
public:
	RelayNoti() {
		typeId = ProtocolType::RelayNoti;
	}

	virtual ~RelayNoti() {}


public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Request::writeHeader(buffer, getLength());
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, userId); }
		if (fingerPrinter[1]) { DBBD::Serialize::write(buffer, relayType); }
		if (fingerPrinter[2]) { DBBD::Serialize::write(buffer, intValue01); }
		if (fingerPrinter[3]) { DBBD::Serialize::write(buffer, intValue02); }
		if (fingerPrinter[4]) { DBBD::Serialize::write(buffer, intValue03); }
		if (fingerPrinter[5]) { DBBD::Serialize::write(buffer, intValue04); }
		if (fingerPrinter[6]) { DBBD::Serialize::write(buffer, floatValue01); }
		if (fingerPrinter[7]) { DBBD::Serialize::write(buffer, floatValue02); }
		if (fingerPrinter[8]) { DBBD::Serialize::write(buffer, floatValue03); }
		if (fingerPrinter[9]) { DBBD::Serialize::write(buffer, floatValue04); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
		DBBD::Serialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, userId); }
		if (fingerPrinter[1]) { DBBD::Deserialize::read(buffer, relayType); }
		if (fingerPrinter[2]) { DBBD::Deserialize::read(buffer, intValue01); }
		if (fingerPrinter[3]) { DBBD::Deserialize::read(buffer, intValue02); }
		if (fingerPrinter[4]) { DBBD::Deserialize::read(buffer, intValue03); }
		if (fingerPrinter[5]) { DBBD::Deserialize::read(buffer, intValue04); }
		if (fingerPrinter[6]) { DBBD::Deserialize::read(buffer, floatValue01); }
		if (fingerPrinter[7]) { DBBD::Deserialize::read(buffer, floatValue02); }
		if (fingerPrinter[8]) { DBBD::Deserialize::read(buffer, floatValue03); }
		if (fingerPrinter[9]) { DBBD::Deserialize::read(buffer, floatValue04); }
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = DBBD::Request::getLength();
		totalLength += sizeof(unsigned int) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += sizeof(int); }
		if (fingerPrinter[1]) { totalLength += sizeof(int); }
		if (fingerPrinter[2]) { totalLength += sizeof(int); }
		if (fingerPrinter[3]) { totalLength += sizeof(int); }
		if (fingerPrinter[4]) { totalLength += sizeof(int); }
		if (fingerPrinter[5]) { totalLength += sizeof(int); }
		if (fingerPrinter[6]) { totalLength += sizeof(float); }
		if (fingerPrinter[7]) { totalLength += sizeof(float); }
		if (fingerPrinter[8]) { totalLength += sizeof(float); }
		if (fingerPrinter[9]) { totalLength += sizeof(float); }
		return totalLength;
	}

public:
	int getUserId() { return userId; }
	void setUserId(int value) {
		userId = value;
		fingerPrinter[0] = true;
	}
	int getRelayType() { return relayType; }
	void setRelayType(int value) {
		relayType = value;
		fingerPrinter[1] = true;
	}
	int getIntValue01() { return intValue01; }
	void setIntValue01(int value) {
		intValue01 = value;
		fingerPrinter[2] = true;
	}
	int getIntValue02() { return intValue02; }
	void setIntValue02(int value) {
		intValue02 = value;
		fingerPrinter[3] = true;
	}
	int getIntValue03() { return intValue03; }
	void setIntValue03(int value) {
		intValue03 = value;
		fingerPrinter[4] = true;
	}
	int getIntValue04() { return intValue04; }
	void setIntValue04(int value) {
		intValue04 = value;
		fingerPrinter[5] = true;
	}
	float getFloatValue01() { return floatValue01; }
	void setFloatValue01(float value) {
		floatValue01 = value;
		fingerPrinter[6] = true;
	}
	float getFloatValue02() { return floatValue02; }
	void setFloatValue02(float value) {
		floatValue02 = value;
		fingerPrinter[7] = true;
	}
	float getFloatValue03() { return floatValue03; }
	void setFloatValue03(float value) {
		floatValue03 = value;
		fingerPrinter[8] = true;
	}
	float getFloatValue04() { return floatValue04; }
	void setFloatValue04(float value) {
		floatValue04 = value;
		fingerPrinter[9] = true;
	}

protected:
	bool fingerPrinter[10] = { false, };
	int userId;
	int relayType;
	int intValue01;
	int intValue02;
	int intValue03;
	int intValue04;
	float floatValue01;
	float floatValue02;
	float floatValue03;
	float floatValue04;
};

