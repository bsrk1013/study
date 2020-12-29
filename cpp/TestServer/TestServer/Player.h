#pragma once
#include "DBBD/TcpSession.h"
#include "DBBD/TimerObject.h"
#include "DBBD/Define.h"

class Player : 
	public std::enable_shared_from_this<Player>,
	public DBBD::TimerObject,
	DBBD::ITcpSession
{
public:
	Player(DBBD::TcpSession::pointer session);
	virtual ~Player();

protected:
	// ITcpSession을(를) 통해 상속됨
	virtual void bindReadInternal(DBBD::ReadInternalParam& dest) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	void update(const boost::system::error_code&);

private:
	bool isDispose = false;
	DBBD::TcpSession::pointer session;
	size_t count = 0;
};