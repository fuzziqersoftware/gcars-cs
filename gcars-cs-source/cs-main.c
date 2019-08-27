#include "gamecube.h"
#include "cs.h"

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

void CSDefaultNetwork()
{
    CS_DATA->gc_gameID = 0x47504F45;
    CS_DATA->unused = 0x55555555;
    CS_DATA->IPAddress[0] = 0x0A000132;
    CS_DATA->IPAddress[1] = 0x0A000106;
    CS_DATA->IPAddress[2] = 0x00000000;
    CS_DATA->IPAddress[3] = 0x00000000;
    CS_ENTITY->defGateway = 0x0A000106;
    CS_ENTITY->subnetMask = 0xFFFFFF00;
    CS_DATA->localPad = 0;
}

int CSInit(GCARS_GAME* game)
{
    u32 x;
    int errors = 0;

    if (exi_deviceid(0,2) != 0x04020200)
    {
        GCARSError("You do not have a BBA.");
        return NET_ERR_NOHARDWARE;
    }

    GCARSStatus("Starting Control Simulator...");

    if (game != NULL)
    {
        CS_DATA->padAddress = (CSPADStatus*)(game->padaddress);
        CS_DATA->gc_gameID  = game->gameid;
        memset(&CS_DATA->latestPads[0],0,sizeof(CSPADStatus) * 4);
        memcpy(CS_ADDRESS_LIST,game->address[CS_DATA->localPad],game->numaddress[CS_DATA->localPad] * 8);
        *(u32*)((u32)CS_ADDRESS_LIST + game->numaddress[CS_DATA->localPad] * 8) = 0;
        *(u32*)((u32)CS_ADDRESS_LIST + 4 + game->numaddress[CS_DATA->localPad] * 8) = 0;
    }

    if (CS_ENTITY->defGateway != 0)
    {
        /*if (CS_DATA->servermode)
        {
            CSNetInit(CS_DATA->IPAddress[0]);
            GCARSStatus("ARPing Default Gateway...");
            CSNetARPAddress(CS_ENTITY->MAC,&(CS_ENTITY->defGatewayMAC),CS_ENTITY->IP,CS_ENTITY->defGateway);
            GCARSStatus("ARPing server...");
            if ((CS_DATA->IPAddress[1] & CS_ENTITY->subnetMask) == (CS_DATA->IPAddress[0] & CS_ENTITY->subnetMask))
            {
                errors = CSNetInitSocket(&CS_DATA->connection[1],CS_DATA->IPAddress[1],CONTROLSIM_PORT,CONTROLSIM_PORT);
            } else {
                CS_DATA->connection[1].localMAC[0]  = CS_ENTITY->MAC[0];
                CS_DATA->connection[1].localMAC[1]  = CS_ENTITY->MAC[1];
                CS_DATA->connection[1].localMAC[2]  = CS_ENTITY->MAC[2];
                CS_DATA->connection[1].localMAC[3]  = CS_ENTITY->MAC[3];
                CS_DATA->connection[1].localMAC[4]  = CS_ENTITY->MAC[4];
                CS_DATA->connection[1].localMAC[5]  = CS_ENTITY->MAC[5];
                CS_DATA->connection[1].localIP      = CS_ENTITY->IP;
                CS_DATA->connection[1].remoteIP     = CS_DATA->IPAddress[1];
                CS_DATA->connection[1].localPort    = CONTROLSIM_PORT;
                CS_DATA->connection[1].remotePort   = CONTROLSIM_PORT;
                CS_DATA->connection[1].packetID     = 0;
                CS_DATA->connection[1].remoteMAC[0] = CS_ENTITY->defGatewayMAC[0];
                CS_DATA->connection[1].remoteMAC[1] = CS_ENTITY->defGatewayMAC[1];
                CS_DATA->connection[1].remoteMAC[2] = CS_ENTITY->defGatewayMAC[2];
                CS_DATA->connection[1].remoteMAC[3] = CS_ENTITY->defGatewayMAC[3];
                CS_DATA->connection[1].remoteMAC[4] = CS_ENTITY->defGatewayMAC[4];
                CS_DATA->connection[1].remoteMAC[5] = CS_ENTITY->defGatewayMAC[5];
            }
            if (errors != 0)
            {
                GCARSError("ARP failed.");
                return errors;
            }
        } else { */
            CSNetInit(CS_DATA->IPAddress[CS_DATA->localPad]);
            GCARSStatus("ARPing Default Gateway...");
            CSNetARPAddress(CS_ENTITY->MAC,&(CS_ENTITY->defGatewayMAC),CS_ENTITY->IP,CS_ENTITY->defGateway);
            GCARSStatus("ARPing Players...");
            for (x = 0; x < 4; x++)
            {
                //GCARSStatus("ARPing players...");
                errors = 0;
                if (CS_DATA->IPAddress[x] == CS_DATA->IPAddress[CS_DATA->localPad]) continue;
                if ((CS_DATA->IPAddress[x] & CS_ENTITY->subnetMask) == (CS_ENTITY->defGateway & CS_ENTITY->subnetMask))
                {
                    errors = CSNetInitSocket(&CS_DATA->connection[x],CS_DATA->IPAddress[x],CONTROLSIM_PORT,CONTROLSIM_PORT);
                } else {
                    memcpy(CS_DATA->connection[x].localMAC,CS_ENTITY->MAC,6);
                    CS_DATA->connection[x].localIP      = CS_ENTITY->IP;
                    CS_DATA->connection[x].remoteIP     = CS_DATA->IPAddress[x];
                    CS_DATA->connection[x].localPort    = CONTROLSIM_PORT;
                    CS_DATA->connection[x].remotePort   = CONTROLSIM_PORT;
                    CS_DATA->connection[x].packetID     = 0;
                    memcpy(CS_DATA->connection[x].remoteMAC,CS_ENTITY->defGatewayMAC,6);
                }
                if (errors != 0)
                {
                    GCARSError("ARP failed.");
                    return errors;
                }
            //}
        }
    } else {
        /*if (CS_DATA->servermode)
        {
            GCARSError("%FAILURE: NO DEFAULT GATEWAY");
        } else { */
            CSNetInit(CS_DATA->IPAddress[CS_DATA->localPad]);
            if ((CS_DATA->localPad != 0) && (CS_DATA->IPAddress[0] != 0))
            {
                GCARSStatus("ARPing Player 1...");
                errors = CSNetInitSocket(&CS_DATA->connection[0],CS_DATA->IPAddress[0],CONTROLSIM_PORT,CONTROLSIM_PORT);
                if (errors != 0)
                {
                    GCARSError("Could not ARP Player 1");
                    return errors;
                }
            }
            if ((CS_DATA->localPad != 1) && (CS_DATA->IPAddress[1] != 0))
            {
                GCARSStatus("ARPing Player 2...");
                errors = CSNetInitSocket(&CS_DATA->connection[1],CS_DATA->IPAddress[1],CONTROLSIM_PORT,CONTROLSIM_PORT);
                if (errors != 0)
                {
                    GCARSError("Could not ARP Player 2");
                    return errors;
                }
            }
            if ((CS_DATA->localPad != 2) && (CS_DATA->IPAddress[2] != 0))
            {
                GCARSStatus("ARPing Player 3...");
                errors = CSNetInitSocket(&CS_DATA->connection[2],CS_DATA->IPAddress[2],CONTROLSIM_PORT,CONTROLSIM_PORT);
                if (errors != 0)
                {
                    GCARSError("Could not ARP Player 3");
                    return errors;
                }
            }
            if ((CS_DATA->localPad != 3) && (CS_DATA->IPAddress[3] != 0))
            {
                GCARSStatus("ARPing Player 4...");
                errors = CSNetInitSocket(&CS_DATA->connection[3],CS_DATA->IPAddress[3],CONTROLSIM_PORT,CONTROLSIM_PORT);
                if (errors != 0)
                {
                    GCARSError("Could not ARP Player 4");
                    return errors;
                }
            }
        //}
    }

    CS_ENTITY->speed = eth_getspeed() & 7;
    return NET_ERR_NONE;
}

