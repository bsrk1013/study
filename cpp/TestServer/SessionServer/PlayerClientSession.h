#pragma once
#include "DBBD/TcpSessionBase.h"

class PlayerClientSession : public DBBD::TcpSessionBase
{
public:
	// context, socket, bufferSize
	PlayerClientSession(DBBD::IoContextSP, DBBD::SocketSP, size_t);
	virtual ~PlayerClientSession();

	// TcpSessionBase��(��) ���� ��ӵ�
	virtual void readInternal(DBBD::Header) override;
	virtual void writeInternal() override;
};