#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS

using namespace std;

#include <windows.h>
#include <process.h>
#include <stdio.h>
#include "functions.hpp"
#include "cracking.hpp"
#include "discord.hpp"
#include "curl.hpp"

class COD {
    public:
        COD();
    };
COD* game;

DWORD WINAPI tExtend(LPVOID);
DWORD g_threadID;
HMODULE g_hModule;

void DLLInit();

template <typename T, typename ... Ts> T call(size_t addr, Ts ... ts);
template <typename T, typename ... Ts>
T call(size_t addr, Ts ... ts) {
    T(*f)(...);
    *(T*)&f = (T)addr;
    return f(ts...);
}