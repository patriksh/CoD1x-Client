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

DWORD WINAPI CoD1X(LPVOID);
DWORD g_threadID;
HMODULE g_hModule;

void* X_Sys_LoadDll(const char*, char*, int(**) (int, ...), int (*)(int, ...));
void DLLInit();
void X_CL_Frame(int);
char* __cdecl X_ClearHostname(char*, const char*, size_t);
int X_Com_ReadCDKey(const char*);