#pragma once

#include <functional>
#include <netinet/in.h>
#include <thread>
#include <vector>

class TcpServer
{
public:
	using ClientHandler = std::function<void(int client_fd, sockaddr_in client_addr)>;

	TcpServer(const char* ip, int port);
	~TcpServer();

	// Forbidden to Copy
	TcpServer(const TcpServer&) = delete;
	TcpServer& operator=(const TcpServer&) = delete;

	bool start(ClientHandler handler);
	void stop();

private:
	int server_fd_{-1};
	sockaddr_in addr_{};
	bool running_{false};
	ClientHandler handler_;

	std::vector<std::thread> workers_;

	void acceptLoop();
};
