#include "gamecube.h"
#include "cs.h"

void NETGUIToDec8(char* buffer,u8 num)
{
    buffer[0] = buffer[1] = buffer[2] = buffer[3] = 0;
    if (num < 100) buffer[0] = '0';
    else if (num < 200)
    {
        num -= 100;
        buffer[0] = '1';
    } else {
        num -= 200;
        buffer[0] = '2';
    }
    if (num < 10) buffer[1] = '0';
    else if (num < 20)
    {
        num -= 10;
        buffer[1] = '1';
    } else if (num < 30)
    {
        num -= 20;
        buffer[1] = '2';
    } else if (num < 40)
    {
        num -= 30;
        buffer[1] = '3';
    } else if (num < 50)
    {
        num -= 40;
        buffer[1] = '4';
    } else if (num < 60)
    {
        num -= 50;
        buffer[1] = '5';
    } else if (num < 70)
    {
        num -= 60;
        buffer[1] = '6';
    } else if (num < 80)
    {
        num -= 70;
        buffer[1] = '7';
    } else if (num < 90)
    {
        num -= 80;
        buffer[1] = '8';
    } else {
        num -= 90;
        buffer[1] = '9';
    }
    buffer[2] = (0x30 + num);
}

void NETGUIConvertIPAddress(char* buffer,u32 ip)
{
    u8 ipbuffer[4];
    memcpy(ipbuffer,&ip,4);
    NETGUIToDec8(&buffer[0],(ip >> 24) & 0xFF);
    NETGUIToDec8(&buffer[4],(ip >> 16) & 0xFF);
    NETGUIToDec8(&buffer[8],(ip >> 8) & 0xFF);
    NETGUIToDec8(&buffer[12],(ip) & 0xFF);
    buffer[3] = '.';
    buffer[7] = '.';
    buffer[11] = '.';
    buffer[15] = 0;
}

void NETGUIPrintIPAddress(u32 x,u32 y,u32 ip)
{
    char buffer[4];
    u8 ipbuffer[4];
    memcpy(ipbuffer,&ip,4);
    NETGUIToDec8(buffer,(ip >> 24) & 0xFF);
    DEBUG_Print(x,y,buffer);
    NETGUIToDec8(buffer,(ip >> 16) & 0xFF);
    DEBUG_Print(x + 64,y,buffer);
    NETGUIToDec8(buffer,(ip >> 8) & 0xFF);
    DEBUG_Print(x + 128,y,buffer);
    NETGUIToDec8(buffer,(ip) & 0xFF);
    DEBUG_Print(x + 192,y,buffer);
    DEBUG_PrintChar(x + 64,y,'.');
    DEBUG_PrintChar(x + 128,y,'.');
    DEBUG_PrintChar(x + 192,y,'.');
}

void NETGUIPrintMACAddress(u32 x,u32 y,u8* mac)
{
    x += 16;
    u32 z;
    for (z = 0; z < 6; z++)
    {
        DEBUG_ShowValueU8(x + (z * 48),y,mac[z]);
        if (z != 5) DEBUG_PrintChar(x + 32 + (z * 48),y,':');
    }
}

