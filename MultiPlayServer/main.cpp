#include "Utils.h"
#include "ServerManager.h"
#include "WSAEventSelected.h"
#include "ClientManager.h"

int main() {
    WSAEventSelected* wsaEventSelected = new WSAEventSelected;
    wsaEventSelected->Initialize();
    wsaEventSelected->StartLoop();
    WSACleanup();
    return 0;
}