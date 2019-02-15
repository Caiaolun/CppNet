
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>
#endif // WIN32

#include <stdio.h>
#include <memory>

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
		printf("==Clinet==: Socket: %d  IP : %s\n", (int)_cSock, inet_ntoa(_client.sin_addr));
	}

	// 5 Send/Receive Data for Client
	DataHeader* _header = (DataHeader*)malloc(sizeof(DataHeader));
	memset(_header, 0, sizeof(DataHeader));

	Login* _login = (Login*)malloc(sizeof(Login));
	memset(_login, 0, sizeof(Login));

	LoginOut* _loginOut = (LoginOut*)malloc(sizeof(LoginOut));
	memset(_loginOut, 0, sizeof(LoginOut));

	LoginResult* _loginRe = (LoginResult*)malloc(sizeof(LoginResult));
	memset(_loginRe, 0, sizeof(LoginResult));

	LoginOutResult* _loginOutRe = (LoginOutResult*)malloc(sizeof(LoginOutResult));
	memset(_loginOutRe, 0, sizeof(LoginOutResult));


	while (true)
	{
		int ret = recv(_cSock, (char*)_header, sizeof(DataHeader), 0);
		if (ret <= 0)
		{
			printf("Client close...\n");
			break;
		}
		printf("recv: CMD: %d, DataLength: %d\n", _header->_cmd, _header->_dataLength);
		
		switch (_header->_cmd)
		{
			case CMD_LOGIN:
			{
				recv(_cSock, (char*)_login, sizeof(Login), 0);

				printf("UserName: %s, UserPassWord; %s\n", _login->_userName, _login->_userPassWord);

				//receive Login struct, you can check data is right
				//if Login struct data is right, we can send result=0 for client
				_header->_cmd = CMD_LOGIN_RESULT;
				_header->_dataLength = sizeof(LoginResult);
				_loginRe->result = 0;

				send(_cSock, (const char*)_header, sizeof(DataHeader), 0);
				send(_cSock, (const char*)_loginRe, sizeof(LoginResult), 0);

				memset(_header, 0, sizeof(DataHeader));
				memset(_loginRe, 0, sizeof(LoginResult));
			}
			break;
			case CMD_LOGIN_OUT:
			{
				recv(_cSock, (char*)_loginOut, sizeof(Login), 0);
				//receive Login struct, you can check data is right
				printf("UserName: %s, \n", "admin");

				_header->_cmd = CMD_LOGIN_OUT_RESULT;
				_header->_dataLength = sizeof(LoginOutResult);
				_loginOutRe->result = 0;

				send(_cSock, (const char*)_header, sizeof(DataHeader), 0);
				send(_cSock, (const char*)_loginOutRe, sizeof(LoginOutResult), 0);

				memset(_header, 0, sizeof(DataHeader));
				memset(_loginOutRe, 0, sizeof(LoginOutResult));
			}
			break;
		default:
			_header->_cmd = CMD_EROOR;
			_header->_dataLength = 0;

			send(_cSock, (char*)_header, sizeof(DataHeader), 0);

			memset(_header, 0, sizeof(DataHeader));
			break;
		}
	}


	// 6 Close Socket
	closesocket(_sock);
	free(_header);
	free(_login);
	free(_loginOut);
	free(_loginRe);
	free(_loginOutRe);


	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}