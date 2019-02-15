
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include<WinSock2.h>
#include <Windows.h>
#endif // _WIN32

#include <stdio.h>
#include <memory>
#include <stdlib.h>
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
		printf("==ERROR==: build socket faild...\n");
	}
	else
	{
		printf("==SUCCESS==: build socket success...\n");
	}

	// 2 Connect Server
	sockaddr_in _server = {};
	_server.sin_family = AF_INET;
	_server.sin_port = htons(4567);
	_server.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (SOCKET_ERROR == connect(_sock, (sockaddr*)&_server, sizeof(_server)))
	{
		printf("==ERROR==: connect server faild...\n");
	}
	else
	{
		printf("==SUCCESS==: connect server success...\n");
	}

	// 3 Receive/Send Server Data
	char* _receBuf = (char*)malloc(sizeof(char) * 512);
	char* _sendBuf= (char*)malloc(sizeof(char) * 512);
	memset(_receBuf, 0, _msize(_receBuf));
	memset(_sendBuf, 0, _msize(_sendBuf));
	//char _sendtemp[512] = {};
	while (true)
	{

		scanf("%s", _sendBuf);
		if (0 == strcmp(_sendBuf, "exit.."))
		{
			printf("exit client\n");
			break;
		}
		else
		{
			*(_sendBuf + strlen(_sendBuf)) = '\0';
			send(_sock, _sendBuf, strlen(_sendBuf) + 1, 0);
		}

		int ret = recv(_sock, _receBuf, _msize(_receBuf), 0);
		if (ret > 0)
		{	
			*(_receBuf + ret) = '\0';
			printf("rece: %s\n", _receBuf);
		}

		memset(_receBuf, NULL, _msize(_receBuf));
		memset(_sendBuf, NULL, _msize(_sendBuf));

	}
	/*
	while (true)
	{
		int ret = recv(_sock, _receBuf, sizeof(_receBuf), 0);
		if (ret > 0)
		{
			printf("rece: %s\n", _receBuf);
		}

		
		scanf(_sendBuf);
		ret = send(_sock, _sendBuf, strlen(_sendBuf), 0);
		if (ret > 0)
		{
			printf("send: s%s\n", _sendBuf);
		}
		
		memset(_receBuf, 0, sizeof(_receBuf));
		memset(_sendBuf, 0, sizeof(_sendBuf));
	}
	*/
	// 4 Close Socket
	closesocket(_sock);
	free(_receBuf);
	free(_sendBuf);

	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}