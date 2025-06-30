#include "WSAEventSelected.h"
#include "PacketMethod.h"

WSAEventSelected::WSAEventSelected():serverManager(ServerManager::getInstance()),clientManager(ClientManager::getInstance())
{
}


void WSAEventSelected::Initialize()
{  
    
    WSAStartup(MAKEWORD(2, 2), &wsaData);
    serverManager.Initialize();
    WSAEVENT listenEvent = WSACreateEvent();

    WSAEventSelect(serverManager.getListenSocket(), listenEvent, FD_ACCEPT);                        //리슨소켓을 서버로부터 받는다

    clientManager.AddSocket(serverManager.getListenSocket(),-1);                                    //리슨소켓도 소켓이기때문에 clientManager에서 관리
    clientEvents.push_back(listenEvent);                                                            //그러기때문에 clientManager의 소켓관리 컨테이너에서는 클라이언트수가 전체 길이-1임
    
    
} 

void WSAEventSelected::StartLoop()
{   
    
    while (true)
    {
        DWORD wait = WSAWaitForMultipleEvents(clientEvents.size(), clientEvents.data(), FALSE, 10, FALSE);
        if (wait == WSA_WAIT_TIMEOUT || wait == WSA_WAIT_FAILED)
            continue;

        //소켓의 인덱스를 계산
        size_t index =  wait- WSA_WAIT_EVENT_0;
        SOCKET sock = clientManager.AtSocket(index);

        WSANETWORKEVENTS ne;
        WSAEnumNetworkEvents(sock, clientEvents[index], &ne);

        if (index == 0 && (ne.lNetworkEvents & FD_ACCEPT)) 
        {   
            //최대 입장 가능 클라이언트는 4명
            if (clientManager.getClientSocketsSize() - 1 >= 4) 
            {
                std::cout << "최대 접속 수 초과. 새 클라이언트 거부." << std::endl;
                SOCKET tempClient = serverManager.Accept();
                if (tempClient != INVALID_SOCKET) {
                    closesocket(tempClient); 
                }
                continue;
            }

            SOCKET client = serverManager.Accept();
            if (!PacketMethod::Login(serverManager, clientManager, client))
                continue;

            WSAEVENT event = WSACreateEvent();
            WSAEventSelect(client, event, FD_READ | FD_CLOSE);

            clientEvents.push_back(event);

            start = PacketMethod::Ready(serverManager, clientManager);
                       
            continue;
        }

        if (ne.lNetworkEvents & FD_READ)
        {   
            //패킷헤더 파싱 후 어떤 패킷인지 파악
            char buf[512];
            int recvLen = recv(sock, buf, sizeof(PakcetPositionSync), 0);
            PacketHeader* header = (PacketHeader*)buf;

            switch (header->type)
            {
            case PacketType::FINISH:
            {                               
                PacketMethod::Finish(header, (PacketFinish*)buf, clientManager, start);
                break;
            }
            case PacketType::POSITION_SYNC:
            {               
                PacketMethod::SyncPosition(header, (PacketPositionUpdate*)buf, clientManager);
                break;
            }
            default:
                break;
            }            
        }

        if (ne.lNetworkEvents & FD_CLOSE)
        {   
            char buf[7];
            int recvLen = recv(sock, buf, sizeof(PacketLogout), 0);
            PacketHeader* header = (PacketHeader*)buf;

            //로그아웃 패킷이 오거나 비정상 종료로 인해 recvLen의 길이가 0 이하가 들어오면 로그아웃으로 간주

            if (recvLen <= 0 || header->type == PacketType::LOGOUT) 
            {           
                WSACloseEvent(clientEvents[index]);
                clientEvents.erase(clientEvents.begin() + index);

                PacketMethod::Logout(clientManager, index, sock);
                continue;
            }                              
        }
    }
}