#include <stdio.h>

#include <thread>

#include "EasyTcpClient.hpp"

#include "MesProtocol.hpp"

bool g_turn = true;
void SendCMD()
{
	char _write[32];

	while (true)
	{
		scanf("%s", _write);

		if (0 == strcmp(_write, "exit.."))
		{
			printf("exit client\n");
			g_turn = false;

			break;
		}
	}
}

int main()
{

	//EasyTcpClient client1;

	////client1.Connect("127.0.0.1", 4567);
	//client1.Connect("192.168.0.60", 4567);
	//std::thread t1(SendCMD, &client1);

	//t1.detach();

	//Login _login;

	//strcpy(_login._userName, "admin");

	//strcpy(_login._userPassWord, "123456");
	//while (client1.IsRun())
	//{
	//	client1.OnRun();
	//	client1.SendData(&_login);
	//}

	//client1.Closesocket();
#define CLIENTS 1000
	EasyTcpClient* client[CLIENTS];
	for (int n = 0; n<CLIENTS; n++)
	{
		client[n] = new EasyTcpClient();
	}
	for (int n = 0; n<CLIENTS; n++)
	{
		//client[n]->Connect("192.168.0.64", 4567);
		client[n]->Connect("127.0.0.1", 4567);

	}
	std::thread t1(SendCMD);

	t1.detach();
	Login _login;
	strcpy(_login._userName, "admin");
	strcpy(_login._userPassWord, "123456");
	while (g_turn)
	{
		for (int n = 0; n < CLIENTS; n++)
		{
			client[n]->SendData(&_login);
			client[n]->OnRun();
		}
	}
	getchar();

	return 0;

}