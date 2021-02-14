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

extern char* (*qcurl_version)(void);

CURL* (*qcurl_easy_init)(void);
CURLcode(*qcurl_easy_setopt)(CURL* curl, CURLoption option, ...);
CURLcode(*qcurl_easy_perform)(CURL* curl);
void (*qcurl_easy_cleanup)(CURL* curl);
CURLcode(*qcurl_easy_getinfo)(CURL* curl, CURLINFO info, ...);
void (*qcurl_easy_reset)(CURL* curl);
const char* (*qcurl_easy_strerror)(CURLcode);

CURLM* (*qcurl_multi_init)(void);
CURLMcode(*qcurl_multi_add_handle)(CURLM* multi_handle,
	CURL* curl_handle);
CURLMcode(*qcurl_multi_remove_handle)(CURLM* multi_handle,
	CURL* curl_handle);
CURLMcode(*qcurl_multi_fdset)(CURLM* multi_handle,
	fd_set* read_fd_set,
	fd_set* write_fd_set,
	fd_set* exc_fd_set,
	int* max_fd);
CURLMcode(*qcurl_multi_perform)(CURLM* multi_handle,
	int* running_handles);
CURLMcode(*qcurl_multi_cleanup)(CURLM* multi_handle);
CURLMsg* (*qcurl_multi_info_read)(CURLM* multi_handle,
	int* msgs_in_queue);
const char* (*qcurl_multi_strerror)(CURLMcode);