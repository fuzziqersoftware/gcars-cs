#ifndef __IS_HOOK

#define GCARS_FILE_IDENTIFIER   0x8A13DE1B
#define GCARS_FILE_VERSION      0x00000005

typedef struct {
    char name[64];
    u32 enabled;
    u32 numlines;
    u32* lines; } GCARS_CODE;

typedef struct {
    char name[64];
    u32 gameid;
    u32 numcodes;
    u32 numaddress[4];
    u32* address[4];
    u32 padaddress;
    s32 frames;
    u32 flags;
    u32 filler;
    GCARS_CODE** codes; } GCARS_GAME;

typedef struct {
    u32 numgames;
    GCARS_GAME** games; } GCARS_DATABLOCK;

void AddGame(GCARS_DATABLOCK*,GCARS_GAME*);
void RemoveGame(GCARS_DATABLOCK*,GCARS_GAME*);
void RemoveGameID(GCARS_DATABLOCK*,u32);
void AddCode(GCARS_GAME*,GCARS_CODE*);
void RemoveCode(GCARS_GAME*,GCARS_CODE*);
void RemoveCodeID(GCARS_GAME*,u32);

#include "message.h"
//#include "gcars-codelist.h"
#include "gcars-select.h"
#include "gcars-entry.h"
#include "gcars-save-load.h"
#include "gcars-start-game.h"

#endif

#include "memorymap.h"

#ifndef __IS_HOOK

#ifndef MAIN_FILE
extern GCARS_DATABLOCK* datablock;
#else
GCARS_DATABLOCK* datablock;
#endif

#endif

