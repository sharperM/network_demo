#include "StdAfx.h"
#include "MspSocket.h"
#include <WinSock.h>
#include <vector>
#include <string>
#include <iostream>

#pragma comment(lib, "Ws2_32.lib")

struct SendData
{
	std::string address;
	std::string data;
	unsigned short port;
}typedef SendData;

struct RecvData
{
	std::string data;
	SOCKET socket;
}typedef RecvData;

class MspSocket::Impl
{
private:
	static SOCKET m_sock;//监听socket
	DWORD dwListenThread;
	MspSocket*	_this;
public:
	Impl(MspSocket* theMspSocket)
	{
		_this = theMspSocket;
		m_sock = INVALID_SOCKET;
	}
	~Impl()
	{
		
	}

	static DWORD WINAPI SendThread (PVOID pvParam/*SendData指针*/)
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

		std::string sendbuf = pSData->data;

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
		re.push_back('\0');
		cout <<"re:" <<re << endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}


	static DWORD WINAPI recvThread (PVOID pvParam/*RecvData指针*/)
	{
		std::cout << "接收线程号:" << GetCurrentThreadId() <<"\n";
		SOCKET  sock =*(SOCKET *) pvParam;
		char buf[1] = {0};
		std::vector<char> result;
		int ret ;
		while((ret = recv(sock,(char*)buf,1,0/*MSG_OOB*/))>0)
		{
			result.push_back(*buf);
		}
		result.push_back('\0');
		if (ret<0)
		{
			wprintf(L"recv failed with error: %ld\n", WSAGetLastError());
		}else if (ret == 0)
		{
			if (result.size()>0)
				printf(":%s\n",&(result[0]));
		}
		send(sock,"OK",sizeof("OK"),0);
		shutdown(sock, SD_SEND);

		return ret;
	}


	static DWORD WINAPI listenThread (PVOID pvParam/*端口*/)
	{
		SOCKET sock;
		WORD wVersionRequested;
		WSADATA wsaData;
		unsigned short port  = *(unsigned short*)pvParam;
		wVersionRequested = MAKEWORD( 2, 0 );
		if (WSAStartup( wVersionRequested, &wsaData )!= NO_ERROR)
		{
			return 0;
		}
		struct sockaddr_in serverSockAddress;
		serverSockAddress.sin_family			= AF_INET;
		serverSockAddress.sin_port				= htons (port);
		serverSockAddress.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");

		if ((sock= socket(AF_INET, SOCK_STREAM, IPPROTO_TCP))==INVALID_SOCKET)
			return 0;
		if (bind(sock,(SOCKADDR*)&serverSockAddress,sizeof(serverSockAddress))== SOCKET_ERROR)
		{
			wprintf(L"bind failed with error: %ld\n", WSAGetLastError());
			return 0;
		}

		if (listen(sock,100)== SOCKET_ERROR)
			return 0;
		struct sockaddr_in clinetSockAddress = {0};
		int length  = sizeof(clinetSockAddress);
		SOCKET connetSocket;
		DWORD dwThreadId;
		while(INVALID_SOCKET!=(connetSocket = accept(sock, (SOCKADDR*)&clinetSockAddress,&length)))
		{
			SendData sd;
			sd.address = inet_ntoa(clinetSockAddress.sin_addr);
			ostringstream stm;
			stm << ntohs(clinetSockAddress.sin_port);
			sd.port = stm.str();

			printf("IP地址：%s:%d说：\n",inet_ntoa(clinetSockAddress.sin_addr),ntohs(clinetSockAddress.sin_port));
			HANDLE hThread = CreateThread(NULL,0,recvThread,(PVOID)&connetSocket, 0, &dwThreadId);

		}
		closesocket(sock);
		WSACleanup();
		return 0;
	}


	void initSocket(unsigned short localPort)
	{
		//监听线程
		CreateThread(NULL, 0, listenThread,PVOID(&localPort),0,&dwListenThread);
	}

};
MspSocket::MspSocket(void)
{
}


MspSocket::~MspSocket(void)
{
}

DWORD WINAPI MspSocket::SendThread( PVOID pvParam/*string指针*/ )
{

}
