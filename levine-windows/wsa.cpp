#include <windows.h>
#include <winsock2.h>
#include "wsa.h"

WSADATA wsaData;

int UDPOpenSocket(int port)
{
    SOCKET sock;
    int errors;
    struct sockaddr_in my_addr;

    sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sock == SOCKET_ERROR) return SOCKET_ERROR;

    my_addr.sin_family = AF_INET;
    my_addr.sin_port = htons(port);
    my_addr.sin_addr.s_addr = INADDR_ANY;
    errors = bind(sock,(sockaddr*)&my_addr,sizeof(sockaddr));
    if (errors != NO_ERROR) return SOCKET_ERROR;

    return sock;
}

/*DWORD safesendto(SOCKET s,char* buffer,int size,int flags,sockaddr* addr,int addrsize)
{
    DWORD posn = 0;
    DWORD lastsend;
    while (posn < size)
    {
        lastsend = sendto(s,(char*)((DWORD)buffer + posn),size - posn,flags,addr,addrsize);
        if (lastsend == 0) return 0;
        posn += lastsend;
    }
    return posn;
} */

void DisableBlocking(int s)
{
    DWORD enable = 1;
    ioctlsocket(s,FIONBIO,&enable);
}

