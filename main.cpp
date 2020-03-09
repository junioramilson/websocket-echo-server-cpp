#include "WSServer.h"

int main(int argc, char** argv)
{
	std::unique_ptr<WSServer> wsServer(new WSServer("127.0.0.1", 3030, 4));

	wsServer->start();

	return 0;
}