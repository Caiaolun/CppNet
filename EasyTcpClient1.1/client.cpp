#include <stdio.h>
#include <iostream>
#include <thread>
#include <mutex>
#include "EasyTcpClient.hpp"

#include "MesProtocol.hpp"

const int cClinet = 1200;
const int  cThreadCount = 1;
bool g_turn = true;
EasyTcpClient* client[cClinet];

std::mutex m;
void SendCMD()
{
	m.lock();
	std::cout << "SendCMD Thread: " << std::endl;
	m.unlock();

	while (true)
	{
		char _write[32] = {};
		scanf("%s", _write);

		if (0 == strcmp(_write, "exit.."))
		{
			printf("exit client\n");
			g_turn = false;

			break;
		}
	}
}
void ConnectServer(int number)
{
	int tempCount = cClinet / cThreadCount;
	int tempBegin = tempCount * number;
	int tempEnd = tempCount * (number + 1);
	m.lock();
	std::cout << "ConnectServer Thread: " << number << std::endl;
	m.unlock();

	Sleep(50);

	for (int n = tempBegin; n < tempEnd; n++)
	{
		client[n] = new EasyTcpClient();
	}
	for (int n = tempBegin; n < tempEnd; n++)
	{
		//client[tempBegin]->Connect("192.168.0.64", 4567);
		client[n]->Connect("127.0.0.1", 4567);
		printf("thread<%d>,Connect=%d\n", number, n);
	}
	Login _login;
	strcpy(_login._userName, "admin");
	strcpy(_login._userPassWord, "123456");
	while (g_turn)
	{
		for (int n = tempBegin; n < tempEnd; n++)
		{
			client[n]->SendData(&_login);
			//client[tempBegin]->OnRun();
		}
	}

	for (tempBegin; tempBegin < tempEnd; tempBegin++)
	{
		client[tempBegin]->Closesocket();
	}
}
int main()
{
	std::thread t1(SendCMD);
	t1.detach();

	for (int n = 0; n < cThreadCount; n++)
	{
		std::thread t(ConnectServer, n);
		t.detach();
	}

	while (g_turn)
		Sleep(100);

	return 0;

}