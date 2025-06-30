#pragma once
#include "Utils.h"
#include "Packet.h"
#include "ClientManager.h"
class PacketMethod
{

public:
	static void UpdatePosition(PacketHeader* header, PakcetPositionSync* p,ClientManager& clientManager);

	static void SyncPosition(PacketHeader* header, PacketPositionUpdate* p, ClientManager& clientManager);

	static void Finish(PacketHeader* header, PacketFinish* p, ClientManager& clientManager, const chrono::steady_clock::time_point start);

	static bool Login(ServerManager& serverManager,ClientManager& clientManager,SOCKET client);

	static void Logout(ClientManager& clientManager,const size_t index,SOCKET sock);

	static chrono::steady_clock::time_point Ready(ServerManager& serveManager,ClientManager& clientManager);
	

};

