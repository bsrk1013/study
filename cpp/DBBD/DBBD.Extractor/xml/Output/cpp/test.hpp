#include "DBBD/Cell.h"
#include "DBBD/Request.h"
#include "DBBD/Response.h"

// 유저 정보
class UserInfo : public Cell {
public:
	UserInfo() {}
	UserInfo(std::stringNickname, longLevel)
		: Nickname(Nickname), Level(Level)
	{}
	virtual ~UserInfo() {}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		Serialize::write(buffer, Nickname);
		Serialize::write(buffer, Level);
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		Deserialize::read(buffer, Nickname);
		Deserialize::read(buffer, Level);
	}
	virtual size_t getLength() {
		return sizeof(size_t) + Nickname.length()sizeof(Level);
	}

public:
	std::string getNickname() { return Nickname; }
	void setNickname(std::string value) { Nickname = value; }
	long getLevel() { return Level; }
	void setLevel(long value) { Level = value; }

private:
	// 닉네임
	std::string Nickname;
	// 레벨
	long Level;
}
class PingCheckReq : public Request {
public:
	PingCheckReq() {}
	PingCheckReq()
		: 
	{}
	virtual ~PingCheckReq() {}
class PingCheckResp : public Response {
public:
	PingCheckResp() {}
	PingCheckResp()
		: 
	{}
	virtual ~PingCheckResp() {}

public:
	virtual void serialize(DBBD::Buffer& buffer) {
		Request::writeHeader(buffer, getLength());
	}
	virtual void deserialize(DBBD::Buffer& buffer) {
		Request::readHeader(buffer);
	}
	virtual size_t getLength() {
		return ;
	}

}
