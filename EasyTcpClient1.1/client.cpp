
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include<WinSock2.h>
#include <Windows.h>
#endif // _WIN32

#include <stdio.h>
#include <memory>
#include <stdlib.h>


//Command
enum CMD
{
	CMD_LOGIN,
	CMD_LOGIN_RESULT,
	CMD_LOGIN_OUT,
	CMD_LOGIN_OUT_RESULT,
	CMD_NEW_USER_JOIN,
	CMD_ERROR
};

//Data struct header
struct DataHeader
{
	int _cmd;
	int _dataLength;
	int _maxDataLength;
};

//Data struct body
struct Login : public DataHeader
{
	Login()
	{
		_dataLength = sizeof(Login);
		_cmd = CMD_LOGIN;
	}
	char _userName[32];
	char _userPassWord[32];
};

//Return Command result
struct LoginResult : public DataHeader
{
	LoginResult()
	{
		_dataLength = sizeof(LoginResult);
		_cmd = CMD_LOGIN_RESULT;
		//if Login struct data is right, we can send result=0 for client
		_result = 0;
	}
	int _result;
};

//Data struct body
struct LoginOut : public DataHeader
{
	LoginOut()
	{
		_dataLength = sizeof(LoginOut);
		_cmd = CMD_LOGIN_OUT;
	}
	char _userName[32];
};

//Return Command result
struct LoginOutResult : public DataHeader
{
	LoginOutResult()
	{
		_dataLength = sizeof(LoginOutResult);
		_cmd = CMD_LOGIN_OUT_RESULT;
		//if Login struct data is right, we can send result=0 for client
		_result = 0;
	}
	int _result;
};
struct NewUserJoin : public DataHeader
{
	NewUserJoin()
	{
		_dataLength = sizeof(NewUserJoin);
		_cmd = CMD_NEW_USER_JOIN;
		//if Login struct data is right, we can send result=0 for client
		_socke = 0;
	}
	int _socke;
};

Login* _login = new Login();

LoginOut* _loginOut = new LoginOut();

char* _dataRecv = (char*)malloc(sizeof(char) * 4096);

int processor(SOCKET _sock)
{
	int nLen = recv(_sock, (char*)_dataRecv, sizeof(DataHeader), 0);
	DataHeader* _header = (DataHeader*)_dataRecv;

	printf("recv: DataHeader, DataLength: %d\n", _header->_dataLength);

	if (nLen <= 0)
	{
		printf("Client close...\n");
		return -1;
	}

	switch (_header->_cmd)
	{
		int ret = 0;
		switch (_header->_cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			ret = recv(_sock, (char*)_dataRecv + sizeof(DataHeader), _header->_dataLength - sizeof(DataHeader), 0);
			LoginResult* _loginRe = (LoginResult*)_dataRecv + sizeof(DataHeader);
			if (ret > 0)
			{
				printf("rece: return %d\n", _loginRe->_result);
			}
		}
		break;
		case CMD_LOGIN_OUT_RESULT:
		{
			ret = recv(_sock, (char*)_dataRecv + sizeof(DataHeader), sizeof(LoginOutResult) - sizeof(DataHeader), 0);
			LoginOutResult* _loginOutRe = (LoginOutResult*)_dataRecv + sizeof(DataHeader);
			if (ret > 0)
			{
				printf("rece: return %d\n", _loginOutRe->_result);
			}
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			ret = recv(_sock, (char*)_dataRecv + sizeof(DataHeader), sizeof(NewUserJoin) - sizeof(DataHeader), 0);
			NewUserJoin* _Uers = (NewUserJoin*)_dataRecv + sizeof(DataHeader);
			if (ret > 0)
			{
				printf("rece: New client: %d com here...\n", _Uers->_socke);
			}
		}
		break;
		default:
			printf("receive: Unable to parse command!\n");
			break;
		}
	}
	memset(_dataRecv, 0, _msize(_dataRecv));
}


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


	while (true)
	{


		fd_set _fdRead;
		FD_ZERO(&_fdRead);
		FD_SET(_sock, &_fdRead);
		//timeval t = { 0,0 };

		int ret = select(_sock, &_fdRead, 0, 0, 0);
		if (ret < 0)
		{
			printf("select() Error...\n");
			break;
		}
		if (FD_ISSET(_sock, &_fdRead))
		{
			FD_CLR(_sock, &_fdRead);
			int ret = recv(_sock, (char*)_dataRecv, sizeof(DataHeader), 0);
			DataHeader* _header = (DataHeader*)_dataRecv;
			if (-1 == processor(_sock))
			{
				printf("select() task over...\n");
				break;
			}
		}
		memset(_login->_userName, 0, sizeof(_login->_userName));
		memset(_login->_userPassWord, 0, sizeof(_login->_userPassWord));

		memcpy(_login->_userName, "ollen", strlen("ollen"));
		memcpy(_login->_userPassWord, "123456", strlen("123456"));
		send(_sock, (const char*)_login, _msize(_login), 0);

		Sleep(4000);
	}

	// 4 Close Socket
	closesocket(_sock);


	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}