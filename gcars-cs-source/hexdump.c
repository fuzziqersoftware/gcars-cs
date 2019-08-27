#include "gamecube.h"
#include "gcars.h"

#define LINES_PER_SCREEN 16

u32 baseDispMem = 0x817F8000;
u32 baseAddr = 0x817F8000;
char* dispTitle;

u32 readpad()
{
    u32 value[3];
    value[0] = *(u32*)0xCC006400;
    value[1] = *(u32*)0xCC006404;
    value[2] = *(u32*)0xCC006408;
    value[1] &= 0x1F7F0000;
    return value[1];
}

void RenderHexViewer()
{
    DEBUG_SetTextColor(0xB2ABB200);
    DEBUG_ShowValueU32(0,50,baseAddr);
    DEBUG_Print(128,50,dispTitle);
    DEBUG_Print(0,360,"Press START to continue");
    DEBUG_SetTextColor(0xE100E194);
    u32 x;
    for (x = 0; x < LINES_PER_SCREEN; x++)
        DEBUG_ShowValueU32(0,70 + (x * 18),baseDispMem + (x * 16));

    u32 prevdispmem = baseDispMem;
    baseDispMem &= 0xBFFFFFFF;

    DEBUG_SetTextColor(0xFF80FF80);
    for (x = 0; x < LINES_PER_SCREEN; x++)
        if (((baseDispMem + (x * 16)) >= 0x80000000) &&
            ((baseDispMem + (x * 16)) < 0x817FFFFF))
            DEBUG_ShowValueU32(128,70 + (x * 18),*(u32*)(baseDispMem + (x * 16)));

    DEBUG_SetTextColor(0xC080C080);
    for (x = 0; x < LINES_PER_SCREEN; x++)
        if (((baseDispMem + 4 + (x * 16)) >= 0x80000000) &&
            ((baseDispMem + 4 + (x * 16)) < 0x817FFFFF))
            DEBUG_ShowValueU32(256,70 + (x * 18),*(u32*)(baseDispMem + 4 + (x * 16)));

    DEBUG_SetTextColor(0xFF80FF80);
    for (x = 0; x < LINES_PER_SCREEN; x++)
        if (((baseDispMem + 8 + (x * 16)) >= 0x80000000) &&
            ((baseDispMem + 8 + (x * 16)) < 0x817FFFFF))
            DEBUG_ShowValueU32(384,70 + (x * 18),*(u32*)(baseDispMem + 8 + (x * 16)));

    DEBUG_SetTextColor(0xC080C080);
    for (x = 0; x < LINES_PER_SCREEN; x++)
        if (((baseDispMem + 12 + (x * 16)) >= 0x80000000) &&
            ((baseDispMem + 12 + (x * 16)) < 0x817FFFFF))
            DEBUG_ShowValueU32(512,70 + (x * 18),*(u32*)(baseDispMem + 12 + (x * 16)));

    debugDrawInfo();
    baseDispMem = prevdispmem;
}

void hexviewer(char* title,u32 addr)
{
    u32 buttons;

    baseDispMem = (addr & 0xFFFFFFF0);
    baseAddr = addr;
    dispTitle = title;
    FS_PAD_WaitEmptyAll();
    for (;;)
    {
        GCARSClearFrameBuffer(0x00800080);
        RenderHexViewer();
tryagain:
        while (readpad() == 0) { }
        buttons = readpad();
        if (buttons == 0x00100000) goto tryagain;
        if (buttons == 0x00080000) baseDispMem -= 0x00000010; // d-up 
        if (buttons == 0x00040000) baseDispMem += 0x00000010; // d-down 
        if (buttons == 0x00020000) baseDispMem += 0x00000100; // d-right 
        if (buttons == 0x00010000) baseDispMem -= 0x00000100; // d-left 
        if (buttons == 0x00200000) baseDispMem += 0x00001000; // r 
        if (buttons == 0x00400000) baseDispMem -= 0x00001000; // l 
        if (buttons == 0x08000000) baseDispMem -= 0x00010000; // y 
        if (buttons == 0x04000000) baseDispMem += 0x00010000; // x 
        if (buttons == 0x00180000) baseDispMem -= 0x00100000; // z+d-up 
        if (buttons == 0x00140000) baseDispMem += 0x00100000; // z+d-down 
        if (buttons == 0x02000000) baseDispMem =  0x80000000; // b 
        if (buttons == 0x01000000) baseDispMem =  0x817F8000; // a 
        if (buttons == 0x10000000) // start 
        {
            FS_PAD_WaitEmptyAll();
            return;
        }
        while (readpad() == buttons) FS_VIDEO_WaitVSync();
        FS_VIDEO_WaitVSync();
    }
}

/* Keys:
 * D-UP     Up   0x10 
 * D-DOWN   Down 0x10 
 * D-LEFT   Up   0x100 
 * D-RIGHT  Down 0x100 
 * L        Up   0x1000 
 * R        Down 0x1000 
 * Y        Up   0x10000 
 * X        Down 0x10000 
 * Z+D-UP   Up   0x100000 
 * Z+D-DOWN Down 0x100000 
 * B        Go   0x80000000 
 * A        Go   0x817F8000 
 * START    Exit */
