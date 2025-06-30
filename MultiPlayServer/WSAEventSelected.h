#pragma once
#include "Utils.h"
#include "ServerManager.h"
#include "ClientManager.h"
#include "Packet.h"

class WSAEventSelected
{
private:
	
	WSADATA wsaData;

	ServerManager& serverManager;						// 서버 관리 객체
	ClientManager& clientManager;						// 클라이언트 관리 객체
	vector<WSAEVENT> clientEvents;						// 클라이언트 이벤트 관리 컨테이너
	chrono::steady_clock::time_point start;				// 시간측정용 변수
	
public:
	WSAEventSelected();
	~WSAEventSelected(){}

	void Initialize();

	void StartLoop();
	
};

