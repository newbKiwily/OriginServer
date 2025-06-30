#pragma once
#include "Utils.h"
#include "ClientManager.h"
class ServerManager
{
private:
	ServerManager(){}
	ServerManager(const ServerManager& s){}
	ServerManager& operator=(const ServerManager& s){}
	~ServerManager(){}
	SOCKET listenSocket;
	sockaddr_in addr = {};

public:
	static ServerManager& getInstance()
	{
		static ServerManager instance;
		return instance;
	}
	void Initialize();

	SOCKET& getListenSocket();

	int Send(SOCKET s, const char* buf, int len, int flags);		//객체 기능 분리를 위해 Send,Accept함수 생성
	SOCKET Accept();

};

