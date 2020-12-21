#pragma once
#include "DBBD/TcpSession.h"
#include "DBBD/TimerObject.h"

class Player : public DBBD::TimerObject, DBBD::ITcpSession
{
public:
	Player(DBBD::TcpSession::pointer session);
	~Player();

protected:
	// ITcpSession을(를) 통해 상속됨
	virtual void bindReadInternal(std::function<bool(const DBBD::Header&, DBBD::Buffer&)>&) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	void update(const boost::system::error_code&);

private:
	DBBD::TcpSession::pointer session;
	size_t count = 0;
};