#pragma once

#include "../net/tcp_server.h"

class LoginServer
{
public:
	explicit LoginServer(int port);
	bool start();
	void stop();

private:
	TcpServer server_;
};
