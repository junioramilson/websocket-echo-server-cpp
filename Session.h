#pragma once

#include "StdAfx.h"

class Session: public std::enable_shared_from_this<Session>
{
public:
	explicit Session(tcp::socket&& socket)
		: ws(std::move(socket))
	{
	}

	void run()
	{
		net::dispatch(
			ws.get_executor(),
			beast::bind_front_handler(&Session::onRun, shared_from_this())
		);
	}

	void onRun()
	{
		ws.set_option(websocket::stream_base::timeout::suggested(beast::role_type::server));

		ws.set_option(websocket::stream_base::decorator(
			[](websocket::response_type & res)
			{
				res.set(http::field::server, std::string(BOOST_BEAST_VERSION_STRING) + " websocket-server-async");
			}
		));

		ws.async_accept(beast::bind_front_handler(&Session::onAccept, shared_from_this()));
	}

	void onAccept(beast::error_code ec)
	{
		if (ec)
		{
			std::cerr << "accept: " << ec.message() << "\n";
			return;
		}

		doRead();
	}

	void doRead()
	{
		ws.async_read(buffer, beast::bind_front_handler(&Session::onRead, shared_from_this()));
	}

	void onRead(beast::error_code ec, std::size_t bytes_transfered)
	{
		boost::ignore_unused(bytes_transfered);

		if (ec == websocket::error::closed)
			return;

		if (ec)
		{
			std::cerr << "read: " << ec.message() << "\n";
			return;
		}

		ws.text(ws.got_text());
		ws.async_write(buffer.data(), beast::bind_front_handler(&Session::onWrite, shared_from_this()));
	}

	void onWrite(beast::error_code ec, std::size_t bytes_transfered)
	{
		boost::ignore_unused(bytes_transfered);

		if (ec)
		{
			std::cerr << "write: " << ec.message() << "\n";
			return;
		}

		buffer.consume(buffer.size());

		doRead();
	}

private:
	websocket::stream<beast::tcp_stream> ws;
	beast::flat_buffer buffer;
};
