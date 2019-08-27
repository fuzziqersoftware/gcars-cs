#include "gamecube.h"
#include "gcars.h"

void DEBUG_RenderHexChar(u32 x,u32 y,u8 ch)
{
    char chf;
    if ((ch <= 9)) chf = ('0' + ch);
    if ((ch >= 0xA) && (ch <= 0xF)) chf = ('A' + ch);
    DEBUG_PrintChar(x,y,chf);
}

u8 DEBUG_ToInt(char num)
{
    switch (num)
    {
      case '0': return 0;
      case '1': return 1;
      case '2': return 2;
      case '3': return 3;
      case '4': return 4;
      case '5': return 5;
      case '6': return 6;
      case '7': return 7;
      case '8': return 8;
      case '9': return 9;
      case 'A': return 10;
      case 'B': return 11;
      case 'C': return 12;
      case 'D': return 13;
      case 'E': return 14;
      case 'F': return 15;
    }
    return 0;
}

// GUI Code Entry //////////////////////////////////////////////////////////////

void GCARSEnterCode(GCARS_GAME* game,GCARS_CODE* code)
{
    char* title = "GCARS: New Code: Data";
    //char letters[4][13] = {"1234567890","QWERTYUIOP","ASDFGHJKL","ZXCVBNM"};
    char* hexnums[4] = {"0123","4567","89AB","CDEF"};
    char* buttons[3] = {"Ins Line","Del Line","Done"};
    //char modes[2][10] = {"Encrypted","Decrypted"};
    u32* lines = malloc(8 * 256);//(u32*)0x81600000;
    u8 tallplace = true;
    u32 lineoffset = 0;
    u32 linenum = 0;
    u32 sel_row = 0;
    u32 sel_col = 0;
    u8 value;
    s32 x;

    if (code->numlines > 0) memcpy(lines,code->lines,code->numlines * 8);

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
            if (linenum < code->numlines)
            {
                DEBUG_ShowValueU32(66,140,lines[0 + (2 * linenum)]);
                DEBUG_ShowValueU32(194,140,lines[1 + (2 * linenum)]);
            } else {
                DEBUG_Print(50,140,"................");
            }
            DEBUG_ShowValueU8(338,140,linenum + 1);
            DEBUG_SetTextColor(COLOR_GRAY);

            if (linenum > 1)
            {
                DEBUG_ShowValueU8(338,100,linenum - 1);
                DEBUG_ShowValueU32(66,100,lines[0 + (2 * linenum) - 4]);
                DEBUG_ShowValueU32(194,100,lines[1 + (2 * linenum) - 4]);
            }
            if (linenum > 0)
            {
                DEBUG_ShowValueU8(338,120,linenum);// + 1 - 1);
                DEBUG_ShowValueU32(66,120,lines[0 + (2 * linenum) - 2]);
                DEBUG_ShowValueU32(194,120,lines[1 + (2 * linenum) - 2]);
            }
            if ((s32)(code->numlines - linenum - 1) > 0)
            {
                DEBUG_ShowValueU8(338,160,linenum + 2);
                DEBUG_ShowValueU32(66,160,lines[0 + (2 * linenum) + 2]);
                DEBUG_ShowValueU32(194,160,lines[1 + (2 * linenum) + 2]);
            }
            if ((s32)(code->numlines - linenum - 1) > 1)
            {
                DEBUG_ShowValueU8(338,180,linenum + 3);
                DEBUG_ShowValueU32(66,180,lines[0 + (2 * linenum) + 4]);
                DEBUG_ShowValueU32(194,180,lines[1 + (2 * linenum) + 4]);
            }

            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_ShowValueU8(386,140,code->numlines);

            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,94,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,game->name);
            DEBUG_Print(50,70,code->name);
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,190,hexnums[0]);
            DEBUG_Print(50,210,hexnums[1]);
            DEBUG_Print(50,230,hexnums[2]);
            DEBUG_Print(50,250,hexnums[3]);
            DEBUG_Print(50,280,buttons[0]);
            DEBUG_Print(200,280,buttons[1]);
            DEBUG_Print(350,280,buttons[2]);
            DEBUG_SetTextColor(COLOR_YELLOW);
            if (sel_row == 4) DEBUG_Print(50 + (sel_col * 150),280,buttons[sel_col]);
            else DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),190 + (sel_row * 20),hexnums[sel_row][sel_col]);
            if (tallplace) DEBUG_PrintChar(66 + (lineoffset * 32),146,'_');
            else DEBUG_PrintChar(82 + (lineoffset * 32),146,'_');
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

        csize = LevineReceiveData(MODE_GCARS_ENTER_CODE,CS_PACKET_BUFFER);
        if (csize > 0)
        {
            if ((csize & 7) == 0)
            {
                memcpy(lines,CS_PACKET_BUFFER,csize);
                tallplace = true;
                lineoffset = 0;
                code->numlines = csize >> 3;
                linenum = code->numlines;
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
                if (linenum >= code->numlines)
                {
                    code->numlines++;
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
                    if (code->numlines == 1)
                    {
                        lines[3] = lines[1];
                        lines[2] = lines[0];
                        lines[0] = 0;
                        lines[1] = 0;
                        code->numlines++;
                        break;
                    }
                    for (x = code->numlines + 1; x >= linenum + 1; x--)
                    {
                        lines[(x * 2) + 2] = lines[x * 2];
                        lines[(x * 2) + 3] = lines[(x * 2) + 1];
                    }
                    lines[linenum * 2] = 0;
                    lines[(linenum * 2) + 1] = 0;
                    code->numlines++;
                    break;
                  case 1:
                    if (linenum >= code->numlines) break;
                    memcpy((void*)((u32)lines + (linenum * 8)),(void*)((u32)lines + (linenum * 8) + 8),(code->numlines + 1 - linenum) * 8);
                    code->numlines--;
                    break;
                  case 2:
                    //code->numlines = linenum + 1;
                    free(code->lines);
                    code->lines = malloc(code->numlines * 8);
                    memcpy(code->lines,lines,code->numlines * 8);
                    free(lines);
                    return;
                }
            }
            draw = 1;
        }
        
        if (pad.Digital.Start)
        {
            free(code->lines);
            code->lines = malloc(code->numlines * 8);
            memcpy(code->lines,lines,code->numlines * 8);
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
            if (linenum < code->numlines) linenum++;
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

void GCARSEnterCodeName(GCARS_GAME* game,GCARS_CODE* code)
{
    char* title = "GCARS: New Code: Name";
    char* uletters[4] = {"1234567890-=","QWERTYUIOP[]","ASDFGHJKL:\" ","ZXCVBNM<>?  "};
    char* lletters[4] = {"1234567890_+","qwertyuiop[]","asdfghjkl;\' ","zxcvbnm,./  "};
    char* buttons[3] = {"Caps","Cancel","Done"};
    char name[64];
    u32 nameoffset = 0;
    u8 caps = true;
    u8 startedwith;
    u32 sel_row = 2;
    u32 sel_col = 4;
    u32 x;

    for (x = 0; x < 64; x++) name[x] = 0;

    if (code != NULL)
    {
        startedwith = true;
        strcpy(name,code->name);
        nameoffset = strlen(name);
    } else {
        startedwith = false;
        code = (GCARS_CODE*)malloc(sizeof(GCARS_GAME));
        AddCode(game,code);
        code->enabled = false;
    }

    u8 draw = 1;
    u8 selected = 0;
    u32 csize;
    PAD pad;
    for (;;)
    {
        if (draw)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,game->name);
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,100,name);
            if (caps)
            {
                DEBUG_Print(50,150,uletters[0]);
                DEBUG_Print(50,170,uletters[1]);
                DEBUG_Print(50,190,uletters[2]);
                DEBUG_Print(50,210,uletters[3]);
                DEBUG_Print(150,240,buttons[1]);
                DEBUG_Print(300,240,buttons[2]);
                DEBUG_SetTextColor(COLOR_AQUA);
                DEBUG_Print(50,240,buttons[0]);
            } else {
                DEBUG_Print(50,150,lletters[0]);
                DEBUG_Print(50,170,lletters[1]);
                DEBUG_Print(50,190,lletters[2]);
                DEBUG_Print(50,210,lletters[3]);
                DEBUG_Print(50,240,buttons[0]);
                DEBUG_Print(150,240,buttons[1]);
                DEBUG_Print(300,240,buttons[2]);
            }
            DEBUG_SetTextColor(COLOR_YELLOW);
            if (sel_row == 4)
            {
                switch (sel_col)
                {
                  case 0:
                    if (caps) DEBUG_SetTextColor(COLOR_LIME);
                    DEBUG_Print(50,240,buttons[0]);
                    break;
                  case 1:
                    DEBUG_Print(150,240,buttons[1]);
                    break;
                  case 2:
                    DEBUG_Print(300,240,buttons[2]);
                    break;
                }
            } else {
                if (uletters[sel_row][sel_col] != ' ')
                {
                    if (caps) DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),150 + (sel_row * 20),uletters[sel_row][sel_col]);
                    else DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),150 + (sel_row * 20),lletters[sel_row][sel_col]);
                }
            }
            DEBUG_SetTextColor(COLOR_YELLOW);
            DEBUG_PrintChar(50 + ((nameoffset + 1) * 16),106,'_');
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

        csize = LevineReceiveData(MODE_GCARS_ENTER_CODENAME,CS_PACKET_BUFFER);
        if (csize > 0)
        {
            memcpy(name,CS_PACKET_BUFFER,csize);
            nameoffset = strlen(name);
            sel_row = 4;
            sel_col = 2;
            draw = 1;
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (sel_row < 4) sel_row++;
            else sel_row = 0;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (sel_row > 0) sel_row--;
            else sel_row = 4;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Left || (pad.Analog.X < -0x40) || (pad.AnalogC.X < -0x40))
        {
            if (sel_col > 0) sel_col--;
            else {
                if (sel_row < 4) sel_col = 11;
                else sel_col = 2;
            }
            draw = 1;
        }
        if (pad.Digital.Right || (pad.Analog.X > 0x40) || (pad.AnalogC.X > 0x40))
        {
            if (sel_row < 4)
            {
                if (sel_col == 11) sel_col = 0;
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
                if (caps) name[nameoffset] = uletters[sel_row][sel_col];
                else name[nameoffset] = lletters[sel_row][sel_col];
                if (nameoffset < 63) nameoffset++;
            } else {
                switch (sel_col)
                {
                  case 0:
                    caps = !caps;
                    break;
                  case 1:
                    if (!startedwith)
                    {
                        RemoveCode(game,code);
                        free(code);
                    }
                    return;
                  case 2:
                    strcpy(code->name,name);
                    GCARSEnterCode(game,code);
                    return;
                }
            }
            draw = 1;
        }
        
        if (pad.Digital.Start)
        {
            sel_row = 4;
            sel_col = 2;
            draw = 1;
        }

        if (pad.Digital.B)
        {
            if (nameoffset == 0)
            {
                sel_row = 4;
                sel_col = 1;
            } else {
                nameoffset--;
                name[nameoffset] = 0;
            }
            draw = 1;
        }

        if (pad.Digital.L)
        {
            caps = !caps;
            draw = 1;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void GCARSGetGameID(GCARS_GAME* game)
{
    char* title = "GCARS: Enter Game ID";
    char* letters[4] = {"1234567890","QWERTYUIOP","ASDFGHJKL ", "ZXCVBNM   "};
    char* buttons[3] = {"Region","Type","Done"};
    char gameid[5];
    gameid[0] = *(u8*)((u32)(&game->gameid));
    gameid[1] = *(u8*)((u32)(&game->gameid) + 1);
    gameid[2] = *(u8*)((u32)(&game->gameid) + 2);
    gameid[3] = *(u8*)((u32)(&game->gameid) + 3);
    gameid[4] = 0;
    u32 nameoffset = 1;
    u8 caps = true;
    u32 sel_row = 2;
    u32 sel_col = 4;
    u32 x;
    int err;

    u8 draw = 1;
    u8 selected = 0;
    u8 dvd = 0;
    u32 csize;
    PAD pad;
    for (;;)
    {
        if (draw)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,94,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,title);
            DEBUG_Print(50,70,game->name);
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,120,gameid);
            DEBUG_Print(50,170,letters[0]);
            DEBUG_Print(50,190,letters[1]);
            DEBUG_Print(50,210,letters[2]);
            DEBUG_Print(50,230,letters[3]);
            DEBUG_Print(50,270,buttons[0]);
            DEBUG_Print(200,270,buttons[1]);
            DEBUG_Print(300,270,buttons[2]);
            DEBUG_SetTextColor(COLOR_YELLOW);
            if (sel_row == 4)
            {
                switch (sel_col)
                {
                  case 0:
                    DEBUG_Print(50,270,buttons[0]);
                    break;
                  case 1:
                    DEBUG_Print(200,270,buttons[1]);
                    break;
                  case 2:
                    DEBUG_Print(300,270,buttons[2]);
                    break;
                }
            } else {
                if (letters[sel_row][sel_col] != ' ') DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),170 + (sel_row * 20),letters[sel_row][sel_col]);
            }
            DEBUG_PrintChar(66 + (nameoffset * 16),126,'_');
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

        csize = LevineReceiveData(MODE_GCARS_ENTER_GAMEID,CS_PACKET_BUFFER);
        if (csize == 4)
        {
            memcpy(&gameid[0],CS_PACKET_BUFFER,4);
            sel_row = 4;
            sel_col = 2;
            draw = 1;
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (sel_row < 4) sel_row++;
            else sel_row = 0;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (sel_row > 0) sel_row--;
            else sel_row = 4;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Left || (pad.Analog.X < -0x40) || (pad.AnalogC.X < -0x40))
        {
            if (sel_col > 0) sel_col--;
            else {
                if (sel_row < 4) sel_col = 9;
                else sel_col = 2;
            }
            draw = 1;
        }
        if (pad.Digital.Right || (pad.Analog.X > 0x40) || (pad.AnalogC.X > 0x40))
        {
            if (sel_row < 4)
            {
                if (sel_col == 9) sel_col = 0;
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
                gameid[nameoffset] = letters[sel_row][sel_col];
                if (nameoffset == 2) nameoffset = 1;
                else nameoffset = 2;
            } else {
                switch (sel_col)
                {
                  case 0:
                    if (gameid[3] == 'E') gameid[3] = 'P';
                    else if (gameid[3] == 'P') gameid[3] = 'J';
                    else if (gameid[3] == 'J') gameid[3] = 'D';
                    else if (gameid[3] == 'D') gameid[3] = 'U';
                    else if (gameid[3] == 'U') gameid[3] = 'E';
                    else gameid[3] = 'E';
                    break;
                  case 1:
                    if (gameid[0] == 'G') gameid[0] = 'D';
                    else gameid[0] = 'G';
                    break;
                  case 2:
                    game->gameid = *(u32*)(gameid);
                    return;
                }
            }
            draw = 1;
        }

        if (pad.Digital.Start)
        {
            game->gameid = *(u32*)(gameid);
            return;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

GCARS_GAME* GCARSEnterGameName(GCARS_GAME* game)
{
    char* title = "GCARS: Enter Game Name";
    char* uletters[4] = {"1234567890-=","QWERTYUIOP[]","ASDFGHJKL:\" ", "ZXCVBNM<>?  "};
    char* lletters[4] = {"1234567890_+","qwertyuiop[]","asdfghjkl;\' ","zxcvbnm,./  "};
    char* buttons[3] = {"Caps","Cancel","Done"};
    char name[64];
    u32 nameoffset = 0;
    u8 caps = true;
    u8 startedwith;
    u32 sel_row = 2;
    u32 sel_col = 4;
    u32 x;

    memset(name,0,64);

    if (game != NULL)
    {
        startedwith = true;
        strcpy(name,game->name);
        nameoffset = strlen(name);
    } else {
        startedwith = false;
        game = (GCARS_GAME*)malloc(sizeof(GCARS_GAME));
        AddGame(datablock,game);
        game->frames = 4;
    }

    u8 draw = 1;
    u8 selected = 0;
    u8 dvd = 0;
    u32 csize;
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
            DEBUG_Print(50,100,name);
            if (caps)
            {
                DEBUG_Print(50,150,uletters[0]);
                DEBUG_Print(50,170,uletters[1]);
                DEBUG_Print(50,190,uletters[2]);
                DEBUG_Print(50,210,uletters[3]);
                DEBUG_Print(150,240,buttons[1]);
                DEBUG_Print(300,240,buttons[2]);
                DEBUG_SetTextColor(COLOR_AQUA);
                DEBUG_Print(50,240,buttons[0]);
            } else {
                DEBUG_Print(50,150,lletters[0]);
                DEBUG_Print(50,170,lletters[1]);
                DEBUG_Print(50,190,lletters[2]);
                DEBUG_Print(50,210,lletters[3]);
                DEBUG_Print(50,240,buttons[0]);
                DEBUG_Print(150,240,buttons[1]);
                DEBUG_Print(300,240,buttons[2]);
            }
            DEBUG_SetTextColor(COLOR_YELLOW);
            if (sel_row == 4)
            {
                switch (sel_col)
                {
                  case 0:
                    if (caps) DEBUG_SetTextColor(COLOR_LIME);
                    DEBUG_Print(50,240,buttons[0]);
                    break;
                  case 1:
                    DEBUG_Print(150,240,buttons[1]);
                    break;
                  case 2:
                    DEBUG_Print(300,240,buttons[2]);
                    break;
                }
            } else {
                if (uletters[sel_row][sel_col] != ' ')
                {
                    if (caps) DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),150 + (sel_row * 20),uletters[sel_row][sel_col]);
                    else DEBUG_PrintChar(66 + (sel_col * DEBUG_TEXT_CHAR_WIDTH),150 + (sel_row * 20),lletters[sel_row][sel_col]);
                }
            }
            DEBUG_SetTextColor(COLOR_YELLOW);
            DEBUG_PrintChar(50 + ((nameoffset + 1) * 16),106,'_');
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

        csize = LevineReceiveData(MODE_GCARS_ENTER_GAMENAME,CS_PACKET_BUFFER);
        if (csize > 0)
        {
            memcpy(name,CS_PACKET_BUFFER,csize);
            nameoffset = strlen(name);
            sel_row = 4;
            sel_col = 2;
            draw = 1;
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (sel_row < 4) sel_row++;
            else sel_row = 0;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (sel_row > 0) sel_row--;
            else sel_row = 4;
            if ((sel_row == 4) && (sel_col > 2)) sel_col = 2;
            draw = 1;
        }
        if (pad.Digital.Left || (pad.Analog.X < -0x40) || (pad.AnalogC.X < -0x40))
        {
            if (sel_col > 0) sel_col--;
            else {
                if (sel_row < 4) sel_col = 11;
                else sel_col = 2;
            }
            draw = 1;
        }
        if (pad.Digital.Right || (pad.Analog.X > 0x40) || (pad.AnalogC.X > 0x40))
        {
            if (sel_row < 4)
            {
                if (sel_col == 11) sel_col = 0;
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
                if (caps) name[nameoffset] = uletters[sel_row][sel_col];
                else name[nameoffset] = lletters[sel_row][sel_col];
                if (nameoffset < 63) nameoffset++;
            } else {
                switch (sel_col)
                {
                  case 0:
                    caps = !caps;
                    break;
                  case 1:
                    if (!startedwith)
                    {
                        RemoveGame(datablock,game);
                        free(game);
                    }
                    return NULL;
                  case 2:
                    strcpy(game->name,name);
                    if (!startedwith) game->gameid = 0x47383845;
                    return game;
                }
            }
            draw = 1;
        }
        
        if (pad.Digital.Start)
        {
            strcpy(game->name,name);
            if (!startedwith) game->gameid = 0x47383845;
            return game;
        }

        if (pad.Digital.B)
        {
            if (nameoffset == 0)
            {
                sel_row = 4;
                sel_col = 1;
            } else {
                nameoffset--;
                name[nameoffset] = 0;
            }
            draw = 1;
        }

        if (pad.Digital.L)
        {
            caps = !caps;
            draw = 1;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

