#pragma once

#include "StdAfx.h"

#include "Session.h"

class Listener: public std::enable_shared_from_this<Listener>
{
public:
	Listener(net::io_context& ioc, tcp::endpoint endpoint)
		: io_context(ioc)
		, acceptor(ioc)
	{
		beast::error_code ec;

		acceptor.open(endpoint.protocol(), ec);

		if (ec)
		{
			std::cerr << "open: " << ec.message() << "\n";
			return;
		}

		acceptor.set_option(net::socket_base::reuse_address(true), ec);

		if (ec)
		{
			std::cerr << "set_option: " << ec.message() << "\n";
			return;
		}

		acceptor.bind(endpoint, ec);

		if (ec)
		{
			std::cerr << "bind: " << ec.message() << "\n";
			return;
		}

		acceptor.listen(net::socket_base::max_listen_connections, ec);

		if (ec)
		{
			std::cerr << "listen: " << ec.message() << "\n";
			return;
		}
	}

	void run()
	{
		doAccept();
	}

protected:
	void doAccept()
	{
		acceptor.async_accept
		(
			net::make_strand(io_context),
			beast::bind_front_handler(&Listener::onAccept, shared_from_this())
		);
	}

	void onAccept(beast::error_code ec, tcp::socket socket)
	{
		if (ec)
		{
			std::cerr << "onAccept: " << ec.message() << "\n";
			return;
		}
		else
		{
			std::make_shared<Session>(std::move(socket))->run();
		}

		doAccept();
	}

private:
	net::io_context& io_context;
	tcp::acceptor acceptor;
};
