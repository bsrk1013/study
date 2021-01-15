#pragma once
#include "DBBD/TcpSessionBase.h"

class PlayerClientSession : public DBBD::TcpSessionBase
{
public:
	// context, socket, bufferSize
	PlayerClientSession(DBBD::IoContextSP, DBBD::SocketSP, size_t);
	virtual ~PlayerClientSession();

	// TcpSessionBase을(를) 통해 상속됨
	virtual void readInternal(DBBD::Header) override;
	virtual void writeInternal() override;
};