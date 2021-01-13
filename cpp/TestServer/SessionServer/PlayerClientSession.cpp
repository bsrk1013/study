#include "PlayerClientSession.h"

PlayerClientSession::PlayerClientSession(){}
PlayerClientSession::~PlayerClientSession() {
	dispose();
}

void PlayerClientSession::init(DBBD::IoContextSP context)
{
	if (!isDisposed) { return; }
	TimerObject::init(context);
}

void PlayerClientSession::dispose()
{
	if (isDisposed) { return; }
	TimerObject::dispose();
}

void PlayerClientSession::reset()
{
	if (!isDisposed) { return; }
	TimerObject::reset();
}

void PlayerClientSession::init(DBBD::TcpSession::pointer session)
{
	this->session = session;
	init(this->session->getContext());
	bindReadInternal(this->session->readInternal);
}

void PlayerClientSession::send(DBBD::Cell* data)
{
	session->write(data);
}

void PlayerClientSession::bindReadInternal(DBBD::ReadInternalParam& dest)
{
	dest = READ_INTERNAL_BINDING(&PlayerClientSession::readInternal);
}

bool PlayerClientSession::readInternal(const DBBD::Header& header, DBBD::Buffer& buffer)
{
	switch (header.typeId) {

	}

	return true;
}

void PlayerClientSession::registTimerEvent()
{
}
