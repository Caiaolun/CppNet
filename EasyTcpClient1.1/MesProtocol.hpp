#ifndef _MesProtocol_H__
#define _MesProtocol_H__



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
	char _data[1024];
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

#endif // _MesProtocol_H__
