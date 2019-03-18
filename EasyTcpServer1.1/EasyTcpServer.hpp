#ifndef _EasyTcpServer_HPP__
#define _EasyTcpServer_HPP__



#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <WinSock2.h>
#include <Windows.h>
#pragma comment(lib,"ws2_32.lib")
#else
#include <unistd.h>
#include <arpa/inet.h>
#include <malloc.h>
#include <string.h>

#define SOCKET int
#define SOCKET_ERROR -1
#define INVALID_SOCKET (SOCKET)(~0)
#endif // WIN32

#include <stdio.h>
#include <stdlib.h>
#include <memory>
#include <vector>


#include "MesProtocol.hpp"
#define DATA_BUF_LEN 409600
#define MESSAGE_BUF_LEN 10240
class ClientSocket
{
public:
	ClientSocket(SOCKET socket);
	~ClientSocket();
	SOCKET GetSocket();
	char* GetDataBuf(int pSit);
	int GetPointSit();
	void SetPointSit(int pSit);
private:
	char* _dataMessage;
	SOCKET _cSocket;
	int _pSit;
};

ClientSocket::ClientSocket(SOCKET socket)
{
	_dataMessage = (char*)malloc(sizeof(char)*MESSAGE_BUF_LEN);
	SOCKET _cSocket = socket;
	int _pSit = 0;
}

ClientSocket::~ClientSocket()
{
	free(_dataMessage);
}
SOCKET ClientSocket::GetSocket()
{
	return _cSocket;
}
char* ClientSocket::GetDataBuf(int pSit = 0)
{
	return _dataMessage + pSit;
}
int ClientSocket::GetPointSit()
{
	return _pSit;
}
void ClientSocket::SetPointSit(int pSit)
{
	_pSit = pSit;
}
class EasyTcpServer
{
public:
	EasyTcpServer();
	virtual ~EasyTcpServer();
	int InitSocket();
	int Bind(const char *ip, int port);
	int Listen(int clientsNumb);
	bool OnRun();
	bool IsRun()
	{
		return (_sock != INVALID_SOCKET);
	}
	void CloseSocket(); 
private:
	SOCKET Accept();
	int RecvData(ClientSocket* client);
	int SendData(SOCKET _cSock, DataHeader* _header);
	int SendDataToAll(DataHeader* _header);
	void OnNetMsg(SOCKET _cSock, DataHeader* _header);
private:
	SOCKET _sock;
	SOCKET _cSock;
	std::vector<ClientSocket*> g_clients;
	char* _dataRecv;
};

EasyTcpServer::EasyTcpServer()
{
	_sock = INVALID_SOCKET;
	_cSock = INVALID_SOCKET;
	_dataRecv = (char*)malloc(sizeof(char) * DATA_BUF_LEN);
}

EasyTcpServer::~EasyTcpServer()
{
	free(_dataRecv);
	CloseSocket();
	g_clients.clear();
}
/*******************************************

* function introduced: InitSocket

* input parameter: NULL

* output parameter: NULL

* return: -1, is close; 0, is true

*******************************************/
int EasyTcpServer::InitSocket()
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
		CloseSocket();
	}
	_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (INVALID_SOCKET == _sock)
	{
		printf("==ERROR==: build socket faild...\n");
		return INVALID_SOCKET;
	}
	else
	{
		printf("==SUCCESS==: build socket success...\n");
	}
	return 0;


}
/*******************************************

* function introduced: bind server IP and Port

* input parameter: IP and Port

* output parameter: NULL

* return: -1, is close; 0, is true

*******************************************/
int EasyTcpServer::Bind(const char *ip = NULL, int port = 0)
{
	// 2 Bind Receive Client Data Port
	sockaddr_in _sin = {};
	_sin.sin_family = AF_INET;
#ifdef _WIN32
	if (ip )
	{
		//host to net
		//int to unsigned short
		_sin.sin_port = htons(port);
		//Only the connection local IP
		//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		// Accept All IP Conection
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);
	}
	else
	{
		_sin.sin_addr.S_un.S_addr = INADDR_ANY;
		_sin.sin_port = htons(4567);
	}
#else
	if (ip)
	{
		//host to net
		//int to unsigned short
		_sin.sin_port = htons(port);
		//Only the connection local IP
		//_sin.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
		// Accept All IP Conection
		_sin.sin_addr.s_addr = inet_addr(ip);	
	}
	else
	{
		_sin.sin_addr.s_addr = INADDR_ANY;
		_sin.sin_port = htons(4567);
	}
#endif
	if (SOCKET_ERROR == bind(_sock, (sockaddr*)&_sin, sizeof(_sin)))
	{
		printf("==ERROR==: bind IP faild!\n");
		return -1;
	}
	else
	{
		printf("==SUCCESS==: bind IP success!\n");
	}
	return 0;
}
/*******************************************

* function introduced: Set listen client number 

* input parameter: clientsNumb

* output parameter: NULL

* return: -1, is Error; 0, is true

*******************************************/
int EasyTcpServer::Listen(int clientsNumb)
{
	// 3 Listen data Port
	if (SOCKET_ERROR == listen(_sock, clientsNumb))
	{
		printf("==ERROR==: listen socket faild!\n");
		return SOCKET_ERROR;
	}
	else
	{
		printf("==SUCCESS==: listen socket success!\n");
	}
	return 0;
}
/*******************************************

* function introduced: Accept clients connetion server

* input parameter: NULL

* output parameter: NULL

* return: -1, is Error; 0, is true

*******************************************/
SOCKET EasyTcpServer::Accept()
{

	// 4 Accept Client Connect
	sockaddr_in _client = {};
	int _nAddrLen = sizeof(_client);
#ifdef _WIN32
	_cSock = accept(_sock, (sockaddr*)&_client, &_nAddrLen);
#else
	_cSock = accept(_sock, (sockaddr*)&_client, (socklen_t *)&_nAddrLen);
#endif
	if (INVALID_SOCKET == _cSock)
	{
		printf("==ERROR==: accept invalid Clinet!\n");
		return INVALID_SOCKET;
	}
	else
	{
		printf("==SUCCESS==: accept Clinet!\n");
		printf("==Clinet==: Socket: %d  IP : %s\n", (int)_cSock, inet_ntoa(_client.sin_addr));
		NewUserJoin user;
		user._socke = _cSock;
		SendDataToAll(&user);
		ClientSocket* temp = new ClientSocket(_cSock);
		g_clients.push_back(temp);
	}
	return _cSock;

}
/*******************************************

* function introduced: Receiving and analysis messages

* input parameter: SOCKET

* output parameter: NULL

* return: -1, is close; 0, is true

*******************************************/
int EasyTcpServer::RecvData(ClientSocket* client)
{
	int tempPSit = client->GetPointSit();
	SOCKET tempSocket = client->GetSocket();
	char* tempBuf = client->GetDataBuf();

	int nLen = recv(tempSocket, (char*)_dataRecv, DATA_BUF_LEN, 0);
	if (nLen <= 0)
	{
		printf("Client close...\n");
		return nLen;
	}
	memcpy(tempBuf + tempPSit, _dataRecv, nLen);
	tempPSit += nLen;
	while (tempPSit >= sizeof(DataHeader))
	{
		DataHeader* header = (DataHeader*)tempBuf;
		if (tempPSit >= header->_dataLength)
		{
			int tem = header->_dataLength;
			OnNetMsg(tempSocket, header);
			tempPSit -= tem;
			memcpy(tempBuf, tempBuf + tem, tempPSit);
		}
		else
		{
			break;
		}
	}
	client->SetPointSit(tempPSit);
	return nLen;
}
/*******************************************

* function introduced: Process network messages

* input parameter: SOCKET and DataHeader*

* output parameter: NULL

* return:NULL

*******************************************/
void EasyTcpServer::OnNetMsg(SOCKET _cSock, DataHeader* _header)
{
	switch (_header->_cmd)
	{
	case CMD_LOGIN:
	{
		Login *_login = (Login*)_header;
		printf("recv: CMD_LOGIN, DataLength: %d\n", _login->_dataLength);
		printf("UserName: %s, UserPassWord; %s\n", _login->_userName, _login->_userPassWord);

		//receive Login struct, you can check data is right
		LoginResult loginRe;
		SendData(_cSock, &loginRe);
	}
	break;
	case CMD_LOGIN_OUT:
	{
		LoginOut* _loginOut = (LoginOut*)_header;
		//receive Login struct, you can check data is right
		printf("recv: CMD_LOGIN_OUT, DataLength: %d\n", _loginOut->_dataLength);
		printf("UserName: %s \n", "admin");

		LoginOutResult loginOutRe;
		SendData(_cSock, &loginOutRe);
	}
	break;
	default:
	{
		printf("recv: Unable analysis CMD\n");
		DataHeader header;
		header._cmd = CMD_ERROR;
		header._dataLength = 0;
		SendData(_cSock, &header);
	}
	break;
	}
}
/*******************************************

* function introduced: Start Server Processor thread

* input parameter: NULL

* output parameter: NULL

* return: NULL

*******************************************/
bool EasyTcpServer::OnRun()
{
	//Berkeley sockets
	fd_set _fdRead;
	fd_set _fdWrite;
	fd_set _fdExp;

	//#define FD_ZERO(set) (((fd_set FAR *)(set))->fd_count=0)
	FD_ZERO(&_fdRead);
	FD_ZERO(&_fdWrite);
	FD_ZERO(&_fdExp);

	FD_SET(_sock, &_fdRead);
	FD_SET(_sock, &_fdWrite);
	FD_SET(_sock, &_fdExp);

	SOCKET maxSocket = _sock;

	timeval t = { 0,0 };

	//Determine if there is any data received
	for (int n = 0; n < (int)g_clients.size(); n++)
	{

		FD_SET(g_clients[n]->GetSocket(), &_fdRead);
		if (maxSocket < g_clients[n]->GetSocket())
		{
			//select first parameter must max socket in linux
			maxSocket = g_clients[n]->GetSocket();
		}
	}
	//nfds is range of sockets, Is not sokcets number
	//select first parameter must max socket in linux, But in WIN32, it doesn't matter what the value is
	int ret = select(maxSocket + 1, &_fdRead, &_fdWrite, &_fdExp, &t);
	if (ret < 0)
	{
		printf("select retuen value: %d...\n", ret);
		//CloseSocket();
		return false;
	}
	if (FD_ISSET(_sock, &_fdRead))
	{
		FD_CLR(_sock, &_fdRead);
		Accept();
	}
	for (int n = 0; n < (int)g_clients.size(); n++)
	{

		if (FD_ISSET(g_clients[n]->GetSocket(), &_fdRead))
		{
			if (-1 == RecvData(g_clients[n]))
			{
				auto iter = g_clients.begin() + n;
				if (iter != g_clients.end())
				{
					delete g_clients[n];
					g_clients.erase(iter);
				}
			}
		}
	}
	return false;
}
/*******************************************

* function introduced: Close clients and server socket

* input parameter: NULL

* output parameter: NULL

* return: NULL

*******************************************/
void EasyTcpServer::CloseSocket()
{
	if (_sock != INVALID_SOCKET)
	{
		// 6 Close Socket
		for (size_t n = 0; n < g_clients.size(); n++)
		{
#ifdef _WIN32
			closesocket(g_clients[n]->GetSocket());
			delete g_clients[n];
#else
			close(g_clients[n]->GetSocket());
			delete g_clients[n];
#endif
		}
#ifdef _WIN32
		closesocket(_sock);
		//Clean Windows socket Envoronment
		WSACleanup();
#else
		close(_sock);
#endif
	}
}
/*******************************************

* function introduced: Send CMD messages

* input parameter: SOCKET and DataHeader*

* output parameter: NULL

* return: -1, is Error; > 0, is true

*******************************************/
int EasyTcpServer::SendData(SOCKET _cSock, DataHeader* _header)
{
	if (IsRun() && _header)
	{
		return send(_cSock, (const char*)_header, _header->_dataLength, 0);
	}
	return SOCKET_ERROR;
}
/*******************************************

* function introduced: Send CMD messages To everyone

* input parameter: DataHeader*

* output parameter: NULL

* return: -1, is Error; 0, is true

*******************************************/
int EasyTcpServer::SendDataToAll(DataHeader* _header)
{
	if (IsRun() && _header)
	{
		for (int n = 0; n < g_clients.size(); n++)
		{
			SendData(g_clients[n]->GetSocket(), _header);
		}
		return 0;
	}
	return SOCKET_ERROR;
}
#endif // !_EasyTcpServer_HPP__
