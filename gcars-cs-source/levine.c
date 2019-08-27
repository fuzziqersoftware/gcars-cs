#include "gamecube.h"
#include "cs-net.h"
#include "levine.h"
#include "gcars.h"

#include "loaddol.h"

u8 levine_extensions = 0;
u8 levine_inited = 0;
CSSocket levine_conn;

void LevineInit()
{
    CSNetInitSocket(&levine_conn,CS_ENTITY->LevineIP,22065,22065);
    levine_inited = 1;
    levine_extensions = 1;
}

void LevineSendData(u32 mode,void* data,u32 size)
{
    if (!levine_inited) return;
    LevinePacket* cmsp = (LevinePacket*)CS_PACKET_BUFFER;
    cmsp->verifier = LEVINE_VERIFIER;
    cmsp->mode = mode;
    cmsp->size = size;
    memcpy(&cmsp->data[0],data,cmsp->size);
    CSNetSend(&levine_conn,cmsp,cmsp->size + 12);
}

u32 LevineReceiveData(u32 mode,void* data)
{
    if (!levine_inited) return 0;
    LevinePacket* cmsp = (LevinePacket*)CS_PACKET_BUFFER;
    u32 size;
    CSNetInitReceive();
    while (CS_ENTITY->bufferSize > 0)
    {
        CSNetDecap(&levine_conn,CS_PACKET_BUFFER,CS_PACKET_BUFFER,&(CS_ENTITY->bufferSize));
        if (cmsp->verifier != LEVINE_VERIFIER) goto badpacket;
        if ((cmsp->mode == 0) && (cmsp->size == 0))
        {
            cmsp->verifier = LEVINE_VERIFIER;
            cmsp->mode = 0;
            cmsp->size = 0;
            CSNetSend(&levine_conn,cmsp,12);
        }
        if ((cmsp->mode == 0) && (cmsp->size == 0xFFFFFFFC) && levine_extensions) dol_load();
        if ((cmsp->mode == 0) && (cmsp->size == 0xFFFFFFFF)) ((void (*)())0x81300000)();
        if (cmsp->mode != mode) goto badpacket;
        size = cmsp->size;
        memcpy(data,cmsp->data,size);
        return size;
badpacket:
        memset(CS_PACKET_BUFFER,0,0x1000);
        CSNetInitReceive();
    }
    return 0;
}

void LevineExit()
{
    levine_inited = 0;
    levine_extensions = 0;
}

u8 LevineEnabled() { return levine_inited; }

