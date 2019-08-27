#include "gamecube.h"
#include "cs.h"

#include "loaddol.h"
#include "loaddol-hook.bin.h"

extern u8 levine_extensions;

u32 _dolsize(DOL_HEADER* buffer)
{
    u32 x,latest = 0;
    for (x = 0; x < 18; x++)
    {
        if (buffer->textpos[x] > buffer->textpos[latest]) latest = x;
    }
    return (buffer->textpos[latest] + buffer->textsize[latest]);
}

u8 _dolvalid(DOL_HEADER* buffer)
{
    u32 x,latest = 0;
    for (x = 0; x < 18; x++)
    {
        if ((buffer->textpos[x] == 0) && (buffer->textmem[x] == 0) && (buffer->textsize[x] == 0)) continue;
        if (buffer->textpos[x] < 0x00000100) return 0;
        if (buffer->textpos[x] > 0x017FFF00) return 0;
        if (buffer->textmem[x] < 0x80000000) return 0;
        if (buffer->textmem[x] >= 0x81800000) return 0;
        if (buffer->textsize[x] < 4) return 0;
        if (buffer->textsize[x] >= 0x017FFF00) return 0;
    }
    if ((buffer->bssaddr != 0) && (buffer->bsssize != 0))
    {
        if (buffer->bssaddr < 0x80000000) return 0;
        if (buffer->bssaddr >= 0x81800000) return 0;
        if (buffer->bsssize < 4) return 0;
        if (buffer->bsssize >= 0x01800000) return 0;
    }
    if (buffer->entrypoint < 0x80000000) return 0;
    if (buffer->entrypoint >= 0x81800000) return 0;
    return 1;
}

void dol_load()
{
    levine_extensions = false;
    memcpy((void*)0x80001800,loaddolhook,sizeof(loaddolhook));
    flush_code((void*)0x80001800,sizeof(loaddolhook));

    DOL_HEADER *temp,*dolbuffer = malloc(sizeof(DOL_HEADER));
    u32 x,sendbuffer[2] = {0x10000000,0};
    u32 dolsize,size,offset = 0,cs,cs2;

    GCARSStatus("Loading DOL file");
    size = LevineReceiveData(0xFFFFFFFD,dolbuffer);
    cs2 = *(u32*)((u32)dolbuffer + (size - 4));
    cs = checksum(dolbuffer,size - 4);
    if (cs != cs2)
    {
        GCARSError("DOL header checksum error");
        levine_extensions = true;
        return;
    }
    dolsize = _dolsize(dolbuffer);
    offset = sizeof(DOL_HEADER);
    sendbuffer[1] = offset;
    LevineSendData(0xFFFFFFFD,&sendbuffer,8);
    temp = malloc(dolsize);
    memcpy(temp,dolbuffer,sizeof(DOL_HEADER));
    free(dolbuffer);
    dolbuffer = temp;
    DEBUG_ShowValueU32(210,150,dolsize);
    DEBUG_PrintChar(194,150,':');
    while (offset < dolsize)
    {
        size = LevineReceiveData(0xFFFFFFFD,(void*)((u32)dolbuffer + offset));
        if (size != 0)
        {
            cs2 = *(u32*)((u32)dolbuffer + offset + (size - 4));
            cs = checksum((void*)((u32)dolbuffer + offset),(size - 4));
            if (cs == cs2) offset += (size - 4);
            sendbuffer[1] = offset;
            if (size != 0) LevineSendData(0xFFFFFFFD,sendbuffer,8);
            GRAPH_Rectangle(33,150,64,20,COLOR_BLACK);
            DEBUG_ShowValueU32(66,150,offset);
        }
    }
    ((void (*)(void*))0x80001800)(dolbuffer);
    free(dolbuffer);
}

void dol_launch(DOL_HEADER* dh)
{
    memcpy((void*)0x80001800,loaddolhook,sizeof(loaddolhook));
    flush_code((void*)0x80001800,sizeof(loaddolhook));
    ((void (*)(void*))0x80001800)(dh);
}

