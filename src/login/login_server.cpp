#include "login_server.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

LoginServer::LoginServer(int port)
	: server_("127.0.0.1", port) // Listen only localhost
{
}

bool LoginServer::start()
{
	return server_.start([](int client_fd, sockaddr_in client_addr)
	{
		char ip[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
		int port = ntohs(client_addr.sin_port);

		std::cout << "[+] client connected from " << ip << ":" << port << std::endl;

		const char* hello = "Hello from L2C2 login server\n";
		send(client_fd, hello, std::strlen(hello), 0);

		unsigned char buf[1024];
		while (true)
		{
			ssize_t n = recv(client_fd, buf, sizeof(buf), 0);
			if (n == 0)
			{
				std::cout << "[-] client closed connection" << std::endl;
                		break;
			}
			if (n < 0)
			{
				perror("recv");
				break;
			}

			std::cout << "[*] received " << n << " bytes: ";
			for (ssize_t i = 0; i < n; ++i)
			{
				std::cout << std::hex << (int)buf[i] << " ";
			}
			std::cout << std::dec << std::endl;
		}

		close(client_fd);
	});
}

void LoginServer::stop()
{
	server_.stop();
}
