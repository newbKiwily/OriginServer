#include "PacketMethod.h"

void PacketMethod::UpdatePosition(PacketHeader* header, PakcetPositionSync* p, ClientManager& clientManager)
{
	if (p->header.playerId == 0)
		return;

	float speed = 5.0f;
	Vector3& pos = clientManager.getClientPosition()[p->header.playerId];
	auto horizontal= pos.x;
	auto vertical = pos.y;
	float lastTimestamp = 0.0f;

	if (clientManager.getLastTimestamps().find(p->header.playerId) != clientManager.getLastTimestamps().end())
		lastTimestamp = clientManager.getLastTimestamps()[p->header.playerId];

	//속도 보간
	float deltaTime = p->timestamp - lastTimestamp;
	if (deltaTime < 0 || deltaTime > 1.0f)
		deltaTime = 0.05f;

	pos.x += p->posX * speed * deltaTime;
	pos.y += p->posY * speed * deltaTime;

	clientManager.getLastTimestamps()[p->header.playerId] = p->timestamp;

	//자동차는 방향에 따라서 회전하기 때문에 회전값을 서버에서 계산하여 보내줘야 함
	float angleDeg = atan2f(-vertical, -horizontal) * (180.0f / 3.14159265f);
	

	PacketPositionUpdate update = {
		{ PacketType::POSITION_UPDATE, sizeof(PacketPositionUpdate), p->header.playerId },
		pos.x, pos.y, pos.z,
		angleDeg
		
	};

	clientManager.BroadCast((char*)&update, sizeof(update));
}

void PacketMethod::SyncPosition(PacketHeader* header, PacketPositionUpdate* p, ClientManager& clientManager)
{
	if(p->header.playerId == 0)							//id==0은 리슨소켓이므로 무시
		return;

	Vector3& pos = clientManager.getClientPosition()[p->header.playerId];
	pos.x = p->posX;
	pos.y = p->posY;
	pos.z = p->posZ;

	float angleDeg = p->angleDeg;

	PacketPositionUpdate update = {
		{ PacketType::POSITION_UPDATE, sizeof(PacketPositionUpdate), p->header.playerId },
		pos.x, pos.y, pos.z,
		angleDeg
	};

	clientManager.BroadCast((char*)&update, sizeof(update));
}

void PacketMethod::Finish(PacketHeader* header, PacketFinish* p, ClientManager& clientManager, const chrono::steady_clock::time_point start)
{
	//Finish 패킷을 보낼 때는 해당 클라이언트가 도착한 시간을 계산하여 보내줌
	chrono::steady_clock::time_point end = chrono::steady_clock::now();
	auto duration = chrono::duration_cast<std::chrono::seconds>(end - start);
	uint32_t total_seconds = duration.count();
	uint32_t minutes = total_seconds / 60;
	uint32_t seconds = total_seconds % 60;

	PacketResult result = { PacketType::RESULT,sizeof(PacketResult),p->header.playerId,minutes,seconds };
	clientManager.BroadCast((char*)&result, sizeof(result));

	if (p->header.playerId == 0)
		return;
	//도착 시 초기 위치로 이동시킴
	Vector3& pos = clientManager.getClientPosition()[p->header.playerId];
	pos.x = 0;
	pos.y = 0;
	pos.z = 0;

	clientManager.getLastTimestamps()[p->header.playerId] = 0;

	PacketPositionUpdate update =
	{
		{ PacketType::POSITION_UPDATE, sizeof(PacketPositionUpdate), p->header.playerId },
			pos.x, pos.y, pos.z,0
	};

	clientManager.BroadCast((char*)&update, sizeof(update));	
}

bool PacketMethod::Login(ServerManager& serverManager, ClientManager& clientManager,SOCKET client)
{
	if (client == INVALID_SOCKET)
		return false;

	cout << "클라이언트 연결됨." << std::endl;

	uint32_t assignedId = clientManager.AssignId();

	clientManager.AddSocket(client, assignedId);

	//로그인 패킷을 보냄과 동시에 서버 내의 클라이언트 관리소켓에도 등록하여 동기화시켜야함
	PacketLogin loginPacket = { PacketType::LOGIN,sizeof(PacketLogin),assignedId,loginPacket.validClients = clientManager.getClientSocketsSize() - 1 };
	for (size_t i = 1; i < clientManager.getClientSocketsSize(); i++)
	{
		uint32_t otherId = clientManager.AtSocketId(i);
		if (otherId != assignedId) { // 본인 제외
			loginPacket.validClientIds[i - 1] = otherId;
		}
	}
	//먼저 로그인 요청을 보낸 클라이언트에게 자신의 정보를 전송
	serverManager.Send(client, (char*)&loginPacket, sizeof(loginPacket), 0);
	//그 후 모든 클라이언트에게 접속 알림
	clientManager.BroadCast((char*)&loginPacket, sizeof(loginPacket));

	return true;
}

chrono::steady_clock::time_point PacketMethod::Ready(ServerManager& serverManager,ClientManager& clientManager)
{	
	//만약 클라이언트 사이즈가 5명(리슨소켓 포함)이 되었다면 Ready로 간주하고 게임 시작
	if (clientManager.getClientSocketsSize() - 1 == 4)
	{
		uint8_t spawnindex = 0;
		for (int i = 1; i < clientManager.getClientSocketsSize(); i++)
		{
			PacketReady readyPacket = { PacketType::READY,sizeof(PacketReady),clientManager.AtSocketId(i),spawnindex };
			serverManager.Send(clientManager.AtSocket(i), (char*)&readyPacket, sizeof(readyPacket), 0);
			spawnindex += 1;
		}

		//시간 측정 시작
		auto start = chrono::steady_clock::now();
		return start;
	}
	return chrono::steady_clock::time_point{};
}

void PacketMethod::Logout(ClientManager& clientManager,const size_t index,SOCKET sock)
{
	uint32_t targetId = clientManager.AtSocketId(index);

	PacketLogout logoutPacket = { PacketType::LOGOUT, sizeof(PacketLogout), targetId };

	closesocket(sock);
	clientManager.deleteSocket(targetId);
	clientManager.BroadCast((char*)&logoutPacket, sizeof(logoutPacket));

	cout << "클라이언트 연결 종료됨" << std::endl;
}
