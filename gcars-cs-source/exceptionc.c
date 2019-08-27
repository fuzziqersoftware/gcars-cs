#include "gamecube.h"

extern char exception_start,exception_end,exception_patch;
extern void exception_handler_default();
extern void exception_handler_decrementer();
extern void exception_handler_system_call();
extern void exception_handler_external();

unsigned long exception_location[15] = {
    0x00000100,0x00000200,0x00000300,0x00000400,0x00000500,0x00000600,
    0x00000700,0x00000800,0x00000900,0x00000C00,0x00000D00,0x00000F00,
    0x00001300,0x00001400,0x00001700};

char *exception_name[15] = {
    "System Reset","Machine Check","DSI","ISI","Interrupt","Alignment",
    "Program","Floating Point","Decrementer","System Call","Trace",
    "Performance","IABR","Reserved","Thermal"};

unsigned char exception_nums[] = {0,2,3,10,12};

void exception_init()
{
    int i;
    for (i = 0; i < 5; ++i)
    {
        void * address = (void*)(exception_location[exception_nums[i]] | 0x80000000);
        memcpy(address,&exception_start,&exception_end - &exception_start);
        *(unsigned long*)(&exception_patch - &exception_start + (int)address) |= exception_nums[i];
        flush_code(address,&exception_end - &exception_start);
        exception_set_handler(exception_nums[i],exception_handler_default);
        flush_code(address,0x100);
    }

    exception_set_handler(8,exception_handler_decrementer);
    exception_set_handler(9,exception_handler_system_call);

    CONTEXT_CURRENT = 0x80002000;
    CONTEXT_CURRENT_PHYS = 0x2000;
}

void exception_close()
{
    int i;
    for (i = 0; i < 15; ++i)
    {
        void * address = (void*)(exception_location[i] | 0x80000000);
        *(long*)address = 0x4C000064;
        flush_code(address,0x100);
    }
}

void exception_set_handler(int exception,void (*handler)(int,struct context_s*))
{
    ((void**)0x80003000)[exception] = handler;
}

void exception_handler_default(int exception)
{
    int i;
    struct context_s *c = (struct context_s*)(CONTEXT_CURRENT);
    // do something? 

    /*PAD pad;
    unsigned long* address;
    u32 memaddr = 0x80000000;
    u8 selected = 0,draw = 1;
    char* menu1 = "Memory Viewer";
    char* menu2 = "Relaunch GCARS-CS";
    char* menu3 = "Reset GameCube";
    for (;;)
    {
        if (draw)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            DEBUG_SetTextColor(COLOR_YELLOW);
            DEBUG_Print(50,50,"Exception:");
            DEBUG_Print(50 + (16 * 11),50,exception_name[exception]);
            DEBUG_SetTextColor(COLOR_WHITE);
            for (i = 0; i < 8; i++)
            {
                DEBUG_ShowValueU32(50,70 + (i * 20),c->GPR[(i * 4) + 0]);
                DEBUG_ShowValueU32(194,70 + (i * 20),c->GPR[(i * 4) + 1]);
                DEBUG_ShowValueU32(338,70 + (i * 20),c->GPR[(i * 4) + 2]);
                DEBUG_ShowValueU32(482,70 + (i * 20),c->GPR[(i * 4) + 3]);
            }
            DEBUG_SetTextColor(COLOR_AQUA);
            DEBUG_ShowValueU32(50,230,c->LR);
            if ((exception == 2) || (exception == 7))
            {
                address = (unsigned long*)c->SRR0;
                memaddr = c->SRR0;
                DEBUG_SetTextColor(COLOR_WHITE);
                for (i = 0; i < 16; i += 4)
                {
                    DEBUG_ShowValueU32(50,250 + (i * 5),address[i]);
                    DEBUG_ShowValueU32(194,250 + (i * 5),address[i + 1]);
                    DEBUG_ShowValueU32(338,250 + (i * 5),address[i + 2]);
                    DEBUG_ShowValueU32(482,250 + (i * 5),address[i + 3]);
                }
                DEBUG_SetTextColor(COLOR_LIME);
                DEBUG_ShowValueU32(194,230,c->SRR0);
            } else {
                DEBUG_SetTextColor(COLOR_GRAY75);
                DEBUG_ShowValueU32(194,230,c->SRR0);
            }
            DEBUG_SetTextColor(COLOR_GRAY75);
            DEBUG_ShowValueU32(338,230,c->SRR1);
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,360,menu1);
            DEBUG_Print(50,380,menu2);
            DEBUG_Print(50,400,menu3);
            DEBUG_SetTextColor(COLOR_YELLOW);
            switch (selected)
            {
              case 0:
                DEBUG_Print(50,360,menu1);
                break;
              case 1:
                DEBUG_Print(50,380,menu2);
                break;
              case 2:
                DEBUG_Print(50,400,menu3);
                break;
              default:
                selected = 0;
            }
            debugDrawInfo();
            FS_PAD_WaitEmptyAll();
            draw = 0;
        }
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < 2) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 2;
            draw = 1;
        }
        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                hexviewer("Memory Viewer",memaddr);
                break;
              case 1:
                ((void (*)())0x81300000)();
                break;
              case 2:
                reset();
                break;
            }
            draw = 1;
        }
        FS_VIDEO_WaitVSync();
    } */
}

