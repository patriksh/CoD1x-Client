#pragma once
#include "main.hpp"

void* Sys_LoadDll(const char* name, char* fqpath, int(**entryPoint) (int, ...), int (*systemcalls)(int, ...)) {
	void* l = call<void*, const char*, char*, int(**)(int, ...)>(0x4633A0, name, fqpath, entryPoint, systemcalls);
	DLLInit();
	return l;
}

void DLLInit() {
	// cgame_mp = (int)GetModuleHandle("cgame_mp_x86.dll");
}

void X_CL_Frame(int msec) {
	CL_Frame(msec);
	X_Discord_Frame();
	X_Download_Frame();
}

char* __cdecl X_ClearHostname(char* a1, const char* a2, size_t a3) {
	std::string hostname(a2);
	hostname = trim(clearSymbols(hostname));
	if (hostname == "") hostname = "Unnamed Server";

	return strncpy(a1, hostname.c_str(), a3);
}

int X_Com_ReadCDKey(const char* filename) {
	return false;
}

COD::COD() {
	cracking_hook_call(0x460399, (int)Sys_LoadDll);
	cracking_hook_call(0x43822C, (int)X_CL_Frame);
	cracking_hook_call(0x412A2C, (int)X_ClearHostname);
	cracking_hook_function(0x436A40, (int)X_Com_ReadCDKey);
	cracking_hook_call(0x410316, (int)X_CL_NextDownload);
	cracking_hook_call(0x410376, (int)X_CL_NextDownload);
	cracking_hook_call(0x41656C, (int)X_CL_NextDownload);

	Cvar_Set("shortversion", "1.1x");
	Cvar_Set("version", "COD MP 1.1x build 1415 Feb 14 2021 18:45 by Dftd and Prawy");

	if (GetModuleHandle("cgame_mp_x86.dll") != NULL)
		DLLInit();
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
		case DLL_PROCESS_ATTACH:
			g_hModule = hModule;
			DisableThreadLibraryCalls(hModule);
			CloseHandle(CreateThread(NULL, NULL, &tExtend, NULL, NULL, &g_threadID));
			break;

		case DLL_PROCESS_DETACH:
		case DLL_THREAD_DETACH:
			delete game;
			Discord_Shutdown();
			FreeLibraryAndExitThread(g_hModule, 0);
			break;
	}

	return true;
}

DWORD WINAPI tExtend(LPVOID) {
	game = new COD();
	return 0;
}
