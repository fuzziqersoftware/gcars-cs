#include "gamecube.h"
#include "cs.h"

extern u32 malloc_count;

bool debug = false;

void sramMenu()
{
    if (!debug) return;

    SRAM sram;//(SRAM*)0x80002000;
    u32 x;
    u8 draw = 1;
    u32 selected = 0;
    s32 scroll = 0;
    PAD pad;

    char* langs[] = {"English","German","French","Spanish","Italian","Dutch"};
    char* title = "GCARS: SRAM Menu";
    char* menu1 = "View as Raw Data";
    char* menu2 = "EAD 0";
    char* menu3 = "EAD 1";
    char* menu4 = "Time Bias";
    char* menu5 = "Video Offset";
    char* menu6 = "Language";
    char* menu7 = "Audio Mode";

    sram_read(&sram);

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
            DEBUG_ShowValueU32(416,170,sram.ead0);
            DEBUG_Print(50,190,menu3);
            DEBUG_ShowValueU32(416,190,sram.ead1);
            DEBUG_Print(50,210,menu4);
            DEBUG_ShowValueU32(416,210,sram.counterbias);
            DEBUG_Print(50,230,menu5);
            DEBUG_ShowValueU8(416,230,sram.displayoffset);
            DEBUG_Print(50,250,menu6);
            DEBUG_Print(400,250,langs[sram.language]);
            DEBUG_Print(50,270,menu7);
            DEBUG_Print(400,270,(sram.flags & 0x04 ? "Stereo" : "Mono"));
            DEBUG_SetTextColor(COLOR_YELLOW);
            switch (selected)
            {
              case 0:
                DEBUG_Print(50,150,menu1);
                break;
              case 1:
                DEBUG_Print(50,170,menu2);
                DEBUG_ShowValueU32(416,170,sram.ead0);
                break;
              case 2:
                DEBUG_Print(50,190,menu3);
                DEBUG_ShowValueU32(416,190,sram.ead1);
                break;
              case 3:
                DEBUG_Print(50,210,menu4);
                DEBUG_ShowValueU32(416,210,sram.counterbias);
                break;
              case 4:
                DEBUG_Print(50,230,menu5);
                DEBUG_ShowValueU8(416,230,sram.displayoffset);
                break;
              case 5:
                DEBUG_Print(50,250,menu6);
                DEBUG_Print(400,250,langs[sram.language]);
                break;
              case 6:
                DEBUG_Print(50,270,menu7);
                DEBUG_Print(400,270,(sram.flags & 0x04 ? "Stereo" : "Mono"));
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
            if (selected < 6) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 6;
            draw = 1;
        }

        if ((pad.Digital.Right || (pad.Analog.X > 0x40) || (pad.AnalogC.X > 0x40)) && (selected == 5))
        {
            if (sram.language < 5) sram.language++;
            else sram.language = 0;
            draw = 1;
        }
        if ((pad.Digital.Left || (pad.Analog.X < -0x40) || (pad.AnalogC.X < -0x40)) && (selected == 5))
        {
            if (sram.language > 0) sram.language--;
            else sram.language = 5;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                hexviewer("View SRAM (0x40 bytes)",(u32)(&sram));
                break;
              case 1:
                NETGUIEnterU32(&sram.ead0,"SRAM: EAD 0");
                break;
              case 2:
                NETGUIEnterU32(&sram.ead1,"SRAM: EAD 1");
                break;
              case 3:
                NETGUIEnterU32(&sram.counterbias,"SRAM: Time Bias");
                break;
              case 4:
                scroll = sram.displayoffset;
                NETGUIEnterU32(&scroll,"SRAM: Video Offset");
                sram.displayoffset = scroll;
                break;
              case 5:
                break;
              case 6:
                sram.flags ^= 0x04;
                break;
              default:
                GCARSError("Invalid menu state.");
                selected = 0;
            }
            draw = 1;
        }

        if (pad.Digital.B)
        {
            sram_write(&sram);
            return;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void debugMenu()
{
    if (!debug) return;

    char* title = "GCARS: Levine / Debug Menu";
    char* menu1 = "Reloader";
    char* menu2 = "Memory Viewer";
    char* menu3 = "Cause DSI Exception";
    char* menu4 = "Rip Disc";
    char* menu5 = "BBA Register Viewer";
    char* menu6 = "Select Card Slot";
    char* menu7 = "View Memory Card";

    void* card;
    u32 x;
    u8 draw = 1;
    u32 selected = 0;
    s32 scroll = 0;
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
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,150,menu1);
            DEBUG_Print(50,170,menu2);
            DEBUG_Print(50,190,menu3);
            DEBUG_Print(50,210,menu4);
            DEBUG_Print(50,230,menu5);
            DEBUG_Print(50,250,menu6);
            DEBUG_Print(50,270,menu7);
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
              case 6:
                DEBUG_Print(50,270,menu7);
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
            if (selected < 6) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 6;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                ((void (*)())0x80001800)();
                break;
              case 1:
                hexviewer("Fuzziqer Software HexDump",0x80000000);
                break;
              case 2:
                *(u32*)0x40000000 = 0;
                break;
              case 3:
                //rip_disc();
                GCARSError("This feature has been disabled.");
                break;
              case 4:
                hexbbaviewer();
                break;
              case 5:
                GCARSSelectCardSlot();
                break;
              case 6:
                GCARSStatus("Loading card");
                x = (exi_deviceid(pad.Digital.Y ? 1 : 0,0) & 0x000000FC) * 0x20000;
                card = malloc(x);
                dcache_inv(card,x);
                CARD_ReadArray(pad.Digital.Y ? 1 : 0,card,0,x);
                dcache_flush(card,x);
                hexviewer(pad.Digital.Y ? "Memory Card in Slot B" : "Memory Card in Slot A",(u32)card);
                free(card);
                break;
              default:
                GCARSError("Invalid menu state.");
                selected = 0;
            }
            draw = 1;
        }

        if (pad.Digital.B) return;

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void debugEnable(bool d) { debug = d; }
bool debugEnabled() { return debug; }

void debugDrawInfo()
{
    if (!debug) return;
    DEBUG_ShowValueU32(66,450,malloc_count);
}

