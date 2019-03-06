
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <Windows.h>

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

std::vector<SOCKET> g_clients;

NewUserJoin* _newUserJoin = new NewUserJoin();

//LoginResult* _loginRe = new LoginResult();

LoginOutResult* _loginOutRe = new LoginOutResult();

char* _dataRecv = (char*)malloc(sizeof(char) * 4096);


int processor(SOCKET _cSock)
{
	int nLen = recv(_cSock, (char*)_dataRecv, sizeof(DataHeader), 0);
	DataHeader* _header = (DataHeader*)_dataRecv;

	printf("recv: DataHeader, DataLength: %d\n", _header->_dataLength);

	if (nLen <= 0)
	{
		printf("Client close...\n");
		return -1;
	}

	/*
	//Determine whether the data is larger than the data header
	if (nLen >= sizeof(DataHeader))
	{
	//Stick package problem
	}
	*/
	switch (_header->_cmd)
	{
	case CMD_LOGIN:
	{
		recv(_cSock, (char*)_dataRecv + sizeof(DataHeader), _header->_dataLength - sizeof(DataHeader), 0);
		Login* _login = (Login*)_dataRecv;
		printf("recv: CMD_LOGIN, DataLength: %d\n", _login->_dataLength);
		printf("UserName: %s, UserPassWord; %s\n", _login->_userName, _login->_userPassWord);

		//receive Login struct, you can check data is right
		LoginResult _loginRe;
		send(_cSock, (const char*)&_loginRe, sizeof(LoginResult), 0);

		memset(_header, 0, sizeof(DataHeader));
	}
	break;
	case CMD_LOGIN_OUT:
	{
		recv(_cSock, (char*)_dataRecv + sizeof(DataHeader), sizeof(LoginOut) - sizeof(DataHeader), 0);
		LoginOut* _loginOut = (LoginOut*)_dataRecv;
		//receive Login struct, you can check data is right
		printf("recv: CMD_LOGIN_OUT, DataLength: %d\n", _loginOut->_dataLength);
		printf("UserName: %s \n", "admin");

		send(_cSock, (const char*)_loginOutRe, sizeof(LoginOutResult), 0);

		memset(_header, 0, sizeof(DataHeader));
	}
	break;
	default:
	{
		printf("recv: Unable analysis CMD\n");
		_header->_cmd = CMD_ERROR;
		_header->_dataLength = 0;

		send(_cSock, (char*)_header, sizeof(DataHeader), 0);

		memset(_header, 0, sizeof(DataHeader));
	}
	break;
	}
#ifdef _WIN32
	memset(_dataRecv, 0, _msize(_dataRecv));
#else
	memset(_dataRecv, 0, 4096);
#endif
}


int main()
{
#ifdef _WIN32
	//Start Windwos Socket 2.x Environment
	WORD _ver = MAKEWORD(2, 2);
	WSADATA _data;
	WSAStartup(_ver, &_data);
#endif
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
#ifdef _WIN32
	_sin.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	_sin.sin_addr.s_addr = INADDR_ANY;
#endif
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

	while (true)
	{
		//Berkeley sockets

		/********************************
		typedef struct fd_set
		{
		u_int fd_count;               // how many are SET?
		SOCKET  fd_array[FD_SETSIZE];  // an array of SOCKETs
		} fd_set;
		********************************/
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
		/********************************
		* I made a mistake here
		* g_clients[n] not additon _fdread
		for (int n = (int)g_clients.size() -1; n > 0; n--)
		{

		FD_SET(g_clients[n], &_fdRead);
		}
		**********************************/
		//Determine if there is any data received
		for (int n = 0; n < (int)g_clients.size(); n++)
		{

			FD_SET(g_clients[n], &_fdRead);
			if (maxSocket < g_clients[n])
			{
				//select first parameter must max socket in linux
				maxSocket = g_clients[n];
			}
		}
		//nfds is range of sockets, Is not sokcets number
		/************************
		select(
		_In_ int nfds,
		_Inout_opt_ fd_set FAR * readfds,
		_Inout_opt_ fd_set FAR * writefds,
		_Inout_opt_ fd_set FAR * exceptfds,
		_In_opt_ const struct timeval FAR * timeout
		);
		**************************/
		//select first parameter must max socket in linux, But in WIN32, it doesn't matter what the value is
		int ret = select(maxSocket + 1, &_fdRead, &_fdWrite, &_fdExp, &t);
		if (ret < 0)
		{
			printf("select task over...\n");

			break;
		}
		if (FD_ISSET(_sock, &_fdRead))
		{
			FD_CLR(_sock, &_fdRead);

			// 4 Accept Client Connect
			sockaddr_in _client = {};
			int _nAddrLen = sizeof(_client);
			SOCKET _cSock = INVALID_SOCKET;
#ifdef _WIN32
			_cSock = accept(_sock, (sockaddr*)&_client, &_nAddrLen);
#else
			_cSock = accept(_sock, (sockaddr*)&_client, (socklen_t *)&_nAddrLen);
#endif
			if (INVALID_SOCKET == _cSock)
			{
				printf("==ERROR==: accept invalid Clinet!\n");
			}

			printf("==SUCCESS==: accept Clinet!\n");
			printf("==Clinet==: Socket: %d  IP : %s\n", (int)_cSock, inet_ntoa(_client.sin_addr));

			for (int n = 0; n < (int)g_clients.size(); n++)
			{
				NewUserJoin _userjoin;
				_userjoin._socke = _cSock;
				send(g_clients[n], (const char*)&_userjoin, sizeof(NewUserJoin), 0);
			}

			g_clients.push_back(_cSock);


		}
		for (int n = 0; n < (int)g_clients.size(); n++)
		{

			if (FD_ISSET(g_clients[n], &_fdRead))
			{
				if (-1 == processor(g_clients[n]))
				{
					auto iter = g_clients.begin();
					if (iter != g_clients.end())
					{
						g_clients.erase(iter);
					}
				}
			}
		}

		/**********************************
		* Can only be used _fdRead.fd_count on WIN32
		*
		*
		for (size_t n = 0; n < _fdRead.fd_count; n++)
		{
		// 5 Send/Receive Data for Client
		if (-1 == processor(_fdRead.fd_array[n]))
		{
		auto iter = find(g_clients.begin(), g_clients.end(), _fdRead.fd_array[n]);
		if (iter != g_clients.end())
		{
		g_clients.erase(iter);
		}
		}
		}
		***********************************/
		//printf("Wait for the task...\n");
	}
	// 6 Close Socket
	for (size_t n = 0; n < g_clients.size(); n++)
	{
#ifdef _WIN32
		closesocket(g_clients[n]);
#else
		close(g_clients[n]);
#endif
	}

#ifdef _WIN32
	closesocket(_sock);
#else
	close(_sock);
#endif
	//free(_loginRe);
	free(_loginOutRe);
	//free(_dataRecv);
#ifdef _WIN32
	//Clean Windows socket Envoronment
	WSACleanup();
#endif // _WIN32

	getchar();
	return 0;
}