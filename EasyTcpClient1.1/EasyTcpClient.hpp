#ifndef __EasyTcpClient_H__

#define __EasyTcpClient_H__



#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include<WinSock2.h>

#include <Windows.h>



#pragma comment(lib,"ws2_32.lib")

#else

#include <arpa/inet.h>

#include <string.h>

#include <unistd.h>

#include <malloc.h>





#define SOCKET int

#define INVALID_SOCKET (SOCKET)(~0)

#define SOCKET_ERROR -1;

#endif // _WIN32



#include <stdio.h>

#include <memory>

#include <stdlib.h>

#include <thread>



#include "MesProtocol.hpp"


#define DATA_BUF_LEN 102400
#define MESSAGE_BUF_LEN 1024000

class EasyTcpClient

{

public:

	EasyTcpClient();

	virtual ~EasyTcpClient();

	int InitSocket();

	int Connect(const char* ip, int port);

	bool OnRun();

	bool IsRun()
	{
		return (_sock != INVALID_SOCKET);
	}

	void Closesocket();

	int RecvData(SOCKET _sock);

	int SendData(DataHeader* header);

private:

	void OnNetMsg(SOCKET _sock, DataHeader* header);

private:

	char* _dataRecv;

	SOCKET _sock;

	char* _dataMessge;

	int _pSit;

};



EasyTcpClient::EasyTcpClient() 
{

	_dataRecv = (char*)malloc(sizeof(char) * DATA_BUF_LEN);
	_dataMessge = (char*)malloc(sizeof(char)* MESSAGE_BUF_LEN);
	memset(_dataRecv, 0, DATA_BUF_LEN);
	memset(_dataMessge, 0, MESSAGE_BUF_LEN);

	_sock = INVALID_SOCKET;
	_pSit = 0;
}



EasyTcpClient::~EasyTcpClient()
{
	Closesocket();

	free(_dataRecv);
}



/*******************************************

* function introduced: InitSocket

* input parameter: NULL

* output parameter: NULL

* return: -1, is close; 0, is true

*******************************************/