void NETGUIEnterU32(u32* val,char* title)
{
    char* hexnums[4] = {"0123","4567","89AB","CDEF"};
    char button[5] = "Done";

    u8 tallplace = true;
    u32 lineoffset = 0;
    u32 sel_row = 0;
    u32 sel_col = 0;
    u8 value;
    s32 x;

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
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,150,hexnums[0]);
            DEBUG_Print(50,170,hexnums[1]);
            DEBUG_Print(50,190,hexnums[2]);
            DEBUG_Print(50,210,hexnums[3]);
            DEBUG_Print(50,240,button);
            DEBUG_ShowValueU32(66,100,*val);
            DEBUG_SetTextColor(COLOR_YELLOW);
            if (sel_row == 4) DEBUG_Print(50,240,button);
            else DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),150 + (sel_row * 20),hexnums[sel_row][sel_col]);
            if (tallplace) DEBUG_PrintChar(66 + (lineoffset * 32),106,'_');
            else DEBUG_PrintChar(82 + (lineoffset * 32),106,'_');
            debugDrawInfo();
            //draw = 0;
            //FS_PAD_WaitEmpty(FS_PAD_CHANNEL_0);
        }

        while (draw)
        {
            FS_PAD_ReadAllState(&pad);
            if (FS_PAD_IsEmpty(&pad)) draw = 0;
            else FS_VIDEO_WaitVSync();
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40))
        {
            if (sel_row < 4) sel_row++;
            else sel_row = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40))
        {
            if (sel_row > 0) sel_row--;
            else sel_row = 4;
            draw = 1;
        }
        if (pad.Digital.Left || (pad.Analog.X < -0x40))
        {
            if (sel_col > 0) sel_col--;
            else sel_col = 3;
            draw = 1;
        }
        if (pad.Digital.Right || (pad.Analog.X > 0x40))
        {
            if (sel_col < 3) sel_col++;
            else sel_col = 0;
            draw = 1;
        }

        if (pad.Digital.A)
        {
            if (sel_row < 4)
            {
                value = DEBUG_ToInt(hexnums[sel_row][sel_col]);
                if (tallplace)
                {
                    value = value << 4;
                    *(u8*)((u32)val + lineoffset) &= 0x0F;
                    *(u8*)((u32)val + lineoffset) |= value;
                    tallplace = false;
                } else {
                    *(u8*)((u32)val + lineoffset) &= 0xF0;
                    *(u8*)((u32)val + lineoffset) |= value;
                    if (lineoffset < 3)
                    {
                        tallplace = true;
                        lineoffset++;
                    }
                }
            } else {
                return;
            }
            draw = 1;
        }
        
        if (pad.Digital.Start || pad.Digital.B) return;

        if (pad.AnalogC.X > 0x40)
        {
            if (!((lineoffset == 3) && !tallplace))
            {
                if (tallplace)
                {
                    tallplace = false;
                } else {
                    tallplace = true;
                    lineoffset++;
                }
            }
            draw = 1;
        }
        if (pad.AnalogC.X < -0x40)
        {
            if (!((lineoffset == 0) && tallplace))
            {
                if (tallplace)
                {
                    tallplace = false;
                    lineoffset--;
                } else {
                    tallplace = true;
                }
            }
            draw = 1;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void NETGUIEnterIPAddress(u32* ip,char* title)
{
    char button[5] = "Done";

    u8* tempip = (u8*)(ip);
    u8 place = 2;
    u8 octet = 0;
    u32 sel_row = 0;
    u32 sel_col = 0;
    u8 value;
    s32 x;

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
            DEBUG_SetTextColor(COLOR_WHITE);
            NETGUIPrintIPAddress(50,100,*ip);
            DEBUG_SetTextColor(COLOR_YELLOW);
            DEBUG_Print(50,150,button);
            DEBUG_PrintChar(50 + ((3 - place) * 16) + (64 * octet),106,'_');
            debugDrawInfo();
            //draw = 0;
            //FS_PAD_WaitEmpty(FS_PAD_CHANNEL_0);
        }

        while (draw)
        {
            FS_PAD_ReadAllState(&pad);
            if (FS_PAD_IsEmpty(&pad)) draw = 0;
            else FS_VIDEO_WaitVSync();
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Left || (pad.AnalogC.X < -0x40) || (pad.Analog.X < -0x40))
        {
            if (place < 2) place++;
            else {
                if (octet == 0)
                {
                    octet = 3;
                    place = 0;
                } else {
                    octet--;
                    place = 0;
                }
            }
            draw = 1;
        }
        if (pad.Digital.Right || (pad.AnalogC.X > 0x40) || (pad.Analog.X > 0x40))
        {
            if (place > 0) place--;
            else {
                if (octet == 3)
                {
                    octet = 0;
                    place = 2;
                } else {
                    octet++;
                    place = 2;
                }
            }
            draw = 1;
        }
        if (pad.Digital.Up || (pad.AnalogC.Y < -0x40) || (pad.Analog.Y < -0x40))
        {
            switch (place)
            {
              case 2:
                if (tempip[octet] >= 200) tempip[octet] -= 200;
                else tempip[octet] += 100;
                break;
              case 1:
                if (tempip[octet] >= 250) tempip[octet] -= 250;
                else tempip[octet] += 10;
                break;
              case 0:
                if (tempip[octet] >= 255) tempip[octet] = 0;
                else tempip[octet]++;
                break;
            }
            draw = 1;
        }
        if (pad.Digital.Down || (pad.AnalogC.Y > 0x40) || (pad.Analog.Y > 0x40))
        {
            switch (place)
            {
              case 2:
                if (tempip[octet] < 100) tempip[octet] += 200;
                else tempip[octet] -= 100;
                break;
              case 1:
                if (tempip[octet] < 10) tempip[octet] += 250;
                else tempip[octet] -= 10;
                break;
              case 0:
                if (tempip[octet] == 0) tempip[octet] = 255;
                else tempip[octet]--;
                break;
            }
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.B || pad.Digital.Start) return;

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void NETGUISelectBBASpeed()
{
    char* title = "GCARS-CS: Select BBA Speed";

    char* menu1 = "10Mbps  / Half Duplex";
    char* menu2 = "10Mbps  / Full Duplex";
    char* menu3 = "100Mbps / Half Duplex";
    char* menu4 = "100Mbps / Full Duplex";
    char* menu5 = "Auto    / Half Duplex";
    char* menu6 = "Auto    / Full Duplex";

    u32 x;
    u8 draw = 1;
    u8 selected;
    PAD pad;

    selected = ((CS_ENTITY->speed <= 5) ? CS_ENTITY->speed : 0);

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
            }
            debugDrawInfo();
            //draw = 0;
            //FS_PAD_WaitEmpty(FS_PAD_CHANNEL_0);
        }

        while (draw)
        {
            FS_PAD_ReadAllState(&pad);
            if (FS_PAD_IsEmpty(&pad)) draw = 0;
            else FS_VIDEO_WaitVSync();
        }

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

        if (pad.Digital.A || pad.Digital.Start || pad.Digital.B)
        {
            CS_ENTITY->speed = selected;
            eth_setspeed(CS_ENTITY->speed | 0x80);
            return;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

