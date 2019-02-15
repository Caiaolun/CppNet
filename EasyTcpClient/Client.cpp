
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include<WinSock2.h>
#include <Windows.h>
#endif // _WIN32

#include <stdio.h>

int main()
{
#ifdef _WIN32
	//Start Windwos Socket 2.x Environment
	WORD _ver = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_ver, &_data);

	// 1 Build Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("==ERROR==: build socket faild!\n");
	}
	else
	{
		printf("==SUCCESS==: build socket success!\n");
	}

	// 2 Connect Server
	sockaddr_in _server = {};
	_server.sin_family = AF_INET;
	_server.sin_port = htons(4567);
	_server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(_sock, (sockaddr*)&_server, sizeof(_server)))
	{
		printf("==ERROR==: connect server faild!\n");
	}
	else
	{
		printf("==SUCCESS==: connect server success!\n");
	}

	// 3 Receive Server Data
	char _msgBuf[512] = {};
	int ret = recv(_sock, _msgBuf, sizeof(_msgBuf), 0);
	if (ret > 0)
	{
		printf("rece: %s\n", _msgBuf);
	}
	else
	{
		printf("rece: is null\n");
	}
	// 4 Close Socket
	closesocket(_sock);

	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}