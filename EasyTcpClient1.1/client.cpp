
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include<WinSock2.h>
#include <Windows.h>
#endif // _WIN32

#include <stdio.h>
#include <memory>
#include <stdlib.h>



struct DataPackage
{
	int age;
	char name[32];
};

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

	DataPackage* _pack = (DataPackage*)malloc(sizeof(DataPackage));
	memset(_pack, 0, _msize(_pack));
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

		int ret = recv(_sock, (char*)_pack, _msize(_pack), 0);
		if (ret > 0)
		{	
			printf("rece: age: %d  name: %s\n", _pack->age, _pack->name);
		}

		memset(_receBuf, NULL, _msize(_receBuf));
		memset(_sendBuf, NULL, _msize(_sendBuf));
		memset(_pack, 0, _msize(_pack));
	}

	// 4 Close Socket
	closesocket(_sock);
	free(_receBuf);
	free(_sendBuf);
	free(_pack);

	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}