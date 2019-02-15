#include <WinSock2.h>
#include <Windows.h>


int main()
{

	WORD ver = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(ver, &data);


	WSACleanup();
	return 0;
}