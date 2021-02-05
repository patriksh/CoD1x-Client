#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>
#include <regex>
// Pretty sure I gotta clear up those #includes. These are both in discord.hpp and here.

#define QDECL __cdecl
#define MAX_SERVERSTATUS_TEXT 1024
#define MAX_NAME_LENGTH 32
#define BIG_INFO_STRING 8192
#define MAX_VA_STRING 32000

typedef enum { qfalse, qtrue } qboolean;

typedef struct cvar_s {
    char* name;
    char* string;
    char* resetString;
    char* latchedString;
    int flags;
    qboolean modified;
    int modificationCount;
    float value;
    int integer;
    struct cvar_s* next;
    struct cvar_s* hashNext;
} cvar_t;

typedef enum {
    ERR_FATAL,
    ERR_VID_FATAL,
    ERR_DROP,
    ERR_SERVERDISCONNECT,
    ERR_DISCONNECT,
    ERR_NEED_CD,
    ERR_AUTOUPDATE
} errorParm_t;

typedef enum {
    NA_BOT,
    NA_BAD,
    NA_LOOPBACK,
    NA_BROADCAST,
    NA_IP,
    NA_IPX,
    NA_BROADCAST_IPX
} netadrtype_t;

typedef struct {
    netadrtype_t type;

    byte ip[4];
    byte ipx[10];

    unsigned short port;
} netadr_t;

typedef struct {
    netadr_t adr;
    char hostName[MAX_NAME_LENGTH];
    char mapName[MAX_NAME_LENGTH];
    char game[MAX_NAME_LENGTH];
    int netType;
    int gameType;
    int clients;
    int maxClients;
    int minPing;
    int maxPing;
    int ping;
    qboolean visible;
    int allowAnonymous;
    int pswrd;
} serverInfo_t;

static int (QDECL* syscall)(int arg, ...) = (int (QDECL*)(int, ...))0x460230;
static short BigShort(short l);

typedef int (*Com_Printf_t)(const char* format, ...);
static Com_Printf_t Com_Printf = (Com_Printf_t)0x4357B0;

typedef int (*QDECL Com_Error_t)(int code, const char* fmt, ...);
static Com_Error_t Com_Error = (Com_Error_t)0x435AD0;

typedef int (*QDECL Com_Sprintf_t)(char* dest, int size, const char* fmt, ...);
static Com_Sprintf_t Com_Sprintf = (Com_Sprintf_t)0x44AC60;

typedef const char* (*QDECL va_t)(const char* format, ...);
static va_t va = (va_t)0x44ACE0;

typedef void (*xcommand_t) (void);
typedef int (*Cmd_AddCommand_t)(const char*, xcommand_t);
static Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x428840;

typedef int (*Cvar_Get_t)(const char*, const char*, int);
static Cvar_Get_t Cvar_Get = (Cvar_Get_t)0x439350;

typedef int (*Cvar_Set_t)(const char*, const char*);
static Cvar_Set_t Cvar_Set = (Cvar_Set_t)0x439650;

typedef int (*CL_Frame_t)(int);
static CL_Frame_t CL_Frame = (CL_Frame_t)0x411280;

typedef char* (*NET_AdrToString_t)(netadr_t);
static NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x449150;

typedef char* (*CL_SetServerInfo_t)(const char*, serverInfo_t*, int);
static CL_SetServerInfo_t CL_SetServerInfo = (CL_SetServerInfo_t)0x4129F0;

typedef char* (*Info_ValueForKey_t)(const char*, const char*);
static Info_ValueForKey_t Info_ValueForKey = (Info_ValueForKey_t)0x44ADA0;

typedef char* (*Info_SetValueForKey_t)(char*, const char*, const char*);
static Info_SetValueForKey_t Info_SetValueForKey = (Info_SetValueForKey_t)0x44B150;

// this eraseAll mess can be coded a bit better
static void eraseAllSubStr(std::string& mainStr, const std::string& toErase) {
    size_t pos = std::string::npos;
    while ((pos = mainStr.find(toErase)) != std::string::npos) {
        mainStr.erase(pos, toErase.length());
    }
}

static void eraseSubStrings(std::string& mainStr, const std::vector<std::string>& strList) {
    std::for_each(strList.begin(), strList.end(), std::bind(eraseAllSubStr, std::ref(mainStr), std::placeholders::_1));
}

static bool clearSymbolsSub(char c) {
    return (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-~`!?@#$.,><'\":;/\\|(){}[]=+*&^ ", c) == NULL);
}

static std::string clearColors(std::string& hostname) {
    eraseSubStrings(hostname, { "^1", "^2", "^3", "^4", "^5", "^6", "^7", "^8", "^9^", "^0" });
    return hostname;
}

static std::string clearSymbols(std::string& s) {
    s.erase(std::remove_if(s.begin(), s.end(), clearSymbolsSub), s.end());
    return s;
}

static std::string trim(const std::string& s) {
    auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) { return std::isspace(c); });
    auto wsback = std::find_if_not(s.rbegin(), s.rend(), [](int c) { return std::isspace(c); }).base();
    return (wsback <= wsfront ? std::string() : std::string(wsfront, wsback));
}