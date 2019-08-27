#include "gamecube.h"
#include "cs.h"

void CSGetLocalController()
{
    char* title = "ControlSim: Local Controller";

    char* menu1 = "Controller 1";
    char* menu2 = "Controller 2";
    char* menu3 = "Controller 3";
    char* menu4 = "Controller 4";

    u32 x;
    u8 draw = 1;
    u8 selected;
    PAD pad;

    if (CS_DATA->localPad > 3) CS_DATA->localPad = 0;
    selected = CS_DATA->localPad;

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

        if (pad.Digital.A || pad.Digital.Start)
        {
            CS_DATA->localPad = selected;
            return;
        }

        if (pad.Digital.B) return;

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void CSGameOptions(GCARS_GAME* game)
{
    u32 x;
    u8 draw = 1;
    u32 selected = 0;
    s32 scroll = 0;
    PAD pad;
    char* menu[11] = {"Edit game name","Game ID","Number of codes",
                      "Script 1 size","Script 2 size","Script 3 size",
                      "Script 4 size","Pad address","Update speed","Flags",
                      "Done"};
    char gameid[8] = {0,0,0,0,0,0,0,0};
    for (;;)
    {
        if (draw)
        {
            memcpy(gameid,&game->gameid,4);
            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,game->name);
            DEBUG_SetTextColor(selected == 0 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,100,menu[0]);
            DEBUG_SetTextColor(selected == 1 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,120,menu[1]);
            DEBUG_Print(420,120,gameid);
            DEBUG_SetTextColor(selected == 2 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,140,menu[2]);
            DEBUG_ShowValueU8(436,140,game->numcodes);
            DEBUG_SetTextColor(selected == 3 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,160,menu[3]);
            DEBUG_ShowValueU32(436,160,game->numaddress[0]);
            DEBUG_SetTextColor(selected == 4 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,180,menu[4]);
            DEBUG_ShowValueU32(436,180,game->numaddress[1]);
            DEBUG_SetTextColor(selected == 5 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,200,menu[5]);
            DEBUG_ShowValueU32(436,200,game->numaddress[2]);
            DEBUG_SetTextColor(selected == 6 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,220,menu[6]);
            DEBUG_ShowValueU32(436,220,game->numaddress[3]);
            DEBUG_SetTextColor(selected == 7 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,240,menu[7]);
            DEBUG_ShowValueU32(436,240,game->padaddress);
            DEBUG_SetTextColor(selected == 8 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,260,menu[8]);
            DEBUG_ShowValueU32(436,260,game->frames);
            DEBUG_SetTextColor(selected == 9 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,280,menu[9]);
            DEBUG_ShowValueU32(436,280,game->flags);
            DEBUG_SetTextColor(selected == 10 ? COLOR_YELLOW : COLOR_WHITE);
            DEBUG_Print(50,300,menu[10]);
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
            if (selected < 10) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 10;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                GCARSEnterGameName(game);
                break;
              case 1:
                GCARSGetGameID(game);
                break;
              case 2:
                GCARSCodes(game);
                break;
              case 3:
                CSEnterCode(game,0);
                break;
              case 4:
                CSEnterCode(game,1);
                break;
              case 5:
                CSEnterCode(game,2);
                break;
              case 6:
                CSEnterCode(game,3);
                break;
              case 7:
                NETGUIEnterU32(&(game->padaddress),"Edit Pad Address");
                break;
              case 8:
                NETGUIEnterU32(&(game->frames),"Edit Update Speed");
                break;
              case 9:
                NETGUIEnterU32(&(game->flags),"Edit Game Flags");
                break;
              case 10:
                return;
            }
            draw = 1;
        }

        if (pad.Digital.B) return;

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void CSNetworkSetup()
{
    char* title = "ControlSim: Network Setup";

    char menu1[] = "BBA Speed";
    char menu2[] = "Local Controller:  ";
    char menu3[40] = "IP Address 1: ";
    char menu4[40] = "IP Address 2: ";
    char menu5[40] = "IP Address 3: ";
    char menu6[40] = "IP Address 4: ";
    char menu7[40] = "Subnet Mask: ";
    char menu8[40] = "Default Gateway: ";
    char menu9[40] = "Levine Address: ";

    u32 x,addr;
    u8 draw = 1;
    u8 selected = 0;
    u8 swapsource = 0xFF;
    PAD pad;
    for (;;)
    {
        if (draw)
        {
            menu2[18] = 0x31 + CS_DATA->localPad;
            NETGUIConvertIPAddress(&menu3[14],CS_DATA->IPAddress[0]);
            NETGUIConvertIPAddress(&menu4[14],CS_DATA->IPAddress[1]);
            NETGUIConvertIPAddress(&menu5[14],CS_DATA->IPAddress[2]);
            NETGUIConvertIPAddress(&menu6[14],CS_DATA->IPAddress[3]);
            NETGUIConvertIPAddress(&menu7[13],CS_ENTITY->subnetMask);
            NETGUIConvertIPAddress(&menu8[17],CS_ENTITY->defGateway);
            NETGUIConvertIPAddress(&menu9[16],CS_ENTITY->LevineIP);

            if ((CS_DATA->localPad == 0) && (CS_DATA->IPAddress[0] == 0)) strcpy(&menu3[14],"DHCP Enabled");
            if ((CS_DATA->localPad == 1) && (CS_DATA->IPAddress[1] == 0)) strcpy(&menu4[14],"DHCP Enabled");
            if ((CS_DATA->localPad == 2) && (CS_DATA->IPAddress[2] == 0)) strcpy(&menu5[14],"DHCP Enabled");
            if ((CS_DATA->localPad == 3) && (CS_DATA->IPAddress[3] == 0)) strcpy(&menu6[14],"DHCP Enabled");

            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,title);
            DEBUG_SetTextColor(COLOR_WHITE);
            //DEBUG_ShowValueU32(338,150,CS_DATA->net_gameID);
            DEBUG_Print(50,150,menu1);
            DEBUG_Print(50,170,menu2);
            DEBUG_Print(50,190,menu3);
            DEBUG_Print(50,210,menu4);
            DEBUG_Print(50,230,menu5);
            DEBUG_Print(50,250,menu6);
            DEBUG_Print(50,270,menu7);
            DEBUG_Print(50,290,menu8);
            DEBUG_Print(50,310,menu9);
            if (swapsource <= 3)
            {
                DEBUG_Print(18,210 + (20 * swapsource),"*");
                DEBUG_Print(530,210 + (20 * swapsource),"*");
            }
            DEBUG_SetTextColor(COLOR_AQUA);
            if (CS_DATA->IPAddress[0] != 0) DEBUG_Print(50,190,menu3);
            if (CS_DATA->IPAddress[1] != 0) DEBUG_Print(50,210,menu4);
            if (CS_DATA->IPAddress[2] != 0) DEBUG_Print(50,230,menu5);
            if (CS_DATA->IPAddress[3] != 0) DEBUG_Print(50,250,menu6);
            if (CS_ENTITY->subnetMask != 0) DEBUG_Print(50,270,menu7);
            if (CS_ENTITY->defGateway != 0) DEBUG_Print(50,290,menu8);
            if (LevineEnabled()) DEBUG_Print(50,310,menu9);
            DEBUG_SetTextColor(COLOR_ORANGE);
            if (CS_DATA->localPad == 0) DEBUG_Print(50,190,menu3);
            if (CS_DATA->localPad == 1) DEBUG_Print(50,210,menu4);
            if (CS_DATA->localPad == 2) DEBUG_Print(50,230,menu5);
            if (CS_DATA->localPad == 3) DEBUG_Print(50,250,menu6);
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
                if (CS_DATA->IPAddress[0] != 0) DEBUG_SetTextColor(COLOR_LIME);
                if (CS_DATA->localPad == 0) DEBUG_SetTextColor(COLOR_RED);
                DEBUG_Print(50,190,menu3);
                break;
              case 3:
                if (CS_DATA->IPAddress[1] != 0) DEBUG_SetTextColor(COLOR_LIME);
                if (CS_DATA->localPad == 1) DEBUG_SetTextColor(COLOR_RED);
                DEBUG_Print(50,210,menu4);
                break;
              case 4:
                if (CS_DATA->IPAddress[2] != 0) DEBUG_SetTextColor(COLOR_LIME);
                if (CS_DATA->localPad == 2) DEBUG_SetTextColor(COLOR_RED);
                DEBUG_Print(50,230,menu5);
                break;
              case 5:
                if (CS_DATA->IPAddress[3] != 0) DEBUG_SetTextColor(COLOR_LIME);
                if (CS_DATA->localPad == 3) DEBUG_SetTextColor(COLOR_RED);
                DEBUG_Print(50,250,menu6);
                break;
              case 6:
                if (CS_ENTITY->subnetMask != 0) DEBUG_SetTextColor(COLOR_LIME);
                DEBUG_Print(50,270,menu7);
                break;
              case 7:
                if (CS_ENTITY->defGateway != 0) DEBUG_SetTextColor(COLOR_LIME);
                DEBUG_Print(50,290,menu8);
                break;
              case 8:
                if (LevineEnabled()) DEBUG_SetTextColor(COLOR_LIME);
                DEBUG_Print(50,310,menu9);
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

        LevineReceiveData(0,CS_PACKET_BUFFER);

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < 8) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 8;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                NETGUISelectBBASpeed();
                break;
              case 1:
                CSGetLocalController();
                break;
              case 2:
                NETGUIEnterIPAddress(&CS_DATA->IPAddress[0],"ControlSim: IP Address 1");
                break;
              case 3:
                NETGUIEnterIPAddress(&CS_DATA->IPAddress[1],"ControlSim: IP Address 2");
                break;
              case 4:
                NETGUIEnterIPAddress(&CS_DATA->IPAddress[2],"ControlSim: IP Address 3");
                break;
              case 5:
                NETGUIEnterIPAddress(&CS_DATA->IPAddress[3],"ControlSim: IP Address 4");
                break;
              case 6:
                NETGUIEnterIPAddress(&CS_ENTITY->subnetMask,"ControlSim: Subnet Mask");
                break;
              case 7:
                NETGUIEnterIPAddress(&CS_ENTITY->defGateway,"ControlSim: Default Gateway");
                break;
              case 8:
                NETGUIEnterIPAddress(&(CS_ENTITY->LevineIP),"GCARS-CS: Levine Address");
                GCARSStatus("Starting Levine");
                CSNetInit(CS_DATA->IPAddress[CS_DATA->localPad]);
                LevineInit();
                break;
            }
            draw = 1;
        }

        if (pad.Digital.Z)
        {
            if ((selected <= 7) && (selected >= 3))
            {
                if (swapsource > 3) swapsource = selected - 3;
                else {
                    if (CS_DATA->localPad == swapsource) CS_DATA->localPad = selected - 3;
                    else if (CS_DATA->localPad == selected - 3) CS_DATA->localPad = swapsource;
                    addr = CS_DATA->IPAddress[swapsource];
                    CS_DATA->IPAddress[swapsource] = CS_DATA->IPAddress[selected - 3];
                    CS_DATA->IPAddress[selected - 3] = addr;
                    swapsource = 0xFF;
                }
                draw = 1;
            }
        }

        if (pad.Digital.B) return;

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void CSSelectGame()
{
    char* title = "ControlSim: Edit Game List";
    u32 swaps = 0xFFFFFFFF;
    GCARS_GAME* temp;

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
            if (datablock->numgames != 0)
            {
                DEBUG_SetTextColor(COLOR_WHITE);
                for (x = 0; x < 12; x++)
                {
                    if (x >= datablock->numgames) break;
                    DEBUG_Print(50,(100 + (x * 20)),datablock->games[x + scroll]->name);
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
                    if (datablock->numgames > 12)
                    DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(34,120 + ((scroll * 180) / (datablock->numgames - 12)),"¿");
                }
            } else {
                GCARSError("No game entries");
                return;
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

        /*if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < datablock->numgames - 1) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = datablock->numgames - 1;
            draw = 1;
        } */

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
        if (pad.Digital.A || pad.Digital.Y)
        {
            CSGameOptions(datablock->games[selected]);
            GCARSSaveCodes();
            draw = 1;
        }

        /*if (pad.Digital.A || pad.Digital.Start)
        {
            GCARSSaveCodes();
            CSStartGameP2P(datablock->games[selected]);
            //CSTest(datablock->games[selected]);
            draw = 1;
        } */

        if (pad.Digital.B) return;

        if (scroll < 0) scroll = 0;
        if (scroll > (datablock->numgames - 12)) scroll = (datablock->numgames - 12);

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

void CSSetup()
{
    char* title = "ControlSim: Setup";
    char* menu1 = "Start Online Game";
    char* menu2 = "Edit Game List";
    char* menu3 = "Network Setup";
    char* menu4 = "Server Mode";
    char* menu5 = "Save Settings";

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
            DEBUG_Print(50,150,menu1);
            DEBUG_Print(50,170,menu2);
            DEBUG_Print(50,190,menu3);
            DEBUG_Print(50,210,menu4);
            DEBUG_Print(50,230,menu5);
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
              default:
                selected = 0;
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
            if (selected < 4) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 4;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                GCARSSaveCodes();
                CSStartGameP2P(NULL);
                break;
              case 1:
                CSSelectGame();
                break;
              case 2:
                CSNetworkSetup();
                break;
              case 3:
                CSStartGameServer();
                break;
              case 4:
                GCARSSaveCodes();
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

