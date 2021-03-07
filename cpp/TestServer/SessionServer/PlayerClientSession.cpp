#include "PlayerClientSession.h"
#include "DBBD/Log.h"
#include "DBBD/Response.h"

class UserInfo : public DBBD::Cell {
public:
	UserInfo() {}
	UserInfo(std::wstring Nickname, long Level)
		: Nickname(Nickname), Level(Level)
	{}
	virtual ~UserInfo() {}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, Nickname); }
		if (fingerPrinter[1]) { DBBD::Serialize::write(buffer, Level); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Deserialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, Nickname); }
		if (fingerPrinter[1]) { DBBD::Deserialize::read(buffer, Level); }
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = 0;
		totalLength = sizeof(unsigned int) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += (unsigned int)sizeof(unsigned int) + (Nickname.size() * (unsigned int)sizeof(wchar_t)); }
		if (fingerPrinter[1]) { totalLength += (unsigned int)sizeof(long); }
		return totalLength;
	}
	virtual std::string toString() { return""; }

public:
	std::wstring getNickname() { return Nickname; }
	void setNickname(std::wstring value) {
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
	// ´Ð³×ÀÓ
	std::wstring Nickname;
	// ·¹º§
	short Level;
};

class LoginReq : public DBBD::Request
{
public:
	LoginReq(){
		typeId = 1;
	}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Request::writeHeader(buffer, getLength());
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, (DBBD::Cell*)&user); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Request::readHeader(buffer);
		DBBD::Deserialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, (DBBD::Cell*)&user); }
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = 0;
		totalLength = DBBD::Request::getLength() + sizeof(unsigned int) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += user.getLength(); }
		return totalLength;
	}
	virtual std::string toString() {
		return "";
	}

public:
	UserInfo getUser() { return user; }
	void setUser(UserInfo value) { 
		user = value; 
		fingerPrinter[0] = true;
	}

private:
	bool fingerPrinter[1] = { false };
	UserInfo user;
};

class LoginResp : public DBBD::Response
{
public:
	LoginResp() {
		typeId = 2;
	}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		DBBD::Response::writeHeader(buffer, getLength());
		DBBD::Serialize::writeArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Serialize::write(buffer, loginResult); }
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		DBBD::Response::readHeader(buffer);
		DBBD::Deserialize::readArray(buffer, fingerPrinter);
		if (fingerPrinter[0]) { DBBD::Deserialize::read(buffer, loginResult); }
	}
	virtual unsigned int getLength() {
		unsigned int totalLength = 0;
		totalLength = DBBD::Response::getLength() + sizeof(unsigned int) + sizeof(fingerPrinter);
		if (fingerPrinter[0]) { totalLength += sizeof(bool); }
		return totalLength;
	}
	virtual std::string toString() {
		return "";
	}

public:
	bool getLoginResult() { return loginResult; }
	void setLoginResult(bool value) {
		loginResult = value;
		fingerPrinter[0] = true;
	}

private:
	bool fingerPrinter[1] = { false };
	bool loginResult;
};

PlayerClientSession::PlayerClientSession(DBBD::IoContextSP context, DBBD::SocketSP socket, size_t sessionId)
	: TcpSessionBase(context, socket, sessionId)
{
}

PlayerClientSession::~PlayerClientSession()
{
}

void PlayerClientSession::startInternal() {
	addTimerEvent(1, TIMER_BINDING(&PlayerClientSession::update), 1000, true);
}

void PlayerClientSession::readInternal(DBBD::Header header)
{
	char* block = readBuffer->readByteBlock(header.length);
	DBBD::Buffer buffer(block, header.length);

	switch (header.typeId)
	{
	case 1:
		LoginReq req;
		DBBD::Deserialize::read(buffer, (DBBD::Cell*)&req);

		std::wstring nickname = req.getUser().getNickname();
		short level = req.getUser().getLevel();

		if (wcscmp(nickname.c_str(), L"»§Ä®¹ý»ç") == 0
			&& level == 80) {
			LoginResp resp;
			resp.setLoginResult(true);

			send((DBBD::Cell*)&resp);
		}
	}
}

void PlayerClientSession::writeInternal()
{
}

void PlayerClientSession::update() {
	LOG_DEBUG("player updated...");
}