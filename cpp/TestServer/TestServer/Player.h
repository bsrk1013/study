#pragma once
#include "DBBD/TcpSession.h"
#include "DBBD/TimerObject.h"
#include "DBBD/Define.h"

class Player : public DBBD::TimerObject, DBBD::ITcpSession
{
public:
	Player(DBBD::TcpSession::pointer session);
	~Player();

protected:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void bindReadInternal(DBBD::ReadInternalParam& dest) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	void update(const boost::system::error_code&);

private:
	DBBD::TcpSession::pointer session;
	size_t count = 0;
};