#include "gamecube.h"
#include "gcars.h"

// GUI Code Selection //////////////////////////////////////////////////////////

void GCARSCodes(GCARS_GAME* game)
{
    u32 swaps = 0xFFFFFFFF;
    GCARS_CODE* temp;

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
            DEBUG_Print(50,50,game->name);
            DEBUG_SetTextColor(COLOR_WHITE);

            if (game->numcodes != 0)
            {
                DEBUG_SetTextColor(COLOR_WHITE);
                for (x = 0; x < 12; x++)
                {
                    if (x >= game->numcodes) break;
                    if (x == (selected - scroll))
                    {
                        if (game->codes[x + scroll]->enabled)
                        {
                            DEBUG_SetTextColor(COLOR_LIME);
                            DEBUG_Print(50,(100 + (x * 20)),game->codes[x + scroll]->name);
                        } else {
                            DEBUG_SetTextColor(COLOR_YELLOW);
                            DEBUG_Print(50,(100 + (x * 20)),game->codes[x + scroll]->name);
                        }
                    } else {
                        if (game->codes[x + scroll]->enabled)
                        {
                            DEBUG_SetTextColor(COLOR_AQUA);
                            DEBUG_Print(50,(100 + (x * 20)),game->codes[x + scroll]->name);
                        } else {
                            DEBUG_SetTextColor(COLOR_WHITE);
                            DEBUG_Print(50,(100 + (x * 20)),game->codes[x + scroll]->name);
                        }
                    }
                    DEBUG_SetTextColor(COLOR_WHITE);
                    if (x + scroll == swaps)
                    {
                        DEBUG_Print(18,(100 + (x * 20)),"*");
                        DEBUG_Print(530,(100 + (x * 20)),"*");
                    }
                }
                if (game->numcodes > 12)
                {
                    if (scroll == 0) DEBUG_SetTextColor(COLOR_GRAY50);
                    else DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(34,100,"{");
                    if (scroll == (game->numcodes - 12)) DEBUG_SetTextColor(COLOR_GRAY50);
                    else DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(34,320,"}");
                    DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(34,120 + ((scroll * 180) / (game->numcodes - 12)),"¿");
                }
            } else {
                DEBUG_SetTextColor(COLOR_RED);
                DEBUG_Print(50,100,"No code entries");
                DEBUG_Print(50,120,"Press R to add a code entry");
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

        LevineReceiveData(0,CS_PACKET_BUFFER);

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < game->numcodes - 1)
            {
                selected++;
                if (selected > (scroll + 11)) scroll++;
            } else {
                selected = 0;
                scroll = 0;
            }
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0)
            {
                selected--;
                if (selected < scroll) scroll--;
            } else {
                selected = game->numcodes - 1;
                scroll = game->numcodes - 12;
            }
            draw = 1;
        }

        if (pad.Digital.A)
        {
            if (selected < game->numcodes) (game->codes[selected]->enabled) = !(game->codes[selected]->enabled);
            draw = 1;
        }
        
        if (pad.Digital.Start)
        {
            GCARSSaveCodes();
            GCARSStartGame(game);
            draw = 1;
        }

        if (pad.Digital.Z)
        {
            if (swaps != 0xFFFFFFFF)
            {
                temp = game->codes[swaps];
                game->codes[swaps] = game->codes[selected];
                game->codes[selected] = temp;
                swaps = 0xFFFFFFFF;
            } else {
                swaps = selected;
            }
            draw = 1;
        }

#ifndef DISABLE_EDITORS
        if (pad.Digital.Y)
        {
            GCARSEnterCodeName(game,game->codes[selected]);
            GCARSSaveCodes();
            draw = 1;
        }
        if (pad.Digital.R)
        {
            GCARSEnterCodeName(game,(GCARS_CODE*)NULL);
            GCARSSaveCodes();
            draw = 1;
        }
        if (pad.Digital.L)
        {
            if (game->numcodes > 0)
            {
                RemoveCodeID(game,selected);
                FS_PAD_WaitEmptyAll();
                if (selected >= game->numcodes) selected = game->numcodes - 1;
                GCARSSaveCodes();
                draw = 1;
            }
        }
