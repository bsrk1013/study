#pragma once
#include "DBBD/TcpClient.h"

class CommunityClient : public DBBD::TcpClient
{
	// TcpClient��(��) ���� ��ӵ�
	virtual void connectInternal(DBBD::TcpSession::pointer session) override;
	virtual void closeInternal() override;
};