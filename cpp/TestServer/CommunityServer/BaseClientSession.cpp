#include "BaseClientSession.h"

BaseClientSession::BaseClientSession(){}
BaseClientSession::~BaseClientSession(){}

void BaseClientSession::init(DBBD::TcpSession::pointer session)
{
    this->session = session;
    bindReadInternal(this->session->readInternal);
}

void BaseClientSession::send(DBBD::Cell* data)
{
    session->write(data);
}

void BaseClientSession::bindReadInternal(DBBD::ReadInternalParam& dest)
{
    dest = READ_INTERNAL_BINDING(&BaseClientSession::readInternal);
}

bool BaseClientSession::readInternal(const DBBD::Header&, DBBD::Buffer&)
{
    return true;
}
