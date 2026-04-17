#include "tcp_server.h"

#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>

TcpServer::TcpServer(const char* ip, int port) 
{
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
    {
        perror("socket");
        return;
    }

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    std::memset(&addr_, 0, sizeof(addr_));
    addr_.sin_family = AF_INET;
    addr_.sin_port   = htons(port);

    if (inet_pton(AF_INET, ip, &addr_.sin_addr) <= 0)
    {
        std::cerr << "inet_pton failed for " << ip << std::endl;
        close(server_fd_);
        server_fd_ = -1;
        return;
    }
}

TcpServer::~TcpServer() {
    stop();
}

bool TcpServer::start(ClientHandler handler)
{
    if (server_fd_ < 0)
    {
        std::cerr << "Server socket not initialized\n";
        return false;
    }

    handler_ = std::move(handler);

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr_), sizeof(addr_)) < 0)
    {
        perror("bind");
        return false;
    }

    if (listen(server_fd_, 16) < 0)
    {
        perror("listen");
        return false;
    }

    running_ = true;
    std::cout << "[OK] TcpServer listening on 127.0.0.1:"
              << ntohs(addr_.sin_port) << std::endl;

    workers_.emplace_back(&TcpServer::acceptLoop, this);
    return true;
}

void TcpServer::stop()
{
    if (!running_)
        return;
    running_ = false;

    if (server_fd_ >= 0)
    {
        close(server_fd_);
        server_fd_ = -1;
    }

    for (auto& t : workers_)
    {
        if (t.joinable())
        {
            t.join();
        }
    }
    workers_.clear();
}

void TcpServer::acceptLoop()
{
    while (running_)
    {
        sockaddr_in client_addr;
        std::memset(&client_addr, 0, sizeof(client_addr));
        socklen_t len = sizeof(client_addr);

        int client_fd = accept(
            server_fd_,
            reinterpret_cast<sockaddr*>(&client_addr),
            &len
        );

        if (client_fd < 0)
        {
            if (running_)
            {
                perror("accept");
            }
            continue;
        }

        std::thread th([this, client_fd, client_addr]() {
            handler_(client_fd, client_addr);
        });
        th.detach();
    }
}
