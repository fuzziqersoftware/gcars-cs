#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

#include "wsa.h"
#include "gamecube.h"
#include "fst.h"
#include "gcm.h"
#include "levine-ripper.h"

extern int sock;
extern sockaddr_in* saddr;
extern bool active;
extern char ip[];

////////////////////////////////////////////////////////////////////////////////

void carduploadfile(char* filename)
{
    DWORD asize,size,fsize,bufferout[512],bufferin[512];
    long offset;
    HANDLE file;

    bool send;
    int sin_size;
    DWORD bytesread;
    sockaddr_in sin3;

    active = false;
    file = CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        printf("[Error] File not found\n");
        return;
    }
    offset = 0;
    size = 0x48;
    bufferout[0] = 0x89286847;
    bufferout[1] = 0x07020134;
    bufferout[2] = byteswap(size);
    bufferout[3] = 0x00000040;
    ReadFile(file,&bufferout[5],0x40,&bytesread,NULL);
    bufferout[4] = checksum(&bufferout[5],0x40);
    asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
    if (asize != (size + 12))
    {
        printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
        CloseHandle(file);
        return;
    }

    fsize = GetFileSize(file,NULL) - 0x40;
    while (offset < fsize)
    {
        send = true;
        ZeroMemory(&bufferin[0],1024);
        bufferout[0] = 0x89286847;
        bufferout[1] = 0x07020134;

        sin_size = sizeof(sockaddr_in);
        do {
            asize = recvfrom(sock,(char*)(bufferin),2048,0,(sockaddr*)&sin3,&sin_size);
            if (asize != SOCKET_ERROR)Sleep(10);
        } while (asize == SOCKET_ERROR);
        if ((asize < 12) || (sin3.sin_addr.s_addr != saddr->sin_addr.s_addr)) continue;
        if (bufferin[0] != 0x89286847) continue;
        if (bufferin[1] != 0x07020134) continue;

        switch (byteswap(bufferin[3]))
        {
          case 0x10004000:
            printf("[10004000]");
            if (byteswap(bufferin[4]) != 0x00000001)
            {
                printf("[Error] Could not init file transfer\n");
                CloseHandle(file);
                return;
            }
            size = 0x40C;
            bufferout[2] = byteswap(size);
            bufferout[3] = 0x00000045;
            bufferout[4] = 0x00040000;
            ReadFile(file,&bufferout[6],0x400,&bytesread,NULL);
            bufferout[5] = checksum(&bufferout[6],0x400);
            offset += 0x400;
            break;
          case 0x10004500:
            if (byteswap(bufferin[4]) == 0x00000001)
            {
                size = 0x40C;
                bufferout[2] = byteswap(size);
                bufferout[3] = 0x00000045;
                bufferout[4] = 0x00040000;
                ReadFile(file,&bufferout[6],0x400,&bytesread,NULL);
                bufferout[5] = checksum(&bufferout[6],0x400);
                offset += 0x400;
            }
            break;
          default:
            send = false;
        }
        if (send)
        {
            asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
            if (asize != (size + 12))
            {
                printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
                CloseHandle(file);
                return;
            }
        }
    }
    CloseHandle(file);
    active = true;
}

void carduploadimage(char* filename)
{
    DWORD asize,size,fsize,bufferout[512],bufferin[512];
    long offset;
    HANDLE file;

    bool send;
    int sin_size;
    DWORD bytesread;
    sockaddr_in sin3;

    active = false;
    file = CreateFile(filename,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        printf("[Error] File not found\n");
        return;
    }
    fsize = GetFileSize(file,NULL);
    offset = 0;
    size = 8;
    bufferout[0] = 0x89286847;
    bufferout[1] = 0x07020134;
    bufferout[2] = byteswap(size);
    bufferout[3] = 0x00000041;
    bufferout[4] = byteswap(fsize);
    asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
    if (asize != (size + 12))
    {
        printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
        CloseHandle(file);
        return;
    }

    while (offset < fsize)
    {
        send = true;
        ZeroMemory(&bufferin[0],1024);
        bufferout[0] = 0x89286847;
        bufferout[1] = 0x07020134;

        sin_size = sizeof(sockaddr_in);
        do {
            asize = recvfrom(sock,(char*)(bufferin),2048,0,(sockaddr*)&sin3,&sin_size);
            if (asize != SOCKET_ERROR)Sleep(10);
        } while (asize == SOCKET_ERROR);
        if ((asize < 12) || (sin3.sin_addr.s_addr != saddr->sin_addr.s_addr)) continue;
        if (bufferin[0] != 0x89286847) continue;
        if (bufferin[1] != 0x07020134) continue;

        switch (byteswap(bufferin[3]))
        {
          case 0x10004100:
            if (bufferin[4] != 0x01000000)
            {
                printf("[Error] Could not init image transfer\n");
                CloseHandle(file);
                return;
            }
          case 0x10004500:
            if (bufferin[4] == 0x01000000)
            {
                size = 0x40C;
                bufferout[2] = byteswap(size);
                bufferout[3] = 0x00000045;
                bufferout[4] = 0x00040000;
                ReadFile(file,&bufferout[6],0x400,&bytesread,NULL);
                bufferout[5] = checksum(&bufferout[6],0x400);
                printf("> sending offset %08X\n",offset);
                offset += 0x400;
            } else printf("> error; resending offset %08X\n",offset);
            break;
          default:
            send = false;
        }
        if (send)
        {
            asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
            if (asize != (size + 12))
            {
                printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
                CloseHandle(file);
                return;
            }
        }
    }
    CloseHandle(file);
    active = true;
}

////////////////////////////////////////////////////////////////////////////////

void carddownloadfile(void* header)
{
    DWORD asize,size,fsize,bufferout[512],bufferin[512];
    long offset;
    HANDLE file;

    bool send;
    int sin_size;
    DWORD bytesread;
    sockaddr_in sin3;

    if (header == NULL)
    {
        size = 8;
        bufferout[0] = 0x89286847;
        bufferout[1] = 0x07020134;
        bufferout[2] = byteswap(size);
        bufferout[3] = 0x00420010;
        bufferout[4] = 0x00000000;
        asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
        if (asize != (size + 12)) printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
        return;
    }

    file = CreateFile((char*)((DWORD)header + 8),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        printf("[Error] File could not be created\n");
        return;
    }
    WriteFile(file,header,0x40,&bytesread,NULL);
    offset = 0;
    size = 8;
    bufferout[0] = 0x89286847;
    bufferout[1] = 0x07020134;
    bufferout[2] = byteswap(size);
    bufferout[3] = 0x00420010;
    bufferout[4] = 0x01000000;
    asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
    if (asize != (size + 12))
    {
        printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
        CloseHandle(file);
        return;
    }

    fsize = byteswaps(*(WORD*)((DWORD)header + 0x38)) * 0x2000;
    while (offset < fsize)
    {
        send = true;
        ZeroMemory(&bufferin[0],1024);
        bufferout[0] = 0x89286847;
        bufferout[1] = 0x07020134;

        sin_size = sizeof(sockaddr_in);
        do {
            asize = recvfrom(sock,(char*)(bufferin),2048,0,(sockaddr*)&sin3,&sin_size);
            if (asize != SOCKET_ERROR)Sleep(10);
        } while (asize == SOCKET_ERROR);
        if ((asize < 12) || (sin3.sin_addr.s_addr != saddr->sin_addr.s_addr)) continue;
        if (bufferin[0] != 0x89286847) continue;
        if (bufferin[1] != 0x07020134) continue;

        switch (byteswap(bufferin[3]))
        {
          case 0x45000000:
            size = 8;
            bufferout[2] = byteswap(size);
            bufferout[3] = 0x00450010;
            if (bufferin[5] == checksum(&bufferin[6],byteswap(bufferin[4])))
            {
                WriteFile(file,&bufferin[6],byteswap(bufferin[4]),&bytesread,NULL);
                bufferout[4] = 0x01000000;
                offset += 0x400;
            } else bufferout[4] = 0x00000000;
            break;
          default:
            send = false;
        }
        if (send)
        {
            asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
            if (asize != (size + 12))
            {
                printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
                CloseHandle(file);
                return;
            }
        }
    }
    CloseHandle(file);
}

void carddownloadimage(DWORD fsize)
{
    DWORD asize,size,bufferout[512],bufferin[512];
    long offset;
    HANDLE file;

    bool send;
    int sin_size;
    DWORD bytesread;
    sockaddr_in sin3;

    file = CreateFile("card.dat",GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        printf("[Error] File could not be created\n");
        return;
    }
    offset = 0;
    size = 8;
    bufferout[0] = 0x89286847;
    bufferout[1] = 0x07020134;
    bufferout[2] = byteswap(size);
    bufferout[3] = 0x00430010;
    bufferout[4] = 0x01000000;
    asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
    if (asize != (size + 12))
    {
        printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
        CloseHandle(file);
        return;
    }

    while (offset < fsize)
    {
        send = true;
        ZeroMemory(&bufferin[0],1024);
        bufferout[0] = 0x89286847;
        bufferout[1] = 0x07020134;

        sin_size = sizeof(sockaddr_in);
        do {
            asize = recvfrom(sock,(char*)(bufferin),2048,0,(sockaddr*)&sin3,&sin_size);
            if (asize != SOCKET_ERROR)Sleep(10);
        } while (asize == SOCKET_ERROR);
        if ((asize < 12) || (sin3.sin_addr.s_addr != saddr->sin_addr.s_addr)) continue;
        if (bufferin[0] != 0x89286847) continue;
        if (bufferin[1] != 0x07020134) continue;

        switch (byteswap(bufferin[3]))
        {
          case 0x45000000:
            size = 8;
            bufferout[2] = byteswap(size);
            bufferout[3] = 0x00450010;
            if (bufferin[5] == checksum(&bufferin[6],byteswap(bufferin[4])))
            {
                WriteFile(file,&bufferin[6],byteswap(bufferin[4]),&bytesread,NULL);
                bufferout[4] = 0x01000000;
                offset += 0x400;
            } else bufferout[4] = 0x00000000;
            break;
          default:
            send = false;
        }
        if (send)
        {
            asize = sendto(sock,(char*)bufferout,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
            if (asize != (size + 12))
            {
                printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
                CloseHandle(file);
                return;
            }
        }
    }
    CloseHandle(file);
}

