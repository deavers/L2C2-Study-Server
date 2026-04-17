#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>
#include <iostream>
#include <thread>

constexpr int LOGIN_PORT = 2106;

void handle_client(int client_fd, sockaddr_in client_addr)
{
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
    int port = ntohs(client_addr.sin_port);

    std::cout << "[+] client connected from " << ip << ":" << port << std::endl;

    const char *hello = "Hello from L2C2 login server\n";
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
}

int main()
{
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        perror("socket");
        return 1;
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(LOGIN_PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr) <= 0)
    {
        std::cerr << "inet_pton failed\n";
        return 1;
    }

    if (bind(server_fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        perror("bind");
        return 1;
    }

    if (listen(server_fd, 16) < 0)
    {
        perror("listen");
        return 1;
    }

    std::cout << "[OK] L2C2 login server listening on 127.0.0.1:" << LOGIN_PORT << std::endl;

    while (true)
    {
        sockaddr_in client_addr{};
        socklen_t len = sizeof(client_addr);
        int client_fd = accept(server_fd, reinterpret_cast<sockaddr*>(&client_addr), &len);

        if (client_fd < 0)
        {
            perror("accept");
            continue;
        }

        std::thread t(handle_client, client_fd, client_addr);
        t.detach();
    }

    close(server_fd);
    return 0;
}
