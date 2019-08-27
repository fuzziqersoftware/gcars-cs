//#define __IS_HOOK

#include "hook.h"
#include "string.h"

// Entrypoint //////////////////////////////////////////////////////////////////

void controlsim()
{
    u32 x;

    //////////////////////////////////////////////////////////////////////// 
    // Count this update and read local pad data /////////////////////////// 
    //////////////////////////////////////////////////////////////////////// 

    CS_DATA->updateCount++;
    for (x = 0; x < 4; x++) if (CS_DATA->local[x]) memcpy(&(CS_DATA->latestPads[x]),&(CS_DATA->padAddress[x]),sizeof(CSPADStatus));

    //////////////////////////////////////////////////////////////////////// 
    // Receive Packets ///////////////////////////////////////////////////// 
    //////////////////////////////////////////////////////////////////////// 

    u8 update[4] = {0,0,0,0};
    u8 lnum;
    u48 srcMAC;
    u32 srcIP,size,num,addr;
    u16 srcPort,dstPort;
    CSPacket* csp = (CSPacket*)CS_PACKET_BUFFER;
    CSSocket css;
    u32* buffer = (u32*)CS_PACKET_BUFFER;
    u32* addresses = CS_ADDRESS_LIST;

    memset(&css,0,sizeof(CSSocket));
    css.localIP = CS_ENTITY->IP;
    memcpy(&(css.localMAC[0]),&(CS_ENTITY->MAC[0]),6);
    css.localPort = CONTROLSIM_PORT;
    css.remotePort = CONTROLSIM_PORT;

    /*if (!FS_EXI_IsSelected(0))
    { */
        // 1 line for debug 
        CSNetInitReceive();
        while (CS_ENTITY->bufferSize > 0)
        {
            CSNetDecap(&css,csp,CS_PACKET_BUFFER,&(CS_ENTITY->bufferSize));

            if (csp->verifier != CONTROLSIM_VERIFIER) goto invalidpacket;
            if (csp->version != CONTROLSIM_VERSION) goto invalidpacket;
            if (csp->gcGameID != CS_DATA->gc_gameID) goto invalidpacket;
            if (CSChecksum(csp) != csp->checksum) goto invalidpacket;
            switch (csp->padspresent & 0xFF00)
            {
              case COMMAND_NEW_SCRIPT:
                lnum = csp->padspresent & 0x0003;
                memcpy(CS_ADDRESS_LIST,(void*)((u32)&csp + sizeof(CSPacket)),0x600);
                CS_DATA->localPad = lnum;
                for (CS_DATA->numAddress = 0; (*(u32*)((u32)CS_ADDRESS_LIST + 0) | *(u32*)((u32)CS_ADDRESS_LIST + 0)) && (CS_DATA->numAddress < 0xBF); CS_DATA->numAddress++);
                break;
              case COMMAND_NEW_CODELIST:
                memcpy(GCARS_CODELIST,(void*)((u32)&csp + sizeof(CSPacket)),0x800);
                break;
              case COMMAND_DBG_PAUSE:
                GCARS_PAUSE = csp->numCommands;
                if (csp->numCommands != 0)
                {
                    for (x = 0; x < 4; x++)
                    {
                        if (!CS_DATA->local[x]) continue;
                        csp->size = sizeof(CSPacket);
                        csp->padspresent = COMMAND_DISCONNECT | x;
                        CSBroadcast(csp);
                        CS_DATA->frames = 0xFFFFFFFE;
                    }
                }
                break;
              case COMMAND_DBG_READMEM:
                addr = csp->numCommands;
                size = *(u32*)((u32)&csp + sizeof(CSPacket));
                if ((size > 0x400) || (addr > 0x81800000) || (addr < 0x80000000))
                {
                    csp->size = sizeof(CSPacket);
                    csp->padspresent = COMMAND_INVALID_DATA;
                } else {
                    csp->size = (sizeof(CSPacket)) + size + 4;
                    csp->padspresent = COMMAND_DBG_READMEMREPLY;
                    memcpy((void*)((u32)&csp + sizeof(CSPacket) + 4),(void*)addr,size);
                }
                CSBroadcast(csp);
                break;
              case COMMAND_DBG_WRITEMEM:
                addr = csp->numCommands;
                size = *(u32*)((u32)&csp + sizeof(CSPacket));
                if ((size > 0x400) || (addr > 0x81800000) || (addr < 0x80000000))
                {
                    csp->size = sizeof(CSPacket);
                    csp->padspresent = COMMAND_INVALID_DATA;
                } else {
                    memcpy((void*)addr,(void*)((u32)&csp + sizeof(CSPacket) + 4),size);
                    flush_code((void*)addr,size);
                    csp->padspresent = COMMAND_DBG_WRITEMEMREPLY;
                    csp->numCommands = 0;
                }
                CSBroadcast(csp);
                break;
              case COMMAND_DBG_BRANCH:
                ((void (*)())(csp->numCommands))();
                break;
              case COMMAND_DISCONNECT:
                CS_DATA->IPAddress[csp->padspresent & 0x0003] = 0;
                CS_DATA->latestPads[csp->padspresent & 0x0003].error = 0xFF;
                break;
              case COMMAND_WRITE:
                memcpy(&(CS_DATA->latestPads[csp->padspresent & 0x0003]),(void*)((u32)&csp + sizeof(CSPacket)),sizeof(CSPADStatus));
                if (csp->numCommands > 0) ecodes((u32)((u32)&csp + sizeof(CSPacket) + sizeof(CSPADStatus)));
                break;
            }

invalidpacket:
            CSNetInitReceive();
        }
    //}

    //////////////////////////////////////////////////////////////////////// 
    // Send Packets //////////////////////////////////////////////////////// 
    //////////////////////////////////////////////////////////////////////// 

    csp = (CSPacket*)((u32)CS_PACKET_BUFFER + 0x800);
    if ((CS_DATA->updateCount > CS_DATA->frames) && (CS_DATA->localPad < 4) && (CS_DATA->frames != 0xFFFFFFFE))
    {
        for (x = 0; x < 4; x++)
        {
            if (CS_DATA->local[x] && (CS_DATA->latestPads[x].error != 0xFF))
            {
                addr = CS_DATA->numAddress * 8;
                size = addr + sizeof(CSPacket) + sizeof(CSPADStatus);
                csp = (CSPacket*)CS_ADDRESS_BUFFER;

                memset(csp,0,size);
                csp->verifier = CONTROLSIM_VERIFIER;
                csp->version = CONTROLSIM_VERSION;
                csp->size = size;
                csp->gcGameID = CS_DATA->gc_gameID;
                csp->padspresent = COMMAND_WRITE | x;
                csp->numCommands = CS_DATA->numAddress;

                memcpy((void*)((u32)&csp + sizeof(CSPacket)),&(CS_DATA->latestPads[x]),sizeof(CSPADStatus));
                //memcpy((void*)((u32)&csp + sizeof(CSPacket) + sizeof(CSPadStatus),addresses,addr);
                rcodes((u32)addresses,(u32)((u32)&csp + sizeof(CSPacket) + sizeof(CSPADStatus)));

                CSBroadcast(csp);
            }
        }
    }

    if (CS_DATA->updateCount > CS_DATA->frames) CS_DATA->updateCount = 0;

    //////////////////////////////////////////////////////////////////////// 
    // Update Controller Data ////////////////////////////////////////////// 
    //////////////////////////////////////////////////////////////////////// 

    memcpy((void*)(CS_DATA->padAddress),&(CS_DATA->latestPads[0]),sizeof(CSPADStatus) * 4);
}

void CSBroadcast(CSPacket* csp)
{
    u32 x;
    csp->checksum = CSChecksum(csp);
    for (x = 0; x < 4; x++)
    {
        if ((CS_DATA->local[x]) || (CS_DATA->IPAddress[x] == 0)) continue;
        CSNetSend(&(CS_DATA->connection[x]),csp,csp->size);
    }
}

u32 CSChecksum(CSPacket* csp)
{
    u32 x,value = 0;
    value = csp->gcGameID;
    value ^= csp->size;
    value ^= csp->padspresent;
    value ^= csp->numCommands;
    for (x = ((u32)&csp + sizeof(CSPacket)); x < csp->size; x += 4) value ^= *(u32*)(x);
    return value;
}

