//#define DISABLE_EDITORS

#define fb (u32*)(0xC0900000)
#define HOOK_SIZE 0x5A00

#define CONTROLSIM_PORT 22064
#define CONTROLSIM_VERSION 0x00000007
#define CONTROLSIM_VERIFIER 0x1058EA9C

#define MODE_GCARS_ENTER_GAMENAME  0x3A000100
#define MODE_GCARS_ENTER_GAMEID    0x3A000101
#define MODE_GCARS_ENTER_CODENAME  0x3A000102
#define MODE_GCARS_ENTER_CODE      0x3A000103
#define MODE_GCARS_ENTER_GAME      0x3A000104
#define MODE_CS_ENTER_CODE         0x3A010103
#define MODE_MEMMANAGE             0x34010207

// GCARS-CS memory map 
//#define LEVINE_CONN        ((CSSocket*)     0x817F7F00) // also addr of card debug data 
#define MEMORY_START       ((void*)         0x817F8000)
#define ENTRYPOINT         ((void (*)())    0x817F8000)
#define GCARS_WRITER       ((void (*)(u32*))0x817F8004)
#define GCARS_READER       ((void (*)(u32*))0x817F8008)
#define CS_PACKET_BUFFER   ((u32*)          0x817FDA00)
#define CS_ADDRESS_BUFFER  ((u32*)          0x817FEA00)
#define CS_ADDRESS_LIST    ((u32*)          0x817FF000)
#define CS_DATA            ((CSData*)       0x817FF600)
#define CS_ENTITY          ((CSEntity*)     0x817FF780)
#define GCARS_PAUSE        (*(u32*)         0x817FF7F4)
#define GCARS_ENABLE_CS    (*(u32*)         0x817FF7F8)
#define GCARS_CONDITIONAL  (*(u32*)         0x817FF7FC)
#define GCARS_CODELIST     ((u32*)          0x817FF800)
#define MEMORY_TOP         ((void*)         0x81800000)

// debugger memory map 
//#define MEMORY_START       ((void*)         0x817F8000)
//#define ENTRYPOINT         ((void (*)())    0x817F8000)
#define DB_PACKET_BUFFER   ((u32*)          0x817FDA00)
#define DB_CONNECTION      ((CSSocket*)     0x817FF600)
#define DB_ENTITY          ((CSEntity*)     0x817FF780)
#define DB_STATE           (*(u32*)         0x817FFFFC)
//#define MEMORY_TOP         ((void*)         0x81800000)

// debugger commands 
#define DB_COMMAND_RESET          0x0000 // no arg 
#define DB_COMMAND_STATE          0x1000 // arg = game state: 
  #define DB_STATE_RUN            0x0000
  #define DB_STATE_PAUSE          0x0001
#define DB_COMMAND_NEXT           0x1100 // no arg 
#define DB_COMMAND_HOOK           0x2000 // no arg (address is u32, immediately follows) 
#define DB_COMMAND_HOOKSEARCH     0x2100 // arg = size of search data 
#define DB_COMMAND_PEEK           0x3000 // arg = size to be read (<64) 
#define DB_COMMAND_POKE           0x3100 // arg = size to be written 
#define DB_COMMAND_DUMP           0x3200 // no arg (size/address are u32, immediately follow) 
#define DB_COMMAND_SCREEN         0x3300 // arg = screen type: 
  #define DB_SCREEN_EFB           0x0000
  #define DB_SCREEN_XFB           0x0001
#define DB_COMMAND_BREAKPOINT     0x4000 // arg = breakpoint type: 
  #define DB_BREAKPOINT_READ      0x0000
  #define DB_BREAKPOINT_WRITE     0x0001
  #define DB_BREAKPOINT_READWRITE 0x0002
  #define DB_BREAKPOINT_EXECUTE   0x0003
  #define DB_BREAKPOINT_STEP      0x0004

