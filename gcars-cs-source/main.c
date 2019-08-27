#define MAIN_FILE

#include "gamecube.h"

#include "cs.h"
#include "hook.bin.h"
#include "load-game.h"

#include "loaddol.h"
#include "memcard.h"

#include "debugmenus.h"

////////////////////////////////////////////////////////////////////////////////

void GCARSCredits();

extern u32 cobra;

int main()
{
    u32 x,y;
    setalign(0); // THIS MUST BE DONE BEFORE ANYTHING ELSE!! 
    mallocset(fb,320 * 480 * 4); // allocate framebuffer 

    PAD pad;
    exception_init();
    exi_init();
    u32 msr;
    msr = GetMSR();
    msr &= ~0x8000;
    msr |= 0x2002;
    SetMSR(msr);
    FS_PAD_Init();

    if (*(u32*)0x81300000 != 0x38210000)
    {
        u32 zero = 0;
        char ipldesc[0x100];
        exi_select(0,1,3);
        exi_imm(0,&zero,4,EXI_WRITE);
        exi_imm_ex(0,&ipldesc,0x100,EXI_READ);
        exi_deselect(0);
        if ((ipldesc[0x55] != 'P') || (ipldesc[0x56] != 'A') || (ipldesc[0x57] != 'L')) *(u32*)0x800000CC = 0;
        else {
            FS_PAD_ReadAllState(&pad);
            if (pad.Digital.B) *(u32*)0x800000CC = 5;
            else *(u32*)0x800000CC = 1;
        }
    }

    if (*(u32*)0x800000CC == 0) FS_VIDEO_Init(FS_VIDEO_640X480_NTSC_YUV16);
    if (*(u32*)0x800000CC == 1) FS_VIDEO_Init(FS_VIDEO_640X480_PAL50_YUV16);
    if (*(u32*)0x800000CC == 5) FS_VIDEO_Init(FS_VIDEO_640X480_PAL60_YUV16);
    FS_VIDEO_SetFrameBuffer(FS_VIDEO_FRAMEBUFFER_BOTH,(u32)fb);

    *(unsigned long*)0x8000001C = 0xC2339F3D;
    *(unsigned long*)0x80000020 = 0x0D15EA5E;
    *(unsigned long*)0x80000024 = 1;
    *(unsigned long*)0x80000028 = 0x017F8000;
    *(unsigned long*)0x8000002C = 1 + ((*(unsigned long*)0xCC00302C) >> 28);
    *(unsigned long*)0x800000EC = 0x817F8000;
    *(unsigned long*)0x800000F0 = 0x017F8000;
    *(unsigned long*)0x800000F8 = 162000000;
    *(unsigned long*)0x800000FC = 486000000;
    *(unsigned short*)0x800030E0 = 6;
    dcache_flush((void*)0x80000000,0x100);

    GRAPH_SetFramebuffer(fb);
    DEBUG_InitText(fb,COLOR_YELLOW);

    if (*(u32*)0x81300000 != 0x38210000)
    {
        GCARSClearFrameBuffer(COLOR_BLACK);
        GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
        GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
        DEBUG_SetTextColor(COLOR_YELLOW);
        DEBUG_Print(50,50 ,"FUZZIQER SOFTWARE presents:");
        DEBUG_SetTextColor(COLOR_QRHYGREEN);
        DEBUG_Print(50,100,"GameCube Action Replay Simulator");
        DEBUG_Print(50,120,"....and Control Simulator, v0.9.9");
        DEBUG_SetTextColor(COLOR_WHITE);
        DEBUG_Print(50,170,"by Fuzziqer Software, 2004-2006");
        DEBUG_Print(50,190,"www.fuzziqersoftware.com");
        DEBUG_SetTextColor(COLOR_YELLOW);
        DEBUG_Print(50,240,"Press L+X+Y to enable Debug Mode");
        DEBUG_Print(50,260,"Press START to begin!");

        for (;;)
        {
            FS_PAD_ReadAllState(&pad);
            if (pad.Digital.Start) break;
            if (pad.Digital.X && pad.Digital.Y && pad.Digital.L)
            {
                debugEnable(true);
                DEBUG_SetTextColor(COLOR_BLACK);
                DEBUG_Print(50,240,"Press L+X+Y to enable Debug Mode");
                DEBUG_SetTextColor(COLOR_YELLOW);
                DEBUG_Print(50,240,"Debug Mode enabled");
            }
            FS_VIDEO_WaitVSync();
        }
    }
    GCARSClearFrameBuffer(COLOR_BLACK);
    GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
    GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
    DEBUG_SetTextColor(COLOR_QRHYGREEN);
    DEBUG_Print(50,50,"Please wait...");
    GRAPH_Rectangle(0,100,320,2,COLOR_YELLOW);
    GRAPH_Rectangle(0,132,320,2,COLOR_YELLOW);
    DEBUG_SetTextColor(COLOR_YELLOW);

    if (*(u32*)0x81300000 != 0x38210000)
    {
        GRAPH_Rectangle(0,102,320,30,COLOR_BLACK);
        DEBUG_Print(50,110,"Setting up network");
        CSDefaultNetwork();
    }
    GRAPH_Rectangle(0,102,320,30,COLOR_BLACK);
    DEBUG_Print(50,110,"Stopping the disc drive");
    dvd_stop();
    if ((*(u32*)0x81300000 != 0x38210000) && !pad.Digital.Y)
    {
        GRAPH_Rectangle(0,102,320,30,COLOR_BLACK);
        DEBUG_Print(50,110,"Starting Ethernet Adapter");
        if (exi_deviceid(0,2) == 0x04020200) eth_init(&(CS_ENTITY->MAC),CS_ENTITY->speed);
    }
    GRAPH_Rectangle(0,102,320,30,COLOR_BLACK);
    DEBUG_Print(50,110,"Clearing hook memory");
    memset((void*)MEMORY_START,0,(u32)MEMORY_TOP - (u32)MEMORY_START);

    if (*(u32*)0x81300000 != 0x38210000) GCARSSelectCardSlot();
    GCARSLoadCodes();
    *(u32*)0x81300000 = 0x38210000;

    char* title = "Fuzziqer Software GCARS-CS";
    char* menu1 = "Action Replay Simulator";
    char* menu2 = "Control Simulator";
    char* menu3 = "Memory Card Manager";
    char* menu4 = "Start Game";
    char* menu5 = "Credits";
    char* menu6 = "Reset";

    void* entry;
    u8 draw = 1;
    u8 selected = 0;
    CSPacket csp;
    u32 size,time;
    for (;;)
    {
        if (draw)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,title);
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,150,menu1);
            DEBUG_Print(50,170,menu2);
            DEBUG_Print(50,190,menu3);
            DEBUG_Print(50,210,menu4);
            DEBUG_Print(50,230,menu5);
            DEBUG_Print(50,250,menu6);
            DEBUG_SetTextColor(COLOR_YELLOW);
            switch (selected)
            {
              case 0:
                DEBUG_Print(50,150,menu1);
                break;
              case 1:
                DEBUG_Print(50,170,menu2);
                break;
              case 2:
                DEBUG_Print(50,190,menu3);
                break;
              case 3:
                DEBUG_Print(50,210,menu4);
                break;
              case 4:
                DEBUG_Print(50,230,menu5);
                break;
              case 5:
                DEBUG_Print(50,250,menu6);
                break;
              default:
                selected = 0;
            }
            debugDrawInfo();
        }

        while (draw)
        {
            FS_PAD_ReadAllState(&pad);
            if (FS_PAD_IsEmpty(&pad)) draw = 0;
            else FS_VIDEO_WaitVSync();
        }

        LevineReceiveData(0,CS_PACKET_BUFFER);

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < 5) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 5;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                GCARSGames();
                break;
              case 1:
                if (exi_deviceid(0,2) != 0x04020200) GCARSError("You do not have a BBA.");
                else CSSetup();
                break;
              case 2:
                if (pad.Digital.Y) debugMenu();
                else if (pad.Digital.X) sramMenu();
                else GCARSError("This feature has been disabled.");
                break;
              case 3:
                entry = LoadGame();
                debug_hook();
                if (entry) ((void (*)())entry)();
                break;
              case 4:
                GCARSCredits();
                break;
              case 5:
                reset();
                break;
              default:
                GCARSError("Invalid menu state.");
                selected = 0;
            }
            draw = 1;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }

    return 0;
}

void GCARSCredits()
{
    char* title = "Fuzziqer Software GCARS-CS v0.9.9";

    u32 x;
    u8 draw = 1;
    u8 selected = 0;
    PAD pad;
    for (;;)
    {
        if (draw)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,title);
            DEBUG_SetTextColor(COLOR_YELLOW);
            DEBUG_Print(50,100,"GCARS by Fuzziqer Software");
            DEBUG_Print(50,120,"ControlSim by Fuzziqer Software");
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,150,"Special thanks to:");
            DEBUG_SetTextColor(COLOR_YELLOW);
            DEBUG_Print(50,170,"Costis   tmbinc    Sappharad");
            DEBUG_Print(50,190,"Peter    Parasyte  CrowTRobo");
            DEBUG_Print(50,210,"Diffuse  biolizard89");
            DEBUG_Print(50,230,"Neo-Ice  |3laze    dj veeman");
            DEBUG_Print(50,250,"WunSick  TheDaddy  DarkAkuma");
            DEBUG_Print(50,270,"emu_kidid");
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,320,"Press A, B, or Start to continue");
        }

        while (draw)
        {
            FS_PAD_ReadAllState(&pad);
            if (FS_PAD_IsEmpty(&pad)) draw = 0;
            else FS_VIDEO_WaitVSync();
        }

        LevineReceiveData(0,CS_PACKET_BUFFER);
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.A || pad.Digital.Start || pad.Digital.B) return;

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

// 2. Server app (works so far; don't know how well one server can handle more than 4 clients at once)
// 3. Synchronization mode (going to be harder than I thought)
// 5. SD support in memory card manager (more partially completed than before)

