
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#endif // WIN32

#include <stdio.h>
#include <memory>

int main()
{
#ifdef _WIN32
	//Start Windwos Socket 2.x Environment
	WORD _ver = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_ver, &_data);

	// 1 Build Socket
	SOCKET _sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	// 2 Bind Receive Client Data Port
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
	//host to net
	//int to unsigned short
	_sin.sin_port = htons(4567);
	//Only the connection local IP
	//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	// Accept All IP Conection
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("==ERROR==: bind IP faild!\n");
	}
	else
	{
		printf("==SUCCESS==: bind IP success!\n");
	}

	// 3 Listen data Port
	if (SOCKET_ERROR == listen(_sock, 5))
	{
		printf("==ERROR==: listen socket faild!\n");
	}
	else
	{
		printf("==SUCCESS==: listen socket success!\n");
	}

	// 4 Accept Client Connect
	char _msgBuf[] = "Hello, I'm Server.";
	sockaddr_in _client = {};
	int _nAddrLen = sizeof(_client);
	SOCKET _cSock = INVALID_SOCKET;
	_cSock = accept(_sock, (sockaddr*)&_client, &_nAddrLen);
	if (INVALID_SOCKET == _cSock)
	{
		printf("==ERROR==: accept invalid Clinet!\n");
	}
	else
	{
		printf("==SUCCESS==: accept Clinet!\n");
		printf("==Clinet==: IP = %s\n", inet_ntoa(_client.sin_addr));
	}

	// 5 Send/Receive Data for Client
	char* _sendBuf = (char*)malloc(sizeof(char)*512);
	char* _receBuf = (char*)malloc(sizeof(char) * 512);
	memset(_receBuf, 0, _msize(_receBuf));
	memset(_sendBuf, 0, _msize(_sendBuf));
	//char _rectemo[512] = {};
	while (true)
	{
		//int ret = recv(_cSock, _rectemo, sizeof(_rectemo), 0);

		//Error : sizeof(_receBuf), sizeof(_receBuf) = 4
		//int ret = recv(_cSock, _receBuf, sizeof(_receBuf), 0);

		//Warning: Don't use sizeof in pointer, you can use _msize
		int ret = recv(_cSock, _receBuf, _msize(_receBuf), 0);
		if (ret <= 0)
		{
			printf("Client close...\n");
			break;
		}
		if (0 == strcmp(_receBuf, "getName"))
		{
			char _msgBuf[] = "XiaoMing.";
			send(_cSock, _msgBuf, strlen(_msgBuf) + 1, 0);
		}
		else if (0 == strcmp(_receBuf, "getAge"))
		{
			char _msgBuf[] = "80.";
			send(_cSock, _msgBuf, strlen(_msgBuf), 0);
		}
		else
		{
			char _msgBuf[] = "????-????-????-????.";
			send(_cSock, _msgBuf, strlen(_msgBuf), 0);
		}

		memset(_sendBuf, 0, _msize(_sendBuf));
		memset(_receBuf, 0, _msize(_receBuf));
	}


	// 6 Close Socket
	closesocket(_sock);
	free(_sendBuf);
	free(_receBuf);

	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}