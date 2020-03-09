#pragma once

#include "StdAfx.h"
#include "Listener.h"

class WSServer
{
public:
	WSServer(std::string host, unsigned short port, int num_threads = 1)
	{
		address = net::ip::make_address(host);
		threads = std::max<int>(1, num_threads);
		port_ = port;
	}

	void start()
	{
		net::io_context ioc{ threads };
		std::make_shared<Listener>(ioc, tcp::endpoint{ address, port_ })->run();

		std::vector<std::thread> vThreads;
		vThreads.reserve(threads - 1);

		for (auto i = 0; i < threads - 1; ++i)
		{
			vThreads.emplace_back([&ioc] {
				std::thread::id threadId = std::this_thread::get_id();
				std::cout << "Running io_context in thread: " << threadId << "\n";
				ioc.run();
			});
		}

		std::thread::id threadId = std::this_thread::get_id();
		std::cout << "Running io_context in thread: " << threadId << "\n";

		ioc.run();
	}

private:
	boost::asio::ip::address address;
	unsigned short port_;
	int threads;
};
