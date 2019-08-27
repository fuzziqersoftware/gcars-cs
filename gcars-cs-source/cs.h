#include "gcars.h"
#include "cs-net.h"

// game flags 
#define FLAG_SYNCHRONIZE          0x00000001 // send & receive data EVERY FRAME 

// bad command 
#define COMMAND_INVALID_DATA      0xFF00 // [ ] error in reading/writing 

// p2p commands 
#define COMMAND_WRITE             0x0000 // [*] write following data 
#define COMMAND_PING              0x4000 // [*] ping (return data) 
#define COMMAND_STATUS            0x8500 // [*] change status 
#define COMMAND_START             0x8A00 // [*] start game 

// debug commands 
#define COMMAND_DBG_PAUSE         0xE000 // [ ] change system control 
#define COMMAND_DBG_READMEM       0xE100 // [ ] read memory 
#define COMMAND_DBG_READMEMREPLY  0xE200 // [ ] read memory reply (contains data) 
#define COMMAND_DBG_WRITEMEM      0xE300 // [ ] write memory 
#define COMMAND_DBG_WRITEMEMREPLY 0xE400 // [ ] write memory reply (for confirmation) 
#define COMMAND_DBG_BRANCH        0xE500 // [ ] branch to address (NOT RETURNABLE!) 

// server commands 
#define COMMAND_CONFIG            0xA000 // [-] copy game ids from packet 
#define COMMAND_LOADGAME          0xA100 // [-] load game 
#define COMMAND_NEW_SCRIPT        0x1000 // [*] overwrite address list and set local pad 
#define COMMAND_NEW_CODELIST      0x1100 // [*] overwrite codelist 

// client commands 
#define COMMAND_READY             0x8500 // [-] client loaded & ready 
#define COMMAND_DISCONNECT        0x8D00 // [-] client disconnected 

typedef struct {
    u16 buttons;
    s8  controlX;
    s8  controlY;
    s8  cstickX;
    s8  cstickY;
    u8  analogL;
    u8  analogR;
    u8  analogA;
    u8  analogB;
    u8  error;
    u8  unused; } CSPADStatus;

typedef struct {
    CSSocket connection[4];
    u32 IPAddress[4];
    CSPADStatus* padAddress; // Addresses of pad structures in memory 
    u32 frames; // number of frames between updates 
    u32 flags; // game flags 
    u8 local[4]; // padAddress remainder..... lol 
    u32 gc_gameID; // Game ID of disc 
    u32 unused; // Game ID of network game (unused) 
    u32 localPad; // Local controller number 
    u32 numAddress; // Number of codes in the reader script 
    u32 updateCount; // Timing variable for the CS hook 
    CSPADStatus latestPads[4]; // Latest pad data received 
    u8 servermode;
    u8 uselesscrap[3];
} CSData;

typedef struct {
    u32 verifier;
    u32 version;
    u16 size;
    u16 padspresent;
    u32 checksum;
    u32 gcGameID;
    u32 numCommands; } CSPacket;

#ifndef __IS_HOOK
#include "cs-select.h"
#include "cs-netgui.h"
#include "cs-entry.h"
#include "cs-start.h"

void CSDefaultNetwork();
int CSInit(GCARS_GAME*);
u32 CSChecksum(CSPacket*);
#endif

