#define CONTROLSIM_VERSION 0x00000007
#define CONTROLSIM_VERIFIER 0x1058EA9C

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

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;

typedef signed char s8;
typedef signed short s16;
typedef signed long s32;

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
    u32 verifier;
    u32 version;
    u16 size;
    u16 padspresent;
    u32 checksum;
    u32 gcGameID;
    u32 numCommands; } CSPacket;

DWORD byteswap(DWORD);
WORD byteswap(WORD);
u32 CSChecksum(CSPacket*);
void PAD_ReadState(CSPADStatus*);

