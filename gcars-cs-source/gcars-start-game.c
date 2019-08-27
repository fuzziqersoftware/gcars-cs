#include "gamecube.h"
#include "gcars.h"
#include "load-game.h"

#include "hook.bin.h"

void GCARSStartGame(GCARS_GAME* game)
{
    GCARSSaveCodes();

    GCARSStatus("Please wait");

    PAD pad;
    u32 x,y;
    u32 codesactive = 0;
    u32 codesoffset = 0;

    //__lwp_stop_multitasking();
    //memset((void*)MEMORY_START,0,(u32)MEMORY_TOP - (u32)MEMORY_START);

    u32* codes = malloc(8 * 256);
    for (x = 0; x < game->numcodes; x++)
    {
        if (game->codes[x]->enabled)
        {
            memcpy((void*)((u32)codes + (8 * codesactive)),game->codes[x]->lines,game->codes[x]->numlines * 8);
            codesactive += game->codes[x]->numlines;
        }
    }

    /*Addresses to check:
        80000028 memory size
        80000034 arena high
        80000038 fst loc in ram
        800000EC memory top
        800000F0 simulated memory size
        800000F4 pad spec? */

    /*if (*(u32*)0x800000EC > 0x817F8000)
    memcpy((void*)((*(u32*)0x800000F4) - 0x8000),(void*)(*(u32*)0x800000F4),0x81800000 - (*(u32*)0x8000003F));

    if (*(u32*)0x80000028 > 0x817F8000) *(u32*)0x80000028 -= 0x8000;
    if (*(u32*)0x80000034 > 0x817F8000) *(u32*)0x80000034 -= 0x8000;
    if (*(u32*)0x80000038 > 0x817F8000) *(u32*)0x80000038 -= 0x8000;
    if (*(u32*)0x800000EC > 0x817F8000) *(u32*)0x800000EC -= 0x8000;
    if (*(u32*)0x800000F0 > 0x817F8000) *(u32*)0x800000F0 -= 0x8000;
    if (*(u32*)0x800000F4 > 0x817F8000) *(u32*)0x800000F4 -= 0x8000; */

    *(u32*)0x80000028 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x80000034 = 0;
    *(u32*)0x80000038 = 0;
    *(u32*)0x800000EC = (u32)MEMORY_START;
    *(u32*)0x800000F0 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x800000F4 = 0;

    memcpy((void*)ENTRYPOINT,&hook,HOOK_SIZE);
    flush_code((void*)ENTRYPOINT,HOOK_SIZE);
    memcpy((void*)GCARS_CODELIST,codes,codesactive * 8);
    free(codes);

    GCARS_PAUSE = 0;
    GCARS_ENABLE_CS = 0; // Disable the Control Simulator 
    GCARS_CONDITIONAL = 0;

    //hexviewer();
    void* entrypoint = LoadGame();
    //GCARSErrorNum("entrypoint is",(u32)entrypoint);
    if (entrypoint == NULL) return;

    //hexviewer();

    codes = (u32*)GCARS_CODELIST;
    for (x = 0; x < codesactive; x++)
    {
        if (((codes[x * 2] & 0x01FFFFFF) | 0x80000000) > *(u32*)0x80000038) codes[x * 2] -= 0x8000;
    }

    //*(vu16*)(0xCC002002) = 2; // disable video 

    debug_hook(); // before AR codes run!
    //hexviewer();
    ENTRYPOINT(); // Run AR codes once!
    //hexviewer();
    debug_hook(); // After AR codes run!
    ((void (*)())entrypoint)(); // Start the game!
}

