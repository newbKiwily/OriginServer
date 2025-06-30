#include "ServerManager.h"

void ServerManager::Initialize()
{
    listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(9000);

    bind(listenSocket, (sockaddr*)&addr, sizeof(addr));

    listen(listenSocket, SOMAXCONN);
    
    std::cout << "서버 시작됨. 포트 9000" << std::endl;

}

SOCKET& ServerManager::getListenSocket()
{
    return listenSocket;
}

int ServerManager::Send(SOCKET s, const char* buf, int len, int flags)
{
   return send(s, buf, len, flags);
}

SOCKET ServerManager::Accept()
{
    return accept(listenSocket, nullptr, nullptr);
}