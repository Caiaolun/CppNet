#include <stdio.h>
#include <thread>
#include "EasyTcpClient.hpp"
#include "MesProtocol.hpp"
int SendCMD(EasyTcpClient *client)
{
	char _write[32];
	while (true)
	{
		scanf("%s", _write);

		if (0 == strcmp(_write, "exit.."))
		{
			printf("exit client\n");
			client->Closesocket();
			break;
		}
		else if (0 == strcmp(_write, "login"))
		{
			Login _login;
			strcpy(_login._userName, "admin");
			strcpy(_login._userPassWord, "123456");
			client->SendData(&_login);
		}
		else if (0 == strcmp(_write, "loginOut"))
		{
			LoginOut _loginOut;
			strcpy(_loginOut._userName, "admin");
			client->SendData(&_loginOut);
		}
		else
		{
			printf("Unable to parse command\n");
			continue;
		}
	}
	return 0;
}
int main()
{

	EasyTcpClient client1;
	client1.Connect("127.0.0.1", 4567);

	std::thread t1(SendCMD, &client1);
	t1.detach();

	while(client1.IsRun())
	{
		client1.OnRun();
	}
	client1.Closesocket();
	getchar();
	return 0;
}