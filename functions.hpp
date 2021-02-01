#pragma once
#define QDECL __cdecl
#define MAX_SERVERSTATUS_TEXT 1024
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

static int (QDECL* syscall)(int arg, ...) = (int (QDECL*)(int, ...))0x460230;
static short BigShort(short l);

typedef int (*Com_Printf_t)(const char* format, ...);
static Com_Printf_t Com_Printf = (Com_Printf_t)0x4357B0;

typedef int (*QDECL Com_Error_t)(int code, const char* fmt, ...);
static Com_Error_t Com_Error = (Com_Error_t)0x435AD0;

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