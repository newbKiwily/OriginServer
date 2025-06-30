#pragma once
#include "Utils.h"
#include "Packet.h"
#include "ServerManager.h"
class ClientManager
{
private:
	ClientManager() {}
	ClientManager(const ClientManager& c) {}
	ClientManager& operator=(const ClientManager& s) {}
	~ClientManager() {}

	vector<pair<SOCKET,uint32_t>> clientSockets;				//클라이언트 소켓 관리 컨테이너(0번째는 리슨소켓)
	unordered_map<uint32_t, Vector3> clientPositions;			//ID와 position을 묶어서 위치업데이트 시 사용
	unordered_map<uint32_t, float> lastTimestamps;				
	uint32_t nextPlayerId = 0;									//아이디 할당 시 카운트증가(랜덤으로 해도 무방)
	
public:
	static ClientManager& getInstance()
	{	
		
		static ClientManager instance;
		return instance;
	}
	void AddSocket(const SOCKET socket, const uint32_t id);
	
	
	SOCKET& AtSocket(const int index);

	uint32_t AtSocketId(const int index);

	uint32_t AssignId();

	void deleteSocket(const int id);

	void BroadCast(const char* data, size_t size);
	
	unordered_map<uint32_t, Vector3>& getClientPosition();

	unordered_map<uint32_t, float>& getLastTimestamps();

	uint32_t getClientSocketsSize();
	
	
};

