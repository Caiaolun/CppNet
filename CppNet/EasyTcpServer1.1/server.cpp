#include <stdio.h>
#include "EasyTcpServer.hpp"

//NewUserJoin* _newUserJoin = new NewUserJoin();

int main()
{

	EasyTcpServer server;
	server.InitSocket();
	//server.Bind("127.0.0.1", 4567);
	server.Bind();
	server.Listen(5);

	while (server.IsRun())
	{
		server.OnRun();
	}
	server.CloseSocket();

	getchar();
	return 0;
}