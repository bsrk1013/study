#pragma once
#include "DBBD/TcpSession.h"

class Player : DBBD::ITcpSession
{
public:
	Player(DBBD::TcpSession::pointer session);
	~Player();

protected:
	// ITcpSession��(��) ���� ��ӵ�
	virtual void bindReadInternal(std::function<bool(const DBBD::Header&, DBBD::Buffer&)>&) override;
	virtual bool readInternal(const DBBD::Header&, DBBD::Buffer&) override;

private:
	DBBD::TcpSession::pointer session;
};