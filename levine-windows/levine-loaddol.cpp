#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

#include "wsa.h"
#include "gamecube.h"
#include "levine-loaddol.h"

extern int sock;
extern sockaddr_in* saddr;
extern bool active;

void loaddol(char* filename)
{
    HANDLE file;
    DWORD x,filesize,fileoff,size,asize,offset = 0,buffer32[512],buffer2[512],bytesread;
    sockaddr_in sin3;
    int sin_size;
    bool send = false;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD loc;

    file = CreateFile(filename,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (file == INVALID_HANDLE_VALUE)
    {
        printf("[Error] %s could not be opened\n",filename);
        return;
    }
    fileoff = 0;
    filesize = GetFileSize(file,NULL);

    printf("        GCARS Loader inited\n");
    active = false;
    //WSACancelBlockingCall();

    size = 0;
    buffer32[0] = 0x89286847;
    buffer32[1] = 0x00000000;
    buffer32[2] = 0xFCFFFFFF;
    asize = sendto(sock,(char*)buffer32,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
    if (asize != (size + 12))
    {
        printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 2,WSAGetLastError());
        return;
    }
    printf("        Sending DOL upload command\n");

    size = 0x104;
    buffer32[0] = 0x89286847;
    buffer32[1] = 0xFDFFFFFF;
    buffer32[2] = 0x04010000;
    ReadFile(file,&buffer32[3],0x100,&bytesread,NULL);
    buffer32[0x43] = checksum(&buffer32[3],0x100);
    asize = sendto(sock,(char*)buffer32,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
    if (asize != (size + 12))
    {
        printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
        return;
    }
    fileoff += 0x100;
    printf("        Sending DOL header\n");
    printf("        Sending data: 00000000 / %08X",filesize);

    while (fileoff < filesize)
    {
        send = true;
        ZeroMemory(&buffer32[0],2048);
        ZeroMemory(&buffer2[0],2048);
        buffer32[0] = 0x89286847;
        buffer32[1] = 0xFDFFFFFF;

        sin_size = sizeof(sockaddr_in);
        do {
            Sleep(10);
            size = recvfrom(sock,(char*)(buffer2),2048,0,(sockaddr*)&sin3,&sin_size);
        } while (size == SOCKET_ERROR);
        if ((size < 12) || (sin3.sin_addr.s_addr != saddr->sin_addr.s_addr)) continue;

        if (buffer2[0] != 0x89286847) continue;
        if (buffer2[1] != 0xFDFFFFFF) continue;
        switch (byteswap(buffer2[3]))
        {
          case 0x10000000:
            fileoff = byteswap(buffer2[4]);
            SetFilePointer(file,byteswap(buffer2[4]),NULL,FILE_BEGIN);
            buffer32[0] = 0x89286847;
            buffer32[1] = 0xFDFFFFFF;
            ReadFile(file,&buffer32[3],0x400,&bytesread,NULL);
            bytesread &= ~3;
            size = bytesread + 4;
            buffer32[2] = byteswap(bytesread + 4);
            buffer32[3 + (bytesread / 4)] = checksum(&buffer32[3],bytesread);

            GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE),&csbi);
            loc.X = csbi.dwCursorPosition.X - 19;
            loc.Y = csbi.dwCursorPosition.Y;
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE),loc);
            printf("%08X / %08X",fileoff,filesize);

            send = true;
            break;
          default:
            send = false;
        }
        if (send)
        {
            asize = sendto(sock,(char*)buffer32,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
            if (asize != (size + 12))
            {
                printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
                return;
            }
        }
    }
    printf("\n        Finished sending %s\n",filename);
    CloseHandle(file);
    active = true;
}
