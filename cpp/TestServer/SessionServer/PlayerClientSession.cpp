#include "PlayerClientSession.h"

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

void PlayerClientSession::readInternal(DBBD::Header)
{
}

void PlayerClientSession::writeInternal()
{
}

void PlayerClientSession::update() {
	std::cout << "updated..." << std::endl;
}