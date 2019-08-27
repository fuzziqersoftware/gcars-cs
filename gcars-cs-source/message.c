#include "gamecube.h"
#include "memorymap.h"

void GCARSClearFrameBuffer(u32 color)
{
    u32 x;
    for (x = 0; x < (320 * 480); x++) (fb)[x] = color;
}

void GCARSError(char* error)
{
    char* title = "Fuzziqer Software GCARS-CS";
    char* errtitle = "Press A or B to continue.";
    GCARSClearFrameBuffer(COLOR_BLACK);
    GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
    GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
    DEBUG_SetTextColor(COLOR_QRHYGREEN);
    DEBUG_Print(50,50,title);
    DEBUG_SetTextColor(COLOR_WHITE);
    DEBUG_Print(50,100,errtitle);
    DEBUG_SetTextColor(COLOR_YELLOW);
    DEBUG_Print(50,150,error);
    debugDrawInfo();
    FS_PAD_WaitEmptyAll();

    PAD pad;
    for (;;)
    {
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.B || pad.Digital.A)
        {
            FS_PAD_WaitEmptyAll();
            break;
        }
        FS_VIDEO_WaitVSync();
    }
}

void GCARSErrorNum(char* error,u32 num)
{
    char* title = "Fuzziqer Software GCARS-CS";
    char* errtitle = "Press A or B to continue.";
    GCARSClearFrameBuffer(COLOR_BLACK);
    GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
    GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
    DEBUG_SetTextColor(COLOR_QRHYGREEN);
    DEBUG_Print(50,50,title);
    DEBUG_SetTextColor(COLOR_WHITE);
    DEBUG_Print(50,100,errtitle);
    DEBUG_ShowValueU32(66,120,num);
    DEBUG_SetTextColor(COLOR_YELLOW);
    DEBUG_Print(50,150,error);
    debugDrawInfo();
    FS_PAD_WaitEmptyAll();

    PAD pad;
    for (;;)
    {
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.B || pad.Digital.A)
        {
            FS_PAD_WaitEmptyAll();
            break;
        }
        FS_VIDEO_WaitVSync();
    }
}

void GCARSStatus(char* status)
{
    char* title = "Fuzziqer Software GCARS-CS";
    GCARSClearFrameBuffer(COLOR_BLACK);
    GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
    GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
    DEBUG_SetTextColor(COLOR_QRHYGREEN);
    DEBUG_Print(50,50,title);
    DEBUG_SetTextColor(COLOR_YELLOW);
    DEBUG_Print(50,100,status);
    debugDrawInfo();
}

/*u8 GCARSPrompt(char* title,char* ch1,char* ch2,char* ch3,u8 back)
{
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
            DEBUG_Print(50,50,"Fuzziqer Software GCARS-CS");
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,100,title);
            DEBUG_Print(50,150,ch1);
            DEBUG_Print(50,170,ch2);
            DEBUG_Print(50,190,ch3);
            DEBUG_SetTextColor(COLOR_YELLOW);
            switch (selected)
            {
              case 0:
                DEBUG_Print(50,150,ch1);
                break;
              case 1:
                DEBUG_Print(50,170,ch2);
                break;
              case 2:
                DEBUG_Print(50,190,ch3);
                break;
            }
            debugDrawInfo();
        }

        while (draw)
        {
            TCBDraw(&tcb);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,title);

            FS_PAD_ReadAllState(&pad);
            if (FS_PAD_IsEmpty(&pad)) draw = 0;
            else FS_VIDEO_WaitVSync();
        }

        TCBDraw(&tcb);
        GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
        GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
        DEBUG_SetTextColor(COLOR_QRHYGREEN);
        DEBUG_Print(50,50,title);

        LevineReceiveData(0,CS_PACKET_BUFFER);

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < 3) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 3;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start) return selected;
        if (pad.Digital.B) return back;

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
} */

