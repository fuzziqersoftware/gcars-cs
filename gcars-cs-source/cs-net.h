#define PACKET_FORM_ETHERNET 0x00
#define PACKET_FORM_ARP 0x06

#define NET_ERR_NONE 0
#define NET_ERR_NOREPLY (-1)
#define NET_ERR_BADPACKET (-2)
#define NET_ERR_UNSUPPORTED (-3)
#define NET_ERR_NOHARDWARE (-4)

#define PACKET_VERIFY(x)  if ((x)->verifier != CONTROLSIM_VERIFIER) goto badpacket; \
                          if ((x)->version != CONTROLSIM_VERSION) goto badpacket; \
                          if ((CS_DATA->gc_gameID != 0) && ((x)->gcGameID != CS_DATA->gc_gameID)) goto badpacket; \
                          if ((CS_DATA->gc_gameID != 0) && ((x)->netGameID != CS_DATA->net_gameID)) goto badpacket;

#include "cache.h"

typedef struct {
    u48 localMAC;
    u48 remoteMAC;
    u32 localIP;
    u32 remoteIP;
    u16 localPort;
    u16 remotePort;
    u16 packetID; } CSSocket;

typedef struct {
    u48 MAC;
    u32 IP;
    u32 subnetMask;
    u32 defGateway;
    u48 defGatewayMAC;

    u32 bufferSize;
    void* bufferptr;

    u32 LevineIP;
    u8 speed;
    u8 uselesscrap[3]; } CSEntity;

#pragma pack(1)

typedef struct {
    unsigned char destMAC[6];
    unsigned char srcMAC[6];
    unsigned char type;
    unsigned char protocol; } ETHpacket;

typedef struct {
    unsigned char version_ihl;
    unsigned char TOS;
    unsigned short len;
    unsigned short ID;
    unsigned short flags_offset;
    unsigned char TTL;
    unsigned char protocol;
    unsigned short checksum;
    unsigned long srcIP;
    unsigned long dstIP; } IPpacket;

typedef struct {
    unsigned short srcPort;
    unsigned short dstPort;
    unsigned short len;
    unsigned short checksum; } UDPpacket;

#pragma pack(0)

u16 CSNetChecksum(u16* abuf,u32 hsize);

bool CSNetDecap(CSSocket* s,void* data,void* dbuffer,u32* length);
bool CSNetEncap(CSSocket* s,void* dbuffer,void* data,u32* length);

void CSNetARPReply(u48 localMAC,u48 remoteMAC,u32 localIP,u32 remoteIP);
int CSNetARPAddress(u48 localMAC,u48* remoteMAC,u32 localIP,u32 remoteIP);

void CSNetInit(u32 ip);
int CSNetInitSocket(CSSocket* s,u32 rip,u16 lport,u16 rport);
void CSNetInitReceive();
u32 CSNetReceiveFrom(void* data,u32 maxSize,u32 port);
u32 CSNetSend(CSSocket* s,void* data,u32 size);

#ifndef __IS_HOOK
void CSNetDoDHCP();
#endif

/* FUNCTIONS: 
bool CSNetDecap(CSSocket* s,void* data,void* dbuffer,u32* length)

    Decapsulates the packet data pointed to by dbuffer.

  CSSocket* s
    Socket with which to compare source/destination data.

  void* data
    Pointer to buffer for raw data

  void* dbuffer
    Pointer to packet data

  u32* length
    Pointer to length variable. When called, this should be set to
    the size of the packet data in dbuffer. CSNetDecap will set this
    to the size of the actual raw data that is extracted.

  Return value:
    TRUE if the packet is valid and was decapped successfully.
    FALSE if the packet was invalid or an ARP packet.
    If the packet was an ARP packet, CSNetDecap will respond.

 */

