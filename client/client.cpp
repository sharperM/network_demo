// client.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <sstream>
#include <ios>
#include <vector>
#include <string>
#include <fstream>

#include "Data_def.h"

#pragma comment(lib, "Ws2_32.lib")

using namespace std;

DWORD WINAPI SendThread (PVOID pvParam/*string指针*/);




struct te
{
	int i1;
	int i2;
	char i3[64];
	char i4[256];
	int i5;
	int i6;
	int i7;
	int i8;
	char i9[24];
	int i10;
	int i11;
	int i12;
	char i13[256];
	float i14;
	char i15[256];
	float i16;
	char i17[256];
	float i18;
	char i19[256];
	float i20;
	char i21[256];
	float i22;
};


int _tmain(int argc, _TCHAR* argv[])
{

	int i= sizeof(struct te);

	std::string en = "yi";
	std::string cn = "没有";
	char aa[21] ={0};
	int tee =strlen(en.c_str());
	int tee1 = strlen (cn.c_str());
	int tee2 = sizeof (aa);

	std::cout << "port:";
	DWORD dwThreadId = 0;

	SendData data;
	data.address = "127.0.0.1";
	std::cin >> data.port;


	std::string in;
	while(true)
	{
		getline(std::cin, in);
		char buf[1024];
		WCHAR wbuf[512];
		ifstream ifile(in.c_str(),ios::binary);
		if (ifile.is_open())
		{
			int c = 0;
			 while (c = ifile.read(buf,1024))
			 {
				data.data.resize(c,"\0");
				memcpy(&(data.data[0]),buf,c);
			 }
			 = buf;
		}
		ifile.close();
		CreateThread(NULL,0,SendThread,(PVOID)&data, 0, &dwThreadId);
	}
	return 0;
}

DWORD WINAPI SendThread (PVOID pvParam/*string指针*/)
{
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	SendData* pSData  = (SendData*)pvParam;

	struct addrinfo *result = NULL, hints;
	ZeroMemory( &hints, sizeof(hints) );
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	getaddrinfo(pSData->address.c_str(), pSData->port.c_str(), &hints, &result);

	SOCKET ConnectSocket;
	if ((ConnectSocket= socket(result->ai_family, result->ai_socktype, result->ai_protocol))==INVALID_SOCKET)
	{
		closesocket(ConnectSocket);
		WSACleanup();
		return 0;
	}

	iResult = connect( ConnectSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		closesocket(ConnectSocket);
		WSACleanup();
		return 0;
	}
	freeaddrinfo(result);

	std::string sendbuf = &(pSData->data[0]);
	
	iResult = send( ConnectSocket, sendbuf.c_str(), sendbuf.size(), 0 );
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}
	cout<<"send:" <<sendbuf <<"to"<< pSData->address << endl;
	
	shutdown(ConnectSocket, SD_SEND);

	char recvbuf;
	string re;
	do 
	{
		iResult = recv(ConnectSocket, &recvbuf, 1, 0);
		re.push_back(recvbuf);
	} while (iResult>0);
	cout <<"re:" <<re << endl;
	closesocket(ConnectSocket);
	WSACleanup();
	return 1;
}



