#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <chrono>
#include <set>
using namespace std;
#pragma comment(lib, "ws2_32.lib")