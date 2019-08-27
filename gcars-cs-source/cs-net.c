//#define FUZZIQER_TYPES
//#include "types.h"
//#include "cache.h"
//#include "bba.h"
//#include "cs.h"
//#include "string.h"

#include "gamecube.h"
#include "cs.h"

u16 CSNetChecksum(u16* abuf,u32 hsize)
{
    int i;
    unsigned long cval;
    cval = 0;
    for (i = 0; i < hsize; i++)
    {
        cval += abuf[i];
        cval = (cval & 0xFFFF) + (cval >> 16);
        cval = (cval & 0xFFFF) + (cval >> 16);
    }
    return (unsigned short)(cval ^ 0xFFFF);
}

bool CSNetDecap(CSSocket* s,void* data,void* dbuffer,u32* length)
{
    ETHpacket* eth;
    IPpacket* ip;
    UDPpacket* udp;
    u32 testip;
    u48 testmac;

    eth = (ETHpacket*)dbuffer;
    if (memcmp(s->localMAC,eth->destMAC,6))
    {
        *length = 0;
        return false;
    }
    if (eth->type != 0x08)
    {
        *length = 0;
        return false;
    }
    u8 type = eth->protocol;
    dbuffer = (void*)((u32)dbuffer + 14);

    switch (type)
    {
      case 0x00: // IP 
        ip = (IPpacket*)dbuffer;
        if ((ip->protocol != 0x11))
        {
            *length = 0;
            return false;
        }
        if (memcmp(&ip->dstIP,&s->localIP,4))
        {
            *length = 0;
            return false;
        }
        dbuffer = (void*)((u32)dbuffer + 20);

        udp = (UDPpacket*)dbuffer;
        if (false)//((udp->srcPort != s->remotePort) || (udp->dstPort != s->localPort))
        {
            *length = 0;
            return false;
        }
        dbuffer = (void*)((u32)dbuffer + 8);
        *length = udp->len - 8;

        if ((*length == 0) || (*length > 0x0C00))
        {
            *length = 0;
            return false;
        }

        memcpy(data,dbuffer,*length);
        return true;
      case 0x06: // ARP 
        memcpy(&testip,(void*)((u32)dbuffer + 0x18),4);
        if (testip == CS_ENTITY->IP)
        {
            //memcpy(s->remoteMAC,eth->srcMAC,6);
            //memcpy(&s->remoteIP,&ip->srcIP,4);
            memcpy(&testip,(void*)((u32)dbuffer + 0x0E),4);
            memcpy(&testmac,(void*)((u32)dbuffer + 0x08),6);
            CSNetARPReply(CS_ENTITY->MAC,testmac,CS_ENTITY->IP,testip);
        }
        *length = 42;
        return false;
      default:
        *length = 0;
        return false;
    }
}

bool CSNetEncap(CSSocket* s,void* dbuffer,void* data,u32* length)
{
    ETHpacket* eth = (ETHpacket*)dbuffer;
    IPpacket* ip = (IPpacket*)((u32)dbuffer + 14);
    UDPpacket* udp = (UDPpacket*)((u32)dbuffer + 34);
    dbuffer = (void*)((u32)dbuffer + 42);

    memmove(dbuffer,data,*length);

    //if (*length < 60) *length = 60;

    if (CS_ENTITY->defGateway == 0) memcpy(eth->destMAC,s->remoteMAC,6);
    else {
        if ((s->remoteIP & CS_ENTITY->subnetMask) == (CS_ENTITY->defGateway & CS_ENTITY->subnetMask)) memcpy(eth->destMAC,s->remoteMAC,6);
        else memcpy(eth->destMAC,&(CS_ENTITY->defGatewayMAC),6);
    }

    //memcpy(eth->destMAC,s->remoteMAC,6);
    memcpy(eth->srcMAC,s->localMAC,6);
    eth->type = 0x08;
    eth->protocol = 0x00;

    ip->version_ihl = 0x45;
    ip->TOS = 0;
    ip->len = *length + 28;
    ip->ID = s->packetID;
    s->packetID = ((s->packetID + 1) & 0xFFFF);
    ip->flags_offset = 0;
    ip->TTL = 0x80;
    ip->protocol = 0x11;
    ip->checksum = 0;
    memcpy(&ip->srcIP,&s->localIP,4);
    memcpy(&ip->dstIP,&s->remoteIP,4);
    ip->checksum = CSNetChecksum((u16*)ip,10);

    udp->srcPort = s->localPort;
    udp->dstPort = s->remotePort;
    udp->len = *length + 8;
    udp->checksum = 0;

    *length = *length + 42;
    return true;
}

////////////////////////////////////////////////////////////////////////////////

void CSNetARPReply(u48 localMAC,u48 remoteMAC,u32 localIP,u32 remoteIP)
{
    u8* buffer = (u8*)CS_PACKET_BUFFER;

    //GCARSStatus("ARPReply: Building packet");
    buffer[0]  = remoteMAC[0];
    buffer[1]  = remoteMAC[1];
    buffer[2]  = remoteMAC[2];
    buffer[3]  = remoteMAC[3];
    buffer[4]  = remoteMAC[4];
    buffer[5]  = remoteMAC[5];
    buffer[6]  = localMAC[0];
    buffer[7]  = localMAC[1];
    buffer[8]  = localMAC[2];
    buffer[9]  = localMAC[3];
    buffer[10] = localMAC[4];
    buffer[11] = localMAC[5];
    buffer[12] = 0x08;
    buffer[13] = 0x06;

    buffer[14] = 0x00;
    buffer[15] = 0x01;
    buffer[16] = 0x08;
    buffer[17] = 0x00;
    buffer[18] = 0x06;
    buffer[19] = 0x04;
    buffer[20] = 0x00;
    buffer[21] = 0x02;
    buffer[22] = localMAC[0];
    buffer[23] = localMAC[1];
    buffer[24] = localMAC[2];
    buffer[25] = localMAC[3];
    buffer[26] = localMAC[4];
    buffer[27] = localMAC[5];
    buffer[28] = (localIP >> 24) & 0xFF;
    buffer[29] = (localIP >> 16) & 0xFF;
    buffer[30] = (localIP >> 8) & 0xFF;
    buffer[31] = localIP & 0xFF;
    buffer[32] = remoteMAC[0];
    buffer[33] = remoteMAC[1];
    buffer[34] = remoteMAC[2];
    buffer[35] = remoteMAC[3];
    buffer[36] = remoteMAC[4];
    buffer[37] = remoteMAC[5];
    buffer[38] = (remoteIP >> 24) & 0xFF;
    buffer[39] = (remoteIP >> 16) & 0xFF;
    buffer[40] = (remoteIP >> 8) & 0xFF;
    buffer[41] = remoteIP & 0xFF;

    dcache_flush(buffer,42);
    eth_send(buffer,42);
}

#ifndef __IS_HOOK
int CSNetARPAddress(u48 localMAC,u48* remoteMAC,u32 localIP,u32 remoteIP)
{
    unsigned long i, len;
    u8* buffer = (u8*)CS_PACKET_BUFFER;
    CSNetInitReceive();

    buffer[0]  = 0xFF;
    buffer[1]  = 0xFF;
    buffer[2]  = 0xFF;
    buffer[3]  = 0xFF;
    buffer[4]  = 0xFF;
    buffer[5]  = 0xFF;
    buffer[6]  = localMAC[0];
    buffer[7]  = localMAC[1];
    buffer[8]  = localMAC[2];
    buffer[9]  = localMAC[3];
    buffer[10] = localMAC[4];
    buffer[11] = localMAC[5];
    buffer[12] = 0x08;
    buffer[13] = 0x06;
    buffer[14] = 0x00;
    buffer[15] = 0x01;
    buffer[16] = 0x08;
    buffer[17] = 0x00;
    buffer[18] = 0x06;
    buffer[19] = 0x04;
    buffer[20] = 0x00;
    buffer[21] = 0x01;
    buffer[22] = localMAC[0];
    buffer[23] = localMAC[1];
    buffer[24] = localMAC[2];
    buffer[25] = localMAC[3];
    buffer[26] = localMAC[4];
    buffer[27] = localMAC[5];
    buffer[28] = (localIP >> 24) & 0xFF;
    buffer[29] = (localIP >> 16) & 0xFF;
    buffer[30] = (localIP >> 8) & 0xFF;
    buffer[31] = localIP & 0xFF;
    buffer[32] = 0x00;
    buffer[33] = 0x00;
    buffer[34] = 0x00;
    buffer[35] = 0x00;
    buffer[36] = 0x00;
    buffer[37] = 0x00;
    buffer[38] = (remoteIP >> 24) & 0xFF;
    buffer[39] = (remoteIP >> 16) & 0xFF;
    buffer[40] = (remoteIP >> 8) & 0xFF;
    buffer[41] = remoteIP & 0xFF;

    dcache_flush(buffer,42);
    eth_send(buffer,42);

    for (;;)
    {
        i = 0x10000000;
        while (!(len = eth_receive(buffer)) || (!i)) i--;
        if (!i) return NET_ERR_NOREPLY;
        if ((buffer[13] == 0x06) && (*(u16*)(&buffer[20]) == 0x02)) break;
    }
    //hexviewer("ARP loop closed",(u32)buffer);

    (*remoteMAC)[0] = buffer[22];
    (*remoteMAC)[1] = buffer[23];
    (*remoteMAC)[2] = buffer[24];
    (*remoteMAC)[3] = buffer[25];
    (*remoteMAC)[4] = buffer[26];
    (*remoteMAC)[5] = buffer[27];
    return NET_ERR_NONE;
}

void CSNetInit(u32 ip)
{
    CS_ENTITY->IP = ip;
    eth_setspeed(CS_ENTITY->speed | 0x80);
}

int CSNetInitSocket(CSSocket* s,u32 rip,u16 lport,u16 rport)
{
    s->localMAC[0]  = CS_ENTITY->MAC[0];
    s->localMAC[1]  = CS_ENTITY->MAC[1];
    s->localMAC[2]  = CS_ENTITY->MAC[2];
    s->localMAC[3]  = CS_ENTITY->MAC[3];
    s->localMAC[4]  = CS_ENTITY->MAC[4];
    s->localMAC[5]  = CS_ENTITY->MAC[5];
    s->localIP      = CS_ENTITY->IP;
    s->remoteIP     = rip;
    s->localPort    = lport;
    s->remotePort   = rport;
    s->packetID     = 0;
    return CSNetARPAddress(s->localMAC,&s->remoteMAC,s->localIP,s->remoteIP);
}
#endif

void CSNetInitReceive()
{
    //CS_ENTITY->buffer = (void*)CS_PACKET_BUFFER;
    //memset(CS_ENTITY->buffer,0,0x1000);
    dcache_inv(CS_PACKET_BUFFER,0x1000);
    CS_ENTITY->bufferSize = eth_receive((unsigned char*)CS_PACKET_BUFFER);
}

/*u32 CSNetReceiveFrom(void* data,u32 maxSize,u32 port)
{
    //GCARSErrorNum("RecvFrom: initing",CS_ENTITY->bufferSize);
    CSSocket css;
    css.localIP = CS_ENTITY->IP;
    memcpy(css.localMAC,CS_ENTITY->MAC,6);
    css.localPort = port;
    css.remotePort = port;
    u32 pax_processed = 0;
    u32 size = 1;
    bool process = false;

    while ((process == false) && (size != 0) && (pax_processed < 5))
    {
        process = CSNetDecap(&css,CS_PACKET_BUFFER,CS_ENTITY->buffer,&size);
        //GCARSErrorNum("RecvFrom: Decap called, size",size);
        CS_ENTITY->bufferSize -= (size + 42);
        CS_ENTITY->buffer = (void*)((u32)(CS_ENTITY->buffer) + (size + 42));
        pax_processed++;
    }
    if (!process) return 0;
    return size;
} */

u32 CSNetSend(CSSocket* s,void* data,u32 size)
{
    if (s->remoteIP == 0) return 0;
    //CS_ENTITY->buffer = CS_PACKET_BUFFER;
    CS_ENTITY->bufferSize = 0;
    CSNetEncap(s,CS_PACKET_BUFFER,data,&size);
    eth_send((u8*)CS_PACKET_BUFFER,size);
}

#ifndef __IS_HOOK
void CSNetDoDHCP()
{
    u8 bufferout[0x0400],bufferin[0x0400];
    u32 transID = mftbl();
    u32 offerIP,serverIP,defGW,subMask;

    memset(CS_ENTITY->defGatewayMAC,0,6);
    CS_ENTITY->IP = 0;
    CS_ENTITY->subnetMask = 0;
    CS_ENTITY->defGateway = 0;
    CS_ENTITY->bufferSize = 0;

    CSSocket css;
    memcpy(&css.localMAC,CS_ENTITY->MAC,6);
    memset(css.remoteMAC,0xFF,6);
    css.localIP    = 0x00000000;
    css.remoteIP   = 0xFFFFFFFF;
    css.localPort  = 68;
    css.remotePort = 67;
    css.packetID   = 0;

    memset(&bufferout[0],0,0x0400);
    memset(&bufferin[0],0,0x0400);

    // DHCP base 
    bufferout[0] = 0x01;
    bufferout[1] = 0x01;
    bufferout[2] = 0x06;
    bufferout[3] = 0x00;
    *(u32*)((u32)(&bufferout[0]) + 4) = transID;
    memcpy(&bufferout[0x1C],CS_ENTITY->MAC,6);
    *(u32*)((u32)(&bufferout[0]) + 0xEC) = 0x63825363;

    memcpy(&bufferout[0xF0],"\x35\x01\x01\x37\x02\x01\x03\x39\x02\x05\xDC\x3D\x07\x01\x00\x00\x00\x00\x00\x00\x33\x04\x00\x76\xA7\x00\xFF",27);
    memcpy(&bufferout[0xFE],CS_ENTITY->MAC,6);
    // discover[3], parameters[4], max length [4], client addr[9],
    // lease time [6], end [1]

    // Go!! 
    CSNetSend(&css,&bufferout[0],0x0120);

    u32 offset = 0xF0;
    //CSNetInitReceive();
    for (;;) // CSNetReceiveFrom(CS_PACKET_buffer,0,CONTROLSIM_PORT)
    {
        CSNetInitReceive();
        if (CS_ENTITY->bufferSize == 0) continue;
        memcpy(&css.remoteMAC,&bufferin[6],6);
        CSNetDecap(&css,&bufferin[0],CS_PACKET_BUFFER,&(CS_ENTITY->bufferSize));

        if (*(u32*)((u32)(&bufferin[0]) + 4) != transID) continue;
        offerIP = *(u32*)((u32)(&bufferin[0]) + 0x10);
        while (offset != 0)
        {
            switch (bufferin[offset])
            {
              case 0x2E:
              case 0x35:
                offset += 3;
                break;
              case 0x06:
              case 0x3A:
              case 0x3B:
              case 0x33:
                offset += 6;
                break;
              case 0x36:
                memcpy(&serverIP,&bufferin[offset + 2],4);
                offset += 6;
                break;
              case 0x01:
                memcpy(&subMask,&bufferin[offset + 2],4);
                offset += 6;
                break;
              case 0x03:
                memcpy(&defGW,&bufferin[offset + 2],4);
                offset += 6;
                break;
              case 0x0F:
                while (bufferin[offset] != 0) offset++;
                offset++;
                break;
              default:
                offset = 0;
                break;
            }
        }
        if (offset == 0) break;
    }

    memcpy(&css.localMAC,CS_ENTITY->MAC,6);
    css.localIP    = 0x00000000;
    css.remoteIP   = 0xFFFFFFFF;
    css.localPort  = 68;
    css.remotePort = 67;
    css.packetID   = 0;

    memset(&bufferout[0],0,0x0400);
    memset(&bufferin[0],0,0x0400);

    // DHCP base 
    bufferout[0] = 0x01;
    bufferout[1] = 0x01;
    bufferout[2] = 0x06;
    bufferout[3] = 0x00;
    *(u32*)((u32)(&bufferout[0]) + 4) = transID;
    memcpy(&bufferout[0x1C],CS_ENTITY->MAC,6);
    *(u32*)((u32)(&bufferout[0]) + 0xEC) = 0x63825363;

    memcpy(&bufferout[0xF0],"\x35\x01\x03\x37\x02\x01\x03\x39\x02\x05\xDC\x3D\x07\x01\x00\x00\x00\x00\x00\x00\x32\x04\x00\x00\x00\x00\x36\x04\x00\x00\x00\x00\xFF",33);
    memcpy(&bufferout[0xFE],CS_ENTITY->MAC,6);
    memcpy(&bufferout[0x0106],&offerIP,4);
    memcpy(&bufferout[0x010C],&serverIP,4);
    // discover[3], parameters[4], max length [4], client addr[9],
    // requested IP [6], server IP [6], end [1]

    // Go!! 
    CSNetSend(&css,&bufferout[0],0x0128);

    /*offset = 0xF0;
    //CSNetInitReceive();
    for (;;) // CSNetReceiveFrom(CS_PACKET_buffer,0,CONTROLSIM_PORT)
    {
        CSNetInitReceive();
        if (CS_ENTITY->bufferSize == 0) continue;
        CSNetDecap(&css,&bufferin[0],CS_ENTITY->buffer,&(CS_ENTITY->bufferSize));

        if (bufferin[0] != 0x02) continue;
        if (bufferin[2] != 0x01) continue;
        if (*(u32*)((u32)(&bufferin[0]) + 4) != transID) continue;
        if (memcmp(&bufferin[0x1C],CS_ENTITY->MAC,6)) continue;
        if (*(u32*)((u32)(&bufferin[0]) + 0xEC) != 0x63825363) continue;
        offerIP = *(u32*)((u32)(&bufferin[0]) + 0x10);
        while (offset != 0)
        {
            switch (bufferin[offset])
            {
              case 0x2E:
                offset += 3;
                break;
              case 0x35:
                if (bufferin[offset + 2] == 0x05) offset = 0;
                break;
              case 0x06:
              case 0x3A:
              case 0x3B:
              case 0x33:
                offset += 6;
                break;
              case 0x36:
                memcpy(&serverIP,&bufferin[offset + 2],4);
                offset += 6;
                break;
              case 0x01:
                memcpy(&(CS_ENTITY->subnetMask),&bufferin[offset + 2],4);
                offset += 6;
                break;
              case 0x03:
                memcpy(&(CS_ENTITY->defGateway),&bufferin[offset + 2],4);
                offset += 6;
                break;
              case 0x0F:
                while (bufferin[offset] != 0) offset++;
                offset++;
                GCARSStatus("0F removed");
                break;
              default:
                offset = 0;
                break;
            }
        }
        if (offset == 0) break;
    } */

    CS_ENTITY->IP = offerIP;
    CS_ENTITY->defGateway = defGW;
    CS_ENTITY->subnetMask = subMask;
}
#endif