int EasyTcpClient::InitSocket()
{
#ifdef _WIN32

	//Start Windwos Socket 2.x Environment

	WORD _ver = MAKEWORD(2, 2);

	WSADATA _data;

	WSAStartup(_ver, &_data);

#endif

	// 1 Build Socket
	if (INVALID_SOCKET != _sock)
	{
		printf("<socket = %d>close links\n", _sock);

		Closesocket();
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	if (INVALID_SOCKET == _sock)
	{
		printf("==ERROR==: build socket faild...\n");

		return -1;
	}
	else
	{
		printf("==SUCCESS==: build socket success...\n");
	}
	return 0;
}



/*******************************************

* function introduced: connect socket

* input parameter: const char* ip and int port

* output parameter: NULL

* return: NULL

*******************************************/
int  EasyTcpClient::Connect(const char* ip, int port)
{
	if (INVALID_SOCKET == _sock)
	{
		InitSocket();
	}
	// 2 Connect Server
	sockaddr_in _server = {};

	_server.sin_family = AF_INET;

	_server.sin_port = htons(port);

#ifdef _WIN32
	_server.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_server.sin_addr.s_addr = inet_addr(ip);
#endif
	if (-1 == connect(_sock, (sockaddr*)&_server, sizeof(_server)))
	{
		printf("==ERROR==: connect server faild...\n");

		Closesocket();

		return -1;
	}
	else
	{
		printf("==SUCCESS==: connect server success...\n");
	}
	return 0;
}



/*******************************************

* function introduced: Close client socket

* input parameter: NULL

* output parameter: NULL

* return: NULL

*******************************************/
void  EasyTcpClient::Closesocket()
{
	if (INVALID_SOCKET != _sock)
	{
		// 4 Close Socket
#ifdef _WIN32
		closesocket(_sock);
		//Clean Windows socket Envoronment
		WSACleanup();
#else
		close(_sock);
#endif // _WIN32
		_sock = INVALID_SOCKET;
	}
	return;
}



/*******************************************

* function introduced: Start Processor thread

* input parameter: NULL

* output parameter: NULL

* return: NULL

*******************************************/
bool EasyTcpClient::OnRun()
{
	if (IsRun())
	{
		fd_set _fdRead;

		FD_ZERO(&_fdRead);

		FD_SET(_sock, &_fdRead);

		timeval t = { 0,0 };

		//if sock not add 1, select recv't message in the linux
		int ret = select(_sock + 1, &_fdRead, 0, 0, &t);

		if (ret < 0)
		{
			Closesocket();

			printf("select() Error...\n");

			return false;
		}
		if (FD_ISSET(_sock, &_fdRead))
		{
			FD_CLR(_sock, &_fdRead);

			//DataHeader* _header = (DataHeader*)_dataRecv;
			if (-1 == RecvData(_sock))
			{
				printf("select() task over...\n");

				Closesocket();

				return false;
			}
		}
		return true;
	}
	return false;
}



/*******************************************

* function introduced: Receiving and analysis messages

* input parameter: SOCKET

* output parameter: NULL

* return: -1, is close; 0, is true

*******************************************/
int EasyTcpClient::RecvData(SOCKET _sock)
{
	// 3 Receive/Send Server Data
	int nLen = recv(_sock, (char*)_dataRecv, DATA_BUF_LEN, 0);
	if (nLen <= 0)
	{
		printf("Client close...\n");
		return nLen;
	}
	//printf("recv data length:%d\n", nLen);
	//2 level buffer pool(To be big enough, At least recv one full message)
	memcpy(_dataMessge + _pSit, _dataRecv, nLen);
	_pSit += nLen;
	//if 2 level buffer pool spill
	if (_pSit > MESSAGE_BUF_LEN)
	{
		printf("Error: memory overflow!!!\n");
	}
	//Stick package and packet disassembly processor
	while (_pSit >= sizeof(DataHeader))
	{
		DataHeader* header = (DataHeader*)_dataMessge;
		if (_pSit >= header->_dataLength)
		{
			int temp = header->_dataLength;
			OnNetMsg(_sock, header);
			memcpy(_dataMessge, _dataMessge + temp, _pSit - temp);
			_pSit -= temp;
		}
		else
		{
			break;
		}
	}
	return nLen;
}

/*******************************************

* function introduced: Process network messages

* input parameter: SOCKET and DataHeader*

* output parameter: NULL

* return:NULL

*******************************************/
void EasyTcpClient::OnNetMsg(SOCKET _sock, DataHeader* _header)
{
	switch (_header->_cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* _loginRe = (LoginResult*)_header;

		//printf("rece: CMD_LOGIN_RESULT %d\n", _loginRe->_result);
	}
	break;
	case CMD_LOGIN_OUT_RESULT:
	{
		LoginOutResult* _loginOutRe = (LoginOutResult*)_header;

		//printf("rece: CMD_LOGIN_OUT_RESULT %d\n", _loginOutRe->_result);
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* _Uers = (NewUserJoin*)_header;

		//printf("rece: New client: %d com here...\n", _Uers->_socke);
	}
	break;
	case CMD_ERROR:
	{
		printf("rece: CMD_ERROR!!! data length: %d\n", _header->_dataLength);
	}

	break;
	default:
		printf("receive: Unable to parse command!!!data length: %d\n", _header->_dataLength);
		break;

	}

}



/*******************************************

* function introduced: Send CMD messages

* input parameter: SOCKET

* output parameter: NULL

* return: -1, is close; 0, is true

*******************************************/
int EasyTcpClient::SendData(DataHeader* header)
{
	if (IsRun() && header)
	{
		int ret = send(_sock, (const char*)header, header->_dataLength, 0);
		//if (ret == EINTR || ret == EWOULDBLOCK || ret == EAGAIN)
		//{
		//	printf("SendData faild!!!\n");
		//}
		return ret;
	}
	return SOCKET_ERROR;
}

#endif // __EasyTcpClient_H__

