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
#define MAX_INFO_STRING 1024
#define MAX_MSGLEN 16384
#define MAX_STRING_CHARS 1024
#define MAX_STRING_TOKENS 256
#define MAX_RELIABLE_COMMANDS 64
#define MAX_QPATH 64
#define MAX_OSPATH 128	

// #define CLC_BASE_ADDR 0x15CE860
// #define clc (*((clientConnection_t*)(CLC_BASE_ADDR)))

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

typedef int fileHandle_t;

typedef enum {
    NS_CLIENT,
    NS_SERVER
} netsrc_t;

typedef struct {
    netsrc_t	sock;

    int			dropped;			// between last packet and previous

    netadr_t	remoteAddress;
    int			qport;				// qport value to write when transmitting

    // sequencing variables
    int			incomingSequence;
    int			outgoingSequence;

    // incoming fragment assembly buffer
    int			fragmentSequence;
    int			fragmentLength;
    byte		fragmentBuffer[MAX_MSGLEN];

    // outgoing fragment buffer
    // we need to space out the sending of large fragmented messages
    qboolean	unsentFragments;
    int			unsentFragmentStart;
    int			unsentLength;
    byte		unsentBuffer[MAX_MSGLEN];
} netchan_t;


typedef struct {

    int			clientNum;
    int			lastPacketSentTime;			// for retransmits during connection
    int			lastPacketTime;				// for timeouts

    netadr_t	serverAddress;
    int			connectTime;				// for connection retransmits
    int			connectPacketCount;			// for display on connection dialog
    char		serverMessage[MAX_STRING_TOKENS];	// for display on connection dialog

    int			challenge;					// from the server to use for connecting
    int			checksumFeed;				// from the server for checksum calculations

    // these are our reliable messages that go to the server
    int			reliableSequence;
    int			reliableAcknowledge;		// the last one the server has executed
    char		reliableCommands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS];

    // server message (unreliable) and command (reliable) sequence
    // numbers are NOT cleared at level changes, but continue to
    // increase as long as the connection is valid

    // message sequence is used by both the network layer and the
    // delta compression layer
    int			serverMessageSequence;

    // reliable messages received from server
    int			serverCommandSequence;
    int			lastExecutedServerCommand;		// last server command grabbed or executed with CL_GetServerCommand
    char		serverCommands[MAX_RELIABLE_COMMANDS][MAX_STRING_CHARS];

    // file transfer from server
    fileHandle_t download;
    char		downloadTempName[MAX_OSPATH];
    char		downloadName[MAX_OSPATH];
    int			downloadNumber;
    int			downloadBlock;	// block we are waiting for
    int			downloadCount;	// how many bytes we got
    int			downloadSize;	// how many bytes we got
    char		downloadList[MAX_INFO_STRING]; // list of paks we need to download
    qboolean	downloadRestart;	// if true, we need to do another FS_Restart because we downloaded a pak

    // demo information
    char		demoName[MAX_QPATH];
    qboolean	spDemoRecording;
    qboolean	demorecording;
    qboolean	demoplaying;
    qboolean	demowaiting;	// don't record until a non-delta message is received
    qboolean	firstDemoFrameSkipped;
    fileHandle_t	demofile;

    int			timeDemoFrames;		// counter of rendered frames
    int			timeDemoStart;		// cls.realtime before first frame
    int			timeDemoBaseTime;	// each frame will be at this time + frameNum * 50

    // big stuff at end of structure so most offsets are 15 bits or less
    netchan_t	netchan;
} clientConnection_t;

typedef void* (*Sys_LoadDll_t)(const char* name, char* fqpath, int(**entryPoint) (int, ...), int (*systemcalls)(int, ...));
static Sys_LoadDll_t Sys_LoadDll = (Sys_LoadDll_t)0x4633A0;

static int (QDECL* syscall)(int arg, ...) = (int (QDECL*)(int, ...))0x460230;
static short BigShort(short l);

typedef int (*Com_Printf_t)(const char* format, ...);
static Com_Printf_t Com_Printf = (Com_Printf_t)0x4357B0;

typedef int (*QDECL Com_Error_t)(int code, const char* fmt, ...);
static Com_Error_t Com_Error = (Com_Error_t)0x435AD0;

