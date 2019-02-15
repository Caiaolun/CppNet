
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
	CMD_EROOR
};

//Data struct header
struct DataHeader
{
	int _cmd;
	int _dataLength;
};

//Data struct body
struct Login
{
	char _userName[32];
	char _userPassWord[32];
};

//Return Command result
struct LoginResult
{
	int result;
};

//Data struct body
struct LoginOut
{
	char _userName[32];
};

//Return Command result
struct LoginOutResult
{
	int result;
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
	DataHeader* _header = (DataHeader*)malloc(sizeof(DataHeader));
	memset(_header, 0, sizeof(DataHeader));

	Login* _login = (Login*)malloc(sizeof(Login));
	memset(_login, 0, sizeof(Login));

	LoginOut* _loginOut = (LoginOut*)malloc(sizeof(LoginOut));
	memset(_loginOut, 0, sizeof(LoginOut));

	char* _write = (char*)malloc(sizeof(char) * 32);
	memset(_write, 0, _msize(_write));

	LoginResult* _loginRe = (LoginResult*)malloc(sizeof(LoginResult));
	memset(_loginRe, 0, sizeof(LoginResult));

	LoginOutResult* _loginOutRe = (LoginOutResult*)malloc(sizeof(LoginOutResult));
	memset(_loginOutRe, 0, sizeof(LoginOutResult));
	while (true)
	{

		scanf("%s", _write);

		if (0 == strcmp(_write, "exit.."))
		{
			printf("exit client\n");
			break;
		}
		else if(0 == strcmp(_write, "login"))
		{
			_header->_cmd = CMD_LOGIN;
			_header->_dataLength = sizeof(Login);
			memcpy(_login->_userName, "admin", strlen("admin"));
			memcpy(_login->_userPassWord, "admin123", strlen("admin123"));

			send(_sock, (const char*)_header, sizeof(DataHeader), 0);
			send(_sock, (const char*)_login, sizeof(Login), 0);
		}
		else if (0 == strcmp(_write, "loginOut"))
		{
			_header->_cmd = CMD_LOGIN_OUT;
			_header->_dataLength = sizeof(LoginOut);
			memcpy(_loginOut->_userName, "admin", strlen("admin"));


			send(_sock, (const char*)_header, sizeof(DataHeader), 0);
			send(_sock, (const char*)_loginOut, sizeof(LoginOut), 0);
		}
		else
		{
			printf("Unable to parse command\n");
			continue;
		}

		int ret = recv(_sock, (char*)_header, sizeof(DataHeader), 0);
		if (ret > 0)
		{
			switch (_header->_cmd)
			{
				case CMD_LOGIN_RESULT:
				{
					ret = recv(_sock, (char*)_loginRe, sizeof(LoginResult), 0);
					if (ret > 0)
					{
						printf("rece: return %d\n", _loginRe->result);
					}
				}
				break;
				case CMD_LOGIN_OUT_RESULT:
				{
					ret = recv(_sock, (char*)_loginOutRe, sizeof(LoginOutResult), 0);
					if (ret > 0)
					{
						printf("rece: return %d\n", _loginOutRe->result);
					}
				}
				break;
			default:
				printf("receive: Unable to parse command!\n");
				break;
			}
		}
	}

	// 4 Close Socket
	closesocket(_sock);


	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}