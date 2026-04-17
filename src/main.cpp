#include "login/login_server.h"
#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	constexpr int LOGIN_PORT = 2106;

	LoginServer login(LOGIN_PORT);
	if (!login.start())
	{
		std::cerr << "Failed to start login server\n";
		return 1;
	}

	std::cout << "[INFO] Login server is running. Press Ctrl+C to exit.\n";

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}

	return 0;
}
