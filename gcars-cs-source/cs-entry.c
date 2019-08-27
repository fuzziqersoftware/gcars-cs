#include "gamecube.h"
#include "cs.h"

void CSEnterCode(GCARS_GAME* game,u32 localPad)
{
    //char letters[4][13] = {"1234567890","QWERTYUIOP","ASDFGHJKL","ZXCVBNM"};
    char* hexnums[4] = {"0123","4567","89AB","CDEF"};
    char* buttons[3] = {"Ins Line","Del Line","Done"};
    char titlebuffer[0x43];
    //char modes[2][10] = {"Encrypted","Decrypted"};
    u32* lines = malloc(8 * 256);//(u32*)0x81600000;
    u8 tallplace = true;
    u32 lineoffset = 0;
    u32 linenum = 0;
    u32 sel_row = 0;
    u32 sel_col = 0;
    u8 value;
    s32 x;

    strcpy(titlebuffer,game->name);
    titlebuffer[strlen(game->name)] = ':';
    titlebuffer[strlen(game->name) + 1] = ' ';
    titlebuffer[strlen(game->name) + 2] = '1' + localPad;
    titlebuffer[strlen(game->name) + 3] = 0x00;
    if (game->numaddress[localPad] > 0) memcpy(lines,game->address[localPad],game->numaddress[localPad] * 8);

    u8 draw = 1;
    u8 selected = 0;
    u32 csize;
    PAD pad;
    for (;;)
    {
        if (draw)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            DEBUG_SetTextColor(COLOR_WHITE);
            if (linenum < game->numaddress[localPad])
            {
                DEBUG_ShowValueU32(66,120,lines[0 + (2 * linenum)]);
                DEBUG_ShowValueU32(194,120,lines[1 + (2 * linenum)]);
            } else {
                DEBUG_Print(50,120,"................");
            }
            DEBUG_ShowValueU8(338,120,linenum + 1);
            DEBUG_SetTextColor(COLOR_GRAY);

            if (linenum > 1)
            {
                DEBUG_ShowValueU8(338,80,linenum - 1);
                DEBUG_ShowValueU32(66,80,lines[0 + (2 * linenum) - 4]);
                DEBUG_ShowValueU32(194,80,lines[1 + (2 * linenum) - 4]);
            }
            if (linenum > 0)
            {
                DEBUG_ShowValueU8(338,100,linenum);// + 1 - 1);
                DEBUG_ShowValueU32(66,100,lines[0 + (2 * linenum) - 2]);
                DEBUG_ShowValueU32(194,100,lines[1 + (2 * linenum) - 2]);
            }
            if ((s32)(game->numaddress[localPad] - linenum - 1) > 0)
            {
                DEBUG_ShowValueU8(338,140,linenum + 2);
                DEBUG_ShowValueU32(66,140,lines[0 + (2 * linenum) + 2]);
                DEBUG_ShowValueU32(194,140,lines[1 + (2 * linenum) + 2]);
            }
            if ((s32)(game->numaddress[localPad] - linenum - 1) > 1)
            {
                DEBUG_ShowValueU8(338,160,linenum + 3);
                DEBUG_ShowValueU32(66,160,lines[0 + (2 * linenum) + 4]);
                DEBUG_ShowValueU32(194,160,lines[1 + (2 * linenum) + 4]);
            }

            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_ShowValueU8(386,120,game->numaddress[localPad]);

            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,titlebuffer);
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,170,hexnums[0]);
            DEBUG_Print(50,190,hexnums[1]);
            DEBUG_Print(50,210,hexnums[2]);
            DEBUG_Print(50,230,hexnums[3]);
            DEBUG_Print(50,260,buttons[0]);
            DEBUG_Print(200,260,buttons[1]);
            DEBUG_Print(350,260,buttons[2]);
            DEBUG_SetTextColor(COLOR_YELLOW);
            if (sel_row == 4) DEBUG_Print(50 + (sel_col * 150),260,buttons[sel_col]);
            else DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),170 + (sel_row * 20),hexnums[sel_row][sel_col]);
            if (tallplace) DEBUG_PrintChar(66 + (lineoffset * 32),126,'_');
            else DEBUG_PrintChar(82 + (lineoffset * 32),126,'_');
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

        csize = LevineReceiveData(MODE_CS_ENTER_CODE,CS_PACKET_BUFFER);
        if (csize > 0)
        {
            if ((csize & 7) == 0)
            {
                memcpy(lines,CS_PACKET_BUFFER,csize);
                tallplace = true;
                lineoffset = 0;
                game->numaddress[localPad] = csize >> 3;
                linenum = game->numaddress[localPad];
                sel_row = 4;
                sel_col = 2;
                draw = 1;
            }
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40))
        {
            if (sel_row < 4) sel_row++;
            else sel_row = 0;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40))
        {
            if (sel_row > 0) sel_row--;
            else sel_row = 4;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Left || (pad.Analog.X < -0x40))
        {
            if (sel_col > 0) sel_col--;
            else {
                if (sel_row < 4) sel_col = 3;
                else sel_col = 2;
            }
            draw = 1;
        }
        if (pad.Digital.Right || (pad.Analog.X > 0x40))
        {
            if (sel_row < 4)
            {
                if (sel_col == 3) sel_col = 0;
                else sel_col++;
            } else {
                if (sel_col == 2) sel_col = 0;
                else sel_col++;
            }
            draw = 1;
        }

        if (pad.Digital.A)
        {
            if (sel_row < 4)
            {
                if (linenum >= game->numaddress[localPad])
                {
                    game->numaddress[localPad]++;
                    lines[(linenum * 2)] = 0;
                    lines[(linenum * 2) + 1] = 0;
                }
                value = DEBUG_ToInt(hexnums[sel_row][sel_col]);
                if (tallplace)
                {
                    value = value << 4;
                    *(u8*)((u32)lines + lineoffset + (linenum * 8)) &= 0x0F;
                    *(u8*)((u32)lines + lineoffset + (linenum * 8)) |= value;
                    tallplace = false;
                } else {
                    *(u8*)((u32)lines + lineoffset + (linenum * 8)) &= 0xF0;
                    *(u8*)((u32)lines + lineoffset + (linenum * 8)) |= value;
                    if (lineoffset < 7)
                    {
                        tallplace = true;
                        lineoffset++;
                    }
                }
            } else {
                switch (sel_col)
                {
                  case 0:
                    // ins line
                    if (game->numaddress[localPad] == 1)
                    {
                        lines[3] = lines[1];
                        lines[2] = lines[0];
                        lines[0] = 0;
                        lines[1] = 0;
                        game->numaddress[localPad]++;
                        break;
                    }
                    for (x = game->numaddress[localPad] + 1; x >= linenum + 1; x--)
                    {
                        lines[(x * 2) + 2] = lines[x * 2];
                        lines[(x * 2) + 3] = lines[(x * 2) + 1];
                    }
                    lines[linenum * 2] = 0;
                    lines[(linenum * 2) + 1] = 0;
                    game->numaddress[localPad]++;
                    break;
                  case 1:
                    if (linenum >= game->numaddress[localPad]) break;
                    memcpy((void*)((u32)lines + (linenum * 8)),(void*)((u32)lines + (linenum * 8) + 8),(game->numaddress[localPad] + 1 - linenum) * 8);
                    game->numaddress[localPad]--;
                    break;
                  case 2:
                    //code->numlines = linenum + 1;
                    free(game->address[localPad]);
                    game->address[localPad] = malloc(game->numaddress[localPad] * 8);
                    memcpy(game->address[localPad],lines,game->numaddress[localPad] * 8);
                    free(lines);
                    return;
                }
            }
            draw = 1;
        }
        
        if (pad.Digital.Start)
        {
            free(game->address[localPad]);
            game->address[localPad] = malloc(game->numaddress[localPad] * 8);
            memcpy(game->address[localPad],lines,game->numaddress[localPad] * 8);
            free(lines);
            return;
        }

        if (pad.Digital.B)
        {
            if ((lineoffset == 0) && tallplace)
            {
                sel_row = 4;
                sel_col = 2;
            } else {
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

        if (pad.AnalogC.Y > 0x40)
        {
            if (linenum < game->numaddress[localPad]) linenum++;
            draw = 1;
        }
        if (pad.AnalogC.Y < -0x40)
        {
            if (linenum > 0) linenum--;
            draw = 1;
        }
        if (pad.AnalogC.X > 0x40)
        {
            if (tallplace)
            {
                tallplace = false;
            } else {
                tallplace = true;
                if (lineoffset == 7) lineoffset = 0;
                else lineoffset++;
            }
            draw = 1;
        }
        if (pad.AnalogC.X < -0x40)
        {
            if (tallplace)
            {
                tallplace = false;
                if (lineoffset == 0) lineoffset = 7;
                else lineoffset--;
            } else {
                tallplace = true;
            }
            draw = 1;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
    free(lines);
}

