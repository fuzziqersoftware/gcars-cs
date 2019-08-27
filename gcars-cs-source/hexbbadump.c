#include "gamecube.h"
#include "gcars.h"

u32 sel;

u32 bbareadpad()
{
    u32 value[3];
    value[0] = *(u32*)0xCC006400;
    value[1] = *(u32*)0xCC006404;
    value[2] = *(u32*)0xCC006408;
    value[1] &= 0x1F7F0000;
    return value[1];
}

void bbaRenderHexViewer()
{
    u32 x,y;
    u8 value;

    DEBUG_SetTextColor(0xE100E194);
    DEBUG_ShowValueU32(0,400,sel);
    for (x = 0; x < 16; x++) DEBUG_ShowValueU32(0,30 + (x * 18),x * 16);

    GRAPH_Rectangle(64 + ((sel & 0x0F) * 16),30 + (((sel & 0xF0) >> 4) * 18),16,16,0x4B554B4A);
    DEBUG_SetTextColor(0xFF80FF80);
    for (x = 0; x < 16; x++)
    {
        BBA_ReceiveCommand((x * 16) + 0x00,(u8*)&value,1);
        DEBUG_ShowValueU8(128,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x01,(u8*)&value,1);
        DEBUG_ShowValueU8(160,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x02,(u8*)&value,1);
        DEBUG_ShowValueU8(192,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x03,(u8*)&value,1);
        DEBUG_ShowValueU8(224,30 + (x * 18),value);
    }

    DEBUG_SetTextColor(0xC080C080);
    for (x = 0; x < 16; x++)
    {
        BBA_ReceiveCommand((x * 16) + 0x04,(u8*)&value,1);
        DEBUG_ShowValueU8(256,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x05,(u8*)&value,1);
        DEBUG_ShowValueU8(288,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x06,(u8*)&value,1);
        DEBUG_ShowValueU8(320,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x07,(u8*)&value,1);
        DEBUG_ShowValueU8(352,30 + (x * 18),value);
    }

    DEBUG_SetTextColor(0xFF80FF80);
    for (x = 0; x < 16; x++)
    {
        BBA_ReceiveCommand((x * 16) + 0x08,(u8*)&value,1);
        DEBUG_ShowValueU8(384,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x09,(u8*)&value,1);
        DEBUG_ShowValueU8(416,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x0A,(u8*)&value,1);
        DEBUG_ShowValueU8(448,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x0B,(u8*)&value,1);
        DEBUG_ShowValueU8(480,30 + (x * 18),value);
    }

    DEBUG_SetTextColor(0xC080C080);
    for (x = 0; x < 16; x++)
    {
        BBA_ReceiveCommand((x * 16) + 0x0C,(u8*)&value,1);
        DEBUG_ShowValueU8(512,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x0D,(u8*)&value,1);
        DEBUG_ShowValueU8(544,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x0E,(u8*)&value,1);
        DEBUG_ShowValueU8(576,30 + (x * 18),value);
        BBA_ReceiveCommand((x * 16) + 0x0F,(u8*)&value,1);
        DEBUG_ShowValueU8(608,30 + (x * 18),value);
    }
    debugDrawInfo();
}

void hexbbaviewer()
{
    sel = 0;
    u32 buttons,value32;
    u8 value8,value82[3];
    FS_PAD_WaitEmptyAll();
    for (;;)
    {
        GCARSClearFrameBuffer(0x00800080);
        bbaRenderHexViewer();
tryagain:
        while (bbareadpad() == 0) { }
        buttons = bbareadpad();
        if (buttons == 0x00080000) sel -= 0x10; // d-up 
        if (buttons == 0x00040000) sel += 0x10; // d-down 
        if (buttons == 0x00020000) sel++; // d-right 
        if (buttons == 0x00010000) sel--; // d-left 
        if (buttons == 0x00100000) // z 
        {
            BBA_ReceiveCommand(sel,&value8,1);
            value32 = value8;
            NETGUIEnterU32(&value32,"Change BBA Register");
            value8 = value32;
            BBA_SendCommand(sel,&value8,1);
        }
        if (buttons == 0x10000000) // start 
        {
            FS_PAD_WaitEmptyAll();
            return;
        }
        while (bbareadpad() == buttons) FS_VIDEO_WaitVSync();
        FS_VIDEO_WaitVSync();
    }
}

