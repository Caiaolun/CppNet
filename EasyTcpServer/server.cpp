
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#endif // WIN32

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
	sockaddr_in _client = {};
	int _nAddrLen = sizeof(_client);
	SOCKET _cSock = INVALID_SOCKET;
	while (true)
	{
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

	// 5 Send Data for Client
		char _msgBuf[] = "Hello, I'm Server.";
		int ret = send(_cSock, _msgBuf, strlen(_msgBuf) + 1, 0);
		if (ret > 0)
		{
			printf("==Server==: send data length is %d\n", ret);
		}
	}


	// 6 Close Socket
	closesocket(_sock);

	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32


	return 0;
}