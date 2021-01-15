#include "PlayerClientSession.h"

PlayerClientSession::PlayerClientSession(DBBD::IoContextSP context, DBBD::SocketSP socket, size_t sessionId)
	: TcpSessionBase(context, socket, sessionId)
{
}

PlayerClientSession::~PlayerClientSession()
{
}

void PlayerClientSession::readInternal(DBBD::Header)
{
}

void PlayerClientSession::writeInternal()
{
}
