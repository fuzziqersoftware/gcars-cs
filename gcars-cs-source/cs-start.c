#include "gamecube.h"
#include "cs.h"

#include "load-game.h"

#include "hook.bin.h"

#define STATUS_DISCONNECTED  0
#define STATUS_CONNECTED     1
#define STATUS_READY         2

void DisableEXIProbe()
{
    u32 _exiprobe1[] = {
         0    ,0x7FE02214,0x4082000C,0x38600001,
    0x48000130,0x3BA00001,     0    ,0x1CBC0014,
    0x801F000C,0x3C80CC00,0x38C46800,0x7CC62A14,
    0x54000739,0x80E60000,0x7C7E1B78,0x408200CC};

    u32 _exiprobe2[] = {
    0x80030000,0x7C002050,0x2C000003,0x40800058,
    0x3BA00000,0x48000050,0x38800000,0x3C608000,
    0x909F0020,0x5780103A,0x386330C0,0x7C83012E,
    0x3BA00000,0x48000030,0x54E004E7,0x4182000C};

    u32 _exiprobe1size = 15;
    u32 _exiprobe2size = 15;

    u32 addr,x;
    for (addr = 0x80000000; addr < 0x81300000; addr += 4)
    {
        for (x = 0; x < _exiprobe1size; x++)
        {
            if (_exiprobe1 == 0) continue;
            if (*(u32*)(addr + (x * 4)) != _exiprobe1[x]) break;
        }
        if (x == _exiprobe1size) *(u16*)(addr + (_exiprobe1size * 4)) = 0x4800;

        for (x = 0; x < _exiprobe2size; x++)
        {
            if (_exiprobe2 == 0) continue;
            if (*(u32*)(addr + (x * 4)) != _exiprobe2[x]) break;
        }
        if (x == _exiprobe2size) *(u16*)(addr + (_exiprobe2size * 4)) = 0x4800;
    }
}

void CSStartGameP2P(GCARS_GAME* game)
{
    PAD pad;

    *(u32*)0x80000028 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x80000034 = 0;
    *(u32*)0x80000038 = 0;
    *(u32*)0x800000EC = (u32)MEMORY_START;
    *(u32*)0x800000F0 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x800000F4 = 0;

    void* entrypoint = LoadGame();
    if (entrypoint == NULL) return;

    u32 x,y;
    if (game == NULL)
    {
        for (x = 0; ((x < datablock->numgames) && (game == NULL)); x++)
        {
            if (*(u32*)0x80000000 == datablock->games[x]->gameid)
            {
                game = datablock->games[x];
                break;
            }
        }
        if (game == NULL)
        {
            dvd_stop_nowait();
            GCARSError("No entry found for this game");
            return;
        }
    } else {
        if (*(u32*)0x80000000 != game->gameid)
        {
            dvd_stop_nowait();
            GCARSError("This is not the correct game!");
            return;
        }
    }

    FS_PAD_ReadAllState(&pad);
    if (!pad.Digital.X)
    {
        GCARSStatus("Disabling memory cards in-game");
        DisableEXIProbe();
    }

    if (CS_DATA->IPAddress[CS_DATA->localPad] == 0)
    {
        GCARSStatus("Contacting the DHCP server");
        CSNetDoDHCP();
        CS_DATA->IPAddress[CS_DATA->localPad] = CS_ENTITY->IP;
    }

    CS_DATA->servermode = 0;
    if (CSInit(game) != NET_ERR_NONE) return;
    CS_DATA->updateCount = 0;
    CS_DATA->frames = game->frames;
    CS_DATA->flags = game->flags;
    for (x = 0; x < 4; x++)
    {
        if (CS_DATA->IPAddress[x] == CS_DATA->IPAddress[CS_DATA->localPad]) CS_DATA->local[x] = 1;
        else CS_DATA->local[x] = 0;
    }

    u32* codes = malloc(8 * 256);//(u32*)0x81600000;
    u32 codesactive = 0;
    u32 codesoffset = 0;

    for (x = 0; x < game->numcodes; x++)
    {
        if (game->codes[x]->enabled)
        {
            memcpy((void*)((u32)codes + (8 * codesactive)),game->codes[x]->lines,game->codes[x]->numlines * 8);
            codesactive += game->codes[x]->numlines;
        }
    }

    /*Addresses to check:
        80000028 memory size
        80000034 arena high
        80000038 fst loc in ram
        800000EC memory top
        800000F0 simulated memory size
        800000F4 pad spec? */

    /**(u32*)0x80000028 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x80000034 = 0;
    *(u32*)0x80000038 = 0;
    *(u32*)0x800000EC = (u32)MEMORY_START;
    *(u32*)0x800000F0 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x800000F4 = 0; */

    memcpy((void*)ENTRYPOINT     ,&hook,HOOK_SIZE);
    flush_code((void*)ENTRYPOINT ,HOOK_SIZE);
    for (x = 0, y = 0; x < 4; x++)
    {
        if (CS_DATA->local[x])
        {
            memcpy((void*)((u32)CS_ADDRESS_LIST + y),game->address[x],game->numaddress[x] * 8);
            y += game->numaddress[x] * 8;
        }
    }
    memcpy((void*)GCARS_CODELIST ,codes,codesactive * 8);
    free(codes);

    CS_DATA->numAddress = (y / 8) + 1;
    CS_DATA->latestPads[0].error = 0xFF;
    CS_DATA->latestPads[1].error = 0xFF;
    CS_DATA->latestPads[2].error = 0xFF;
    CS_DATA->latestPads[3].error = 0xFF;

    GCARS_PAUSE = 0;
    GCARS_CONDITIONAL = 0;
    GCARS_ENABLE_CS = 0; // Disable the Control Simulator, for now 

    // MUST be done AFTER the game is loaded! 
    codes = GCARS_CODELIST;
    for (x = 0; x < game->codes[0]->numlines; x++)
    {
        if (((codes[x * 2] & 0x01FFFFFF) | 0x80000000) > *(u32*)0x80000038) codes[x * 2] -= 0x8000;
    }

    u8 draw = 1,loop = 1;
    u8 ready[4] = {0,0,0,0};
    u32 size,src;
    u48 srcMAC;
    u32 srcIP,num;
    u16 srcPort,dstPort;
    CSPacket* cspr = (CSPacket*)CS_PACKET_BUFFER;
    CSPacket* csps = (CSPacket*)((u32)CS_PACKET_BUFFER + 0x800);

    CSSocket css;
    css.localIP = CS_DATA->IPAddress[CS_DATA->localPad];
    memcpy(&(css.localMAC[0]),&(CS_ENTITY->MAC[0]),6);
    css.localPort = CONTROLSIM_PORT;
    css.remotePort = CONTROLSIM_PORT;

    FS_VIDEO_WaitVSync();
    FS_PAD_ReadAllState(&pad);

    loop = 1;
    draw = 1;
    u8 status[4] = {STATUS_DISCONNECTED,STATUS_DISCONNECTED,STATUS_DISCONNECTED,STATUS_DISCONNECTED};
    for (x = 0; x < 4; x++) if (CS_DATA->local[x]) status[x] = STATUS_CONNECTED;
    u32 frames,loopframes = ((*(u32*)0x800000CC == 1) ? 50 : 60) * 2;
    frames = loopframes; // ensure that status data is sent immediately 
    while (loop)
    {
        // draw screen 
        if (draw)
        {
            GCARSStatus("Waiting for players");
            DEBUG_Print(50,120,"Press A to change status");
            if (CS_DATA->localPad == 0) DEBUG_Print(50,140,"Press START to start game!");
            DEBUG_Print(50,270,"Press B to disconnect");
            DEBUG_Print(50,170,"Player 1:");
            DEBUG_Print(50,190,"Player 2:");
            DEBUG_Print(50,210,"Player 3:");
            DEBUG_Print(50,230,"Player 4:");
            for (x = 0; x < 4; x++)
            {
                switch (status[x])
                {
                  case STATUS_DISCONNECTED:
                    DEBUG_SetTextColor(COLOR_GRAY);
                    DEBUG_Print(206,170 + (20 * x),"Disconnected");
                    break;
                  case STATUS_CONNECTED:
                    DEBUG_SetTextColor(COLOR_WHITE);
                    DEBUG_Print(206,170 + (20 * x),"Connected");
                    break;
                  case STATUS_READY:
                    DEBUG_SetTextColor(COLOR_LIME);
                    DEBUG_Print(206,170 + (20 * x),"Ready!");
                    break;
                  default:
                    DEBUG_SetTextColor(COLOR_RED);
                    DEBUG_Print(206,170 + (20 * x),"Invalid State");
                    break;
                }
            }
            debugDrawInfo();
            draw = 0;
        }

        // receive data 
        CSNetInitReceive();
        while (CS_ENTITY->bufferSize > 0)//while ((size = CSNetReceiveFrom(csp,0,CONTROLSIM_PORT)) != 0)
        {
            CSNetDecap(&css,CS_PACKET_BUFFER,CS_PACKET_BUFFER,&(CS_ENTITY->bufferSize));
            if (cspr->verifier != CONTROLSIM_VERIFIER) goto badpacket;
            if (cspr->version != CONTROLSIM_VERSION) goto badpacket;
            if (cspr->gcGameID != CS_DATA->gc_gameID) goto badpacket;
            if (CSChecksum(cspr) != cspr->checksum) goto badpacket;
            switch (cspr->padspresent & 0xFF00)
            {
              case COMMAND_NEW_CODELIST: // overwrite code list 
                memcpy(GCARS_CODELIST,(void*)((u32)&cspr + sizeof(CSPacket)),0x800);
                break;
              case COMMAND_START: // start command 
                loop = 0;
                break;
              case COMMAND_STATUS: // status command 
                status[(cspr->padspresent & 0x0030) >> 4] = (cspr->padspresent & 0x0003);
                draw = 1;
                break;
            }
badpacket:
            //memset(CS_PACKET_BUFFER,0,0x1000);
            CSNetInitReceive();
        }

        // check controllers 
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.A)
        {
            if (status[CS_DATA->localPad] == STATUS_CONNECTED) for (x = 0; x < 4; x++) if (CS_DATA->local[x]) status[x] = STATUS_READY;
            else for (x = 0; x < 4; x++) if (CS_DATA->local[x]) status[x] = STATUS_CONNECTED;
            draw = 1;
            FS_PAD_WaitEmptyAll();
            FS_VIDEO_WaitVSync();
        }
        if (pad.Digital.Start && (CS_DATA->localPad == 0))
        {
            codes = GCARS_CODELIST;
            FS_VIDEO_WaitVSync();
            FS_PAD_WaitEmptyAll();
            if (CS_DATA->localPad == 0)
            {
                csps->verifier = CONTROLSIM_VERIFIER;
                csps->version = CONTROLSIM_VERSION;
                csps->size = sizeof(CSPacket) + 4;
                csps->gcGameID = CS_DATA->gc_gameID;
                csps->padspresent = COMMAND_NEW_CODELIST;
                for (x = 0; ((codes[x * 2] | codes[(x * 2) + 1]) != 0); x++)
                {
                    *(u32*)((u32)&csps + sizeof(CSPacket) + (x * 2) + 0) = codes[x * 2];
                    *(u32*)((u32)&csps + sizeof(CSPacket) + (x * 2) + 1) = codes[(x * 2) + 1];
                    csps->size += 8;
                }
                *(u32*)((u32)&csps + sizeof(CSPacket) + (x * 2) + 0) = 0;
                *(u32*)((u32)&csps + sizeof(CSPacket) + (x * 2) + 1) = 0;
                csps->checksum = CSChecksum(csps);
                for (x = 0; x < 4; x++) if ((!CS_DATA->local[x]) && (CS_DATA->IPAddress[x] != 0)) CSNetSend(&(CS_DATA->connection[x]),csps,csps->size);
                GCARSStatus("Please wait");
                for (x = 0; x < loopframes; x++) FS_VIDEO_WaitVSync();
            }

            csps->verifier = CONTROLSIM_VERIFIER;
            csps->version = CONTROLSIM_VERSION;
            csps->size = sizeof(CSPacket);
            csps->gcGameID = CS_DATA->gc_gameID;
            csps->padspresent = COMMAND_START;
            csps->checksum = CSChecksum(csps);
            for (x = 0; x < 4; x++) if ((!CS_DATA->local[x]) && (CS_DATA->IPAddress[x] != 0)) CSNetSend(&(CS_DATA->connection[x]),csps,csps->size);
            loop = 0;
        }
        if (pad.Digital.B)
        {
            csps->verifier = CONTROLSIM_VERIFIER;
            csps->version = CONTROLSIM_VERSION;
            csps->size = sizeof(CSPacket);
            csps->gcGameID = CS_DATA->gc_gameID;
            csps->padspresent = COMMAND_STATUS | (CS_DATA->localPad << 4);
            csps->checksum = CSChecksum(csps);
            for (x = 0; x < 4; x++)
            {
                if ((CS_DATA->local[x]) || (CS_DATA->IPAddress[x] == 0)) continue;
                CSNetSend(&(CS_DATA->connection[x]),csps,csps->size);
            }
            ((void (*)())0x81300000)(); // restart!! 
        }

        // if 3 seconds have elapsed, send status data 
        if (frames >= loopframes)
        {
            for (y = 0; y < 4; y++)
            {
                if (!CS_DATA->local[y]) continue;
                memset(csps,0,sizeof(CSPacket));
                csps->verifier = CONTROLSIM_VERIFIER;
                csps->version = CONTROLSIM_VERSION;
                csps->size = sizeof(CSPacket);
                csps->gcGameID = CS_DATA->gc_gameID;
                csps->padspresent = COMMAND_STATUS | (y << 4) | status[y];
                csps->checksum = CSChecksum(csps);
                for (x = 0; x < 4; x++)
                {
                    if ((CS_DATA->local[x]) || (CS_DATA->IPAddress[x] == 0)) continue;
                    CSNetSend(&(CS_DATA->connection[x]),csps,csps->size);
                }
                frames = 0;
            }
        }

        // wait for vsync and increment counter 
        FS_VIDEO_WaitVSync();
        frames++;
    }

    //hexviewer("About to launch game!",0x817FF600);

    //*(vu16*)(0xCC002002) = 2; // disable video 

    ENTRYPOINT(); // Run GCARS once! 
    GCARS_ENABLE_CS = 1; // Enable the Control Simulator!! 
    //hexviewer("GCARS has run!",0x817FF600);
    //ENTRYPOINT(); // Run GCARS-CS once! 
    //hexviewer("GCARS-CS has run!",0x817FF600);
    ((void (*)())entrypoint)(); // Start the game! 
}

void CSStartGameServer()
{
    *(u32*)0x80000028 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x80000034 = 0;
    *(u32*)0x80000038 = 0;
    *(u32*)0x800000EC = (u32)MEMORY_START;
    *(u32*)0x800000F0 = (u32)MEMORY_START & 0x01FFFFFF;
    *(u32*)0x800000F4 = 0;

    memcpy((void*)ENTRYPOINT,&hook,HOOK_SIZE);
    flush_code((void*)ENTRYPOINT,HOOK_SIZE);

    if (CS_DATA->IPAddress[CS_DATA->localPad] == 0)
    {
        GCARSStatus("Contacting the DHCP server");
        CSNetDoDHCP();
        CS_DATA->IPAddress[CS_DATA->localPad] = CS_ENTITY->IP;
    }

    CS_DATA->latestPads[0].error = 0xFF;
    CS_DATA->latestPads[1].error = 0xFF;
    CS_DATA->latestPads[2].error = 0xFF;
    CS_DATA->latestPads[3].error = 0xFF;
    CS_DATA->gc_gameID = 0;
    CS_DATA->localPad = 4;
    CS_DATA->numAddress = 0;
    CS_DATA->updateCount = 0;
    CS_DATA->servermode = 1;
    if (CSInit(NULL) != NET_ERR_NONE) return;

    //memcpy((void*)CS_ADDRESS_LIST,game->address[CS_DATA->localPad],game->numaddress[CS_DATA->localPad] * 8);
    //memcpy((void*)GCARS_CODELIST ,codes,codesactive * 8);

    GCARS_PAUSE = 0;
    GCARS_CONDITIONAL = 0;
    GCARS_ENABLE_CS = 0; // Disable the Control Simulator, for now 

    u8 draw = 1,loop = 1;
    u16 srcPort,dstPort;
    u32 size,src,srcIP,num,x;
    u32 frames = 0,loopframes = ((*(u32*)0x800000CC == 1) ? 50 : 60) * 2;
    u32 *buffer = CS_PACKET_BUFFER;
    u48 srcMAC;
    PAD pad;
    CSPacket* csp = (CSPacket*)CS_PACKET_BUFFER;
    CSSocket css;
    void* entrypoint = NULL;

    char gameid[5] = {0,0,0,0,0};
    css.localIP = CS_DATA->IPAddress[0];
    memcpy(&(css.localMAC[0]),&(CS_ENTITY->MAC[0]),6);
    css.localPort = CONTROLSIM_PORT;
    css.remotePort = CONTROLSIM_PORT;

    loop = 1;
    draw = 1;
    while (loop)
    {
        // draw screen 
        if (draw)
        {
            memcpy(gameid,&(CS_DATA->gc_gameID),4);
            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,"Waiting for Data");
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(50,150,"GameCube Game ID :");
            DEBUG_Print(50,170,"Network Game ID  : Disabled");
            DEBUG_Print(50,190,"Local Controller :");
            DEBUG_Print(50,210,"Pad Addresses    :");
            DEBUG_Print(50,230,"GCARS Code List  :");
            DEBUG_Print(50,250,"ControlSim Script:");
            DEBUG_Print(50,270,"Game             :");
            DEBUG_SetTextColor(COLOR_ORANGE);
            if (!CS_DATA->gc_gameID) DEBUG_Print(354,150,"----");
            if (CS_DATA->localPad > 3) DEBUG_Print(354,190,"-");
            if (!CS_DATA->padAddress) DEBUG_Print(354,210,"Not Present");
            if (!(*GCARS_CODELIST)) DEBUG_Print(354,230,"Not Present");
            if (!(*CS_ADDRESS_LIST)) DEBUG_Print(354,250,"Not Present");
            if (entrypoint == NULL) DEBUG_Print(354,270,"Not Loaded");
            DEBUG_SetTextColor(COLOR_LIME);
            if (CS_DATA->gc_gameID) DEBUG_Print(354,150,gameid);
            if (CS_DATA->localPad < 4) DEBUG_PrintChar(370,190,0x31 + CS_DATA->localPad);
            if (CS_DATA->padAddress) DEBUG_Print(354,210,"Present");
            if (*GCARS_CODELIST) DEBUG_Print(354,230,"Present");
            if (*CS_ADDRESS_LIST) DEBUG_Print(354,250,"Present");
            if (entrypoint) DEBUG_ShowValueU32(370,270,(u32)entrypoint);
            draw = 0;
        }

        CSNetInitReceive();
        while (CS_ENTITY->bufferSize > 0)
        {
            CSNetDecap(&css,CS_PACKET_BUFFER,CS_ENTITY->bufferptr,&(CS_ENTITY->bufferSize));
            if (csp->verifier != CONTROLSIM_VERIFIER) goto badpacket;
            if (csp->version != CONTROLSIM_VERSION) goto badpacket;
            if (CS_DATA->gc_gameID && (csp->gcGameID != CS_DATA->gc_gameID)) goto badpacket;
            if (CSChecksum(csp) != csp->checksum) goto badpacket;
            switch (csp->padspresent & 0xFF00)
            {
              case COMMAND_START:
                loop = 0;
                break;
              case COMMAND_CONFIG:
                CS_DATA->gc_gameID = csp->gcGameID;
                CS_DATA->padAddress = (CSPADStatus*)(*(u32*)((u32)&csp + sizeof(CSPacket)));
                draw = 1;
                break;
              case COMMAND_LOADGAME:
                entrypoint = LoadGame();
                if ((entrypoint == NULL) || (*(u32*)0x80000000 != CS_DATA->gc_gameID))
                {
                    GCARSError("You have been disconnected.");
                    return;
                }
                DisableEXIProbe();
                csp->size = sizeof(CSPacket);
                csp->padspresent = COMMAND_READY;
                CSNetSend(&(CS_DATA->connection[1]),csp,csp->size);
                draw = 1;
                break;
              case COMMAND_NEW_SCRIPT:
                memset(CS_ADDRESS_LIST,0,0x600);
                for (x = 0; ((buffer[(x * 2) + 5] | buffer[(x * 2) + 6]) != 0); x++)
                {
                    *(u32*)((u32)CS_ADDRESS_LIST + (x * 2) + 0) = buffer[(x * 2) + 5];
                    *(u32*)((u32)CS_ADDRESS_LIST + (x * 2) + 1) = buffer[(x * 2) + 6];
                }
                CS_DATA->localPad = csp->padspresent & 0x0003;
                CS_DATA->numAddress = x;
                draw = 1;
                break;
              case COMMAND_NEW_CODELIST:
                memset(GCARS_CODELIST,0,0x800);
                for (x = 0; ((buffer[(x * 2) + 5] | buffer[(x * 2) + 6]) != 0); x++)
                {
                    *(u32*)((u32)GCARS_CODELIST + (x * 2) + 0) = buffer[(x * 2) + 5];
                    *(u32*)((u32)GCARS_CODELIST + (x * 2) + 1) = buffer[(x * 2) + 6];
                }
                draw = 1;
                break;
            }
badpacket:
            memset(CS_ENTITY->bufferptr,0,0x1000);
            CSNetInitReceive();
        }

        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.Start && (CS_DATA->localPad == 0) && (CS_DATA->gc_gameID != 0))
        {
            FS_VIDEO_WaitVSync();
            FS_PAD_WaitEmptyAll();
            csp->size = sizeof(CSPacket);
            csp->padspresent = COMMAND_START;
            CSNetSend(&(CS_DATA->connection[1]),csp,csp->size);
            loop = 0;
        }
        if (frames >= loopframes)
        {
            csp->size = sizeof(CSPacket);
            csp->padspresent = COMMAND_PING;
            CSNetSend(&(CS_DATA->connection[1]),csp,sizeof(CSPacket));
            frames = 0;
        }
        FS_VIDEO_WaitVSync();
        frames++;
    }

    *(vu16*)(0xCC002002) = 2; // disable video 

    ENTRYPOINT(); // Run GCARS once! 
    GCARS_ENABLE_CS = 1; // Enable the Control Simulator!! 
    ((void (*)())entrypoint)(); // Start the game! 
}