static void Com_Sprintf(char* dest, int size, const char* fmt, ...) {
    int ret;
    va_list argptr;

    va_start(argptr, fmt);
    ret = vsnprintf(dest, size, fmt, argptr);
    va_end(argptr);
    if (ret == -1) {
        Com_Printf("Com_Sprintf: overflow of %i bytes buffer\n", size);
    }
}

typedef const char* (*QDECL va_t)(const char* format, ...);
static va_t va = (va_t)0x44ACE0;

typedef void (*xcommand_t) (void);
typedef int (*Cmd_AddCommand_t)(const char*, xcommand_t);
static Cmd_AddCommand_t Cmd_AddCommand = (Cmd_AddCommand_t)0x428840;

typedef cvar_t (*Cvar_Get_t)(const char*, const char*, int);
static Cvar_Get_t Cvar_Get = (Cvar_Get_t)0x439350;

typedef cvar_t* (*Cvar_FindVar_t)(const char*);
static Cvar_FindVar_t Cvar_FindVar = (Cvar_FindVar_t)0x439280;

static const char* Cvar_VariableString(const char* var_name) {
    cvar_t* var;

    var = Cvar_FindVar(var_name);
    if (!var)
        return "";
    return var->string;
}

static void Cvar_VariableStringBuffer(const char* var_name, char* buffer, int bufsize) {
    cvar_t* var;

    var = Cvar_FindVar(var_name);
    if (!var) {
        *buffer = 0;
    } else {
        strncpy(buffer, var->string, bufsize);
    }
}

typedef int (*Cvar_Set_t)(const char*, const char*);
static Cvar_Set_t Cvar_Set = (Cvar_Set_t)0x439650;

typedef int (*Cvar_SetValue_t)(const char*, float);
static Cvar_SetValue_t Cvar_SetValue = (Cvar_SetValue_t)0x439960;

typedef int (*FS_Write_t)(const void*, int, fileHandle_t);
static FS_Write_t FS_Write = (FS_Write_t)0x42A560;

typedef int (*FS_FCloseFile_t)(fileHandle_t);
static FS_FCloseFile_t FS_FCloseFile = (FS_FCloseFile_t)0x4294B0;

typedef fileHandle_t(*FS_SV_FOpenFileWrite_t)(const char*);
static FS_SV_FOpenFileWrite_t FS_SV_FOpenFileWrite = (FS_SV_FOpenFileWrite_t)0x43A7E0;

typedef void(*FS_Restart_t)(int);
static FS_Restart_t FS_Restart = (FS_Restart_t)0x42D2B0;

typedef char*(*FS_BuildOSPath_t)(const char*, const char*, const char*);
static FS_BuildOSPath_t FS_BuildOSPath = (FS_BuildOSPath_t)0x428EE0;

typedef void(*FS_CopyFile_t)(char*, char*);
static FS_CopyFile_t FS_CopyFile = (FS_CopyFile_t)0x429070;

static void FS_SV_Rename(const char* from, const char* to) {
    char from_ospath[MAX_OSPATH];
    char to_ospath[MAX_OSPATH];

    Com_Sprintf(from_ospath, sizeof(from_ospath), "%s/%s", Cvar_VariableString("fs_homepath"), from);
    Com_Sprintf(to_ospath, sizeof(to_ospath), "%s/%s", Cvar_VariableString("fs_homepath"), to);

    if (rename(from_ospath, to_ospath)) {
        FS_CopyFile(from_ospath, to_ospath);
        remove(from_ospath);
    }
}

typedef int (*CL_Frame_t)(int);
static CL_Frame_t CL_Frame = (CL_Frame_t)0x411280;

typedef int (*CL_Reconnect_f_t)(void);
static CL_Reconnect_f_t CL_Reconnect_f = (CL_Reconnect_f_t)0x40F640;

typedef char* (*NET_AdrToString_t)(netadr_t);
static NET_AdrToString_t NET_AdrToString = (NET_AdrToString_t)0x449150;

typedef void (*CL_NextDownload_t)(void);
static CL_NextDownload_t CL_NextDownload = (CL_NextDownload_t)0x410190;

typedef void (*CL_DownloadsComplete_t)(void);
static CL_DownloadsComplete_t CL_DownloadsComplete = (CL_DownloadsComplete_t)0x40FFB0;

typedef void (*CL_BeginDownload_t)(const char*, const char*);
static CL_BeginDownload_t CL_BeginDownload = (CL_BeginDownload_t)0x4100D0;

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