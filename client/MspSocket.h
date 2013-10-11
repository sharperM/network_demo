#pragma once
#include <memory>
#include <string>
class MspSocket
{


private:
	class Impl;
	friend class Impl;
	std::auto_ptr<Impl> impl;
public:
	MspSocket(void);
	~MspSocket(void);
	DWORD WINAPI SendThread (PVOID pvParam/*string÷∏’Î*/);
	DWORD WINAPI recvThread (PVOID pvParam);
	DWORD WINAPI listenThread (PVOID pvParam);
};