#endif

        if (pad.Digital.B) return;

        if (scroll < 0) scroll = 0;
        if (scroll > (game->numcodes - 12)) scroll = (game->numcodes - 12);

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void GCARSGames()
{
    char* title = "GCARS: Select Game";
    u32 swaps = 0xFFFFFFFF;
    GCARS_GAME* temp;

    u32 x;
    u8 draw = 1;
    u32 selected = 0;
    s32 scroll = 0;
    PAD pad;
    GCARS_GAME* game;
    u32 csize;
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
            if (datablock->numgames != 0)
            {
                DEBUG_SetTextColor(COLOR_WHITE);
                for (x = 0; x < 12; x++)
                {
                    if (x >= datablock->numgames) break;
                    DEBUG_Print(50,(100 + (x * 20)),datablock->games[scroll + x]->name);
                    if (x + scroll == swaps)
                    {
                        DEBUG_Print(18,(100 + (x * 20)),"*");
                        DEBUG_Print(530,(100 + (x * 20)),"*");
                    }
                }
                DEBUG_SetTextColor(COLOR_YELLOW);
                DEBUG_Print(50,(100 + ((selected - scroll) * 20)),datablock->games[selected]->name);
                if (datablock->numgames > 12)
                {
                    if (scroll == 0) DEBUG_SetTextColor(COLOR_GRAY50);
                    else DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(34,100,"{");
                    if (scroll == (datablock->numgames - 12)) DEBUG_SetTextColor(COLOR_GRAY50);
                    else DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(34,320,"}");
                    DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(34,120 + ((scroll * 180) / (datablock->numgames - 12)),"¿");
                }
            } else {
                DEBUG_SetTextColor(COLOR_RED);
                DEBUG_Print(50,100,"No game entries");
                DEBUG_Print(50,120,"Press R to add a game entry");
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

        csize = LevineReceiveData(MODE_GCARS_ENTER_GAME,CS_PACKET_BUFFER);
        if (csize > 0)
        {
            game = (GCARS_GAME*)malloc(sizeof(GCARS_GAME));
            AddGame(datablock,game);
            memcpy(game,CS_PACKET_BUFFER,sizeof(GCARS_GAME));
            game->frames = 4;
            GCARSSaveCodes();
            selected = datablock->numgames - 1;
            scroll = datablock->numgames - 12;
            if (scroll < 0) scroll = 0;
            draw = 1;
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < datablock->numgames - 1)
            {
                selected++;
                if (selected > (scroll + 11)) scroll++;
            } else {
                selected = 0;
                scroll = 0;
            }
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0)
            {
                selected--;
                if (selected < scroll) scroll--;
            } else {
                selected = datablock->numgames - 1;
                scroll = datablock->numgames - 12;
            }
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            if (selected < datablock->numgames)
            {
                GCARSCodes(datablock->games[selected]);
                draw = 1;
            }
        }

        if (pad.Digital.Z)
        {
            if (swaps != 0xFFFFFFFF)
            {
                temp = datablock->games[swaps];
                datablock->games[swaps] = datablock->games[selected];
                datablock->games[selected] = temp;
                swaps = 0xFFFFFFFF;
            } else {
                swaps = selected;
            }
            draw = 1;
        }

#ifndef DISABLE_EDITORS
        if (pad.Digital.R)
        {
            GCARS_GAME* tg = GCARSEnterGameName((GCARS_GAME*)NULL);
            if (tg != NULL)
            {
                CSGameOptions(tg);
                GCARSSaveCodes();
            }
            draw = 1;
        }
        if (pad.Digital.L)
        {
            if (datablock->numgames > 0)
            {
                RemoveGameID(datablock,selected);
                FS_PAD_WaitEmptyAll();
                if (selected >= datablock->numgames) selected = datablock->numgames - 1;
                GCARSSaveCodes();
                draw = 1;
            }
        }
        if (pad.Digital.Y)
        {
            CSGameOptions(datablock->games[selected]);
            GCARSSaveCodes();
            draw = 1;
        }
#endif

        if (pad.Digital.B) return;

        if (scroll < 0) scroll = 0;
        if (scroll > (datablock->numgames - 12)) scroll = (datablock->numgames - 12);

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}
