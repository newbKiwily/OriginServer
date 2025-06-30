#pragma once
#include "Utils.h"
struct Vector3 { float x = 0, y = 0, z = 0; };

enum class PacketType : uint8_t { POSITION_SYNC = 0, MOVE_INPUT = 1, POSITION_UPDATE = 2, LOGIN = 3, LOGOUT = 4, READY = 5, FINISH = 6, RESULT = 7};

#pragma pack(push, 1)
struct PacketHeader {
    PacketType type;
    uint16_t size;
    uint32_t playerId;
};

struct PakcetPositionSync {
    PacketHeader header;
    float posX, posY, posZ;
    float timestamp;
};

struct PacketPositionUpdate {
    PacketHeader header;
    float posX, posY, posZ;
    
    float angleDeg;
    
};

struct PacketLogin {
    PacketHeader header;
    uint32_t clientId;

    uint8_t validClients;
    uint32_t validClientIds[4] = { NULL };
   
};

struct PacketLogout {
    PacketHeader header;
};

struct PacketReady {

    PacketHeader header;
    uint32_t playerId;
    uint8_t spawnIndex;

};

struct PacketFinish {
    PacketHeader header;


};

struct PacketResult {
    PacketHeader header;
    uint32_t minute;
    uint32_t second;
};

#pragma pack(pop)


