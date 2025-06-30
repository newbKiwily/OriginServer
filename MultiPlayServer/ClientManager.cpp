#include "ClientManager.h"

void ClientManager::BroadCast(const char* data, size_t size)
{
	ServerManager& serverManager = ServerManager::getInstance();

	for (auto s : clientSockets)
	{
		serverManager.Send(s.first, data, size, 0);		
	}
}

void ClientManager::AddSocket(const SOCKET socket, const uint32_t id)
{
	clientSockets.push_back({ socket,id });
	return;
}

SOCKET& ClientManager::AtSocket(const int index) 
{
	return clientSockets.at(index).first;
}

uint32_t ClientManager::AtSocketId(const int index)
{
	return clientSockets.at(index).second;
}

uint32_t ClientManager::getClientSocketsSize()
{
	return (uint32_t)clientSockets.size();
}

void ClientManager::deleteSocket(const int id)
{
	for (auto it = clientSockets.begin(); it != clientSockets.end(); it++)
	{
		if ((*it).second == id)
		{
			clientSockets.erase(it);
			return;
		}
	}
}

uint32_t ClientManager::AssignId()
{
	if (nextPlayerId == 0)
		nextPlayerId = 1;
	else
		nextPlayerId += 1;
	return nextPlayerId;
}
unordered_map<uint32_t, Vector3>& ClientManager::getClientPosition()
{
	return clientPositions;
}

unordered_map<uint32_t, float>& ClientManager::getLastTimestamps()
{
	return lastTimestamps;
}
