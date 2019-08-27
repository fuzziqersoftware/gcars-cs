#include <windows.h>
#include <winsock2.h>
#include <stdio.h>

#include "wsa.h"
#include "gamecube.h"
#include "levine-ripper.h"
#include "levine-loaddol.h"
#include "levine-card.h"

extern WSADATA wsaData;

int sock;
sockaddr_in* saddr;
bool active = true;
char ip[512];

////////////////////////////////////////////////////////////////////////////////

DWORD handlethread();
bool prepcardfile(char*,char*,char*,char*,char*);

int main(int argc,char* argv[])
{
    DWORD x = 0,size,asize;
    char errors;
    DWORD offset = sizeof(sockaddr_in);
    DWORD off_gcc,off_com1,off_com2,off_name;
    char buffer[2048],sbuffer[2048],buffer2[2048];
    DWORD* buffer32 = (DWORD*)(&buffer[0]);
    bool console = true;
    sbuffer[0] = 0;

    printf("Fuzziqer Software Levine v0.6\n\n");
    SetConsoleTitle("Fuzziqer Software Levine v0.6");

    if (argc < 2)
    {
        printf("[Error] No target IP address; usage: levine.exe <IP address>\n",argv[0]);
        printf("        Enter target IP address: ");
        gets(&sbuffer[0]);
        strcpy(ip,sbuffer);
        printf("\n");
    } else strcpy(ip,argv[1]);
    if (argc > 2)
    {
        for (x = 2; x < argc; x++)
        {
            strcat(sbuffer,argv[x]);
            strcat(sbuffer," ");
        }
        sbuffer[strlen(sbuffer) - 1] = 0;
        console = false;
    }

    if (strlen(ip) < 7)
    {
        printf("[Error] Invalid IP address \"%s\"; too short\n",ip);
        return 0;
    }
    if (strlen(ip) > 15)
    {
        printf("[Error] Invalid IP address \"%s\"; too long\n",ip);
        return 0;
    }

    errors = FilterString_IP(ip);
    if (errors != 0)
    {
        printf("[Error] Invalid IP address \"%s\"; contains \'%c\'\n",ip,errors);
        return 0;
    }

    sock = WSAStartup(MAKEWORD(2,2),&wsaData);
    if (sock != 0)
    {
        printf("[Error] WSAStartup failed; code %d\n",WSAGetLastError());
        //WSACleanup();
        return 0;
    }
    WSASetLastError(0);
    sock = UDPOpenSocket(22065);
    if (sock == SOCKET_ERROR)
    {
        printf("[Error] UDPOpenSocket failed; code %d\n",WSAGetLastError());
        WSACleanup();
        return 0;
    }

    sockaddr_in sin,sin2,sin3;
    int sin_size = sizeof(sockaddr_in);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(22065);
    sin.sin_addr.s_addr = inet_addr(ip);
    saddr = &sin;

    DWORD enable = 1;
    ioctlsocket(sock,FIONBIO,&enable);

    if (console) printf("Verifying %s:%d > ",ip,22065);
    ZeroMemory(buffer,12);
    buffer32[0] = 0x89286847;
    size = sendto(sock,(char*)(&buffer[0]),12,0,(sockaddr*)&sin,sizeof(sockaddr_in));
    if (size != 12)
    {
        printf("[Error] sendto failed; target %s:%d, codes %d, %d\n",ip,22065,size,WSAGetLastError);
        return 0;
    }
    for (;;)
    {
        do {
            size = recvfrom(sock,(char*)(&buffer[0]),12,0,(sockaddr*)&sin2,(int*)&offset);
            Sleep(50);
        } while (size == SOCKET_ERROR);
        if (size != 12) printf("[Error] recvfrom failed; target %s:%d, codes %d, %d\n",ip,22065,size,WSAGetLastError);
        if ((sin2.sin_addr.s_addr == sin.sin_addr.s_addr) && (sin2.sin_port == sin.sin_port))
        {
            if (console) printf("Success\n\n");
            break;
        }
    }

    CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)handlethread,NULL,0,NULL);

    bool send = false;
    for (;;)
    {
        send = false;
        ZeroMemory(&buffer[0],1024);
        buffer32[0] = 0x89286847;
        buffer32[1] = 0;
        if (console)
        {
            ZeroMemory(&sbuffer[0],2048);
            printf("%s > ",ip);
            gets(&sbuffer[0]);
        }

        if (!buffer32[1] && !memcmp(sbuffer,"exit",4)) break;
        if (!buffer32[1] && !memcmp(sbuffer,"load",4)) buffer32[1] = 0xFFFFFFFC;
        if (!buffer32[1] && !memcmp(sbuffer,"re",2)) buffer32[1] = 0xFFFFFFFE;
        if (!buffer32[1] && !memcmp(sbuffer,"script",6)) buffer32[1] = 0x0301013A;
        if (!buffer32[1] && !memcmp(sbuffer,"gamename",8)) buffer32[1] = 0x0001003A;
        if (!buffer32[1] && !memcmp(sbuffer,"codename",8)) buffer32[1] = 0x0201003A;
        if (!buffer32[1] && !memcmp(sbuffer,"gameid",6)) buffer32[1] = 0x0101003A;
        if (!buffer32[1] && !memcmp(sbuffer,"game",4)) buffer32[1] = 0x0401003A;
        if (!buffer32[1] && !memcmp(sbuffer,"code",4)) buffer32[1] = 0x0301003A;
        if (!buffer32[1] && !memcmp(sbuffer,"cardfile",8)) buffer32[1] = 0x07020134;
        if (!buffer32[1] && !memcmp(sbuffer,"cardimage",8)) buffer32[1] = 0x08020134;
        if (!buffer32[1] && !memcmp(sbuffer,"carddol",7)) buffer32[1] = 0x09020134;
        if (!buffer32[1] && !memcmp(sbuffer,"verify",6))
        {
            printf("Verifying %s:%d > ",ip,22065);
            active = false;
            ZeroMemory(buffer,12);
            buffer32[0] = 0x89286847;
            size = sendto(sock,(char*)(&buffer[0]),12,0,(sockaddr*)&sin,sizeof(sockaddr_in));
            if (size != 12)
            {
                printf("[Error] sendto failed; target %s:%d, codes %d, %d\n",ip,22065,size,WSAGetLastError);
                return 0;
            }
            for (;;)
            {
                asize = sizeof(sockaddr_in);
                do {
                    size = recvfrom(sock,(char*)(buffer2),2048,0,(sockaddr*)&sin2,(int*)&asize);
                    Sleep(10);
                } while (size == SOCKET_ERROR);
                if (size != 12) printf("[Error] recvfrom failed; target %s:%d, codes %d, %d\n",ip,22065,size,WSAGetLastError);
                if ((sin2.sin_addr.s_addr == sin.sin_addr.s_addr) &&
                    (sin2.sin_port == sin.sin_port))
                {
                    printf("Success\n");
                    break;
                }
            }
            active = true;
            buffer32[1] = 1;
        }
        if (!buffer32[1])
        {
            printf("[Error] unknown command \"%s\"\n",sbuffer);
            buffer32[1] = 1;
        }

        for (x = 0; x < strlen(sbuffer); x++)
        {
            if (sbuffer[x] == ' ')
            {
                offset = x + 1;
                break;
            }
        }

        switch (buffer32[1])
        {
          case 0x0001003A: // game name 
          case 0x0201003A: // code name 
            errors = FilterString_GameCube(&sbuffer[offset]);
            if (errors != 0)
            {
                printf("[Error] character \'%c\' is unsupported\n",errors);
            } else {
                strcpy(&buffer[12],&sbuffer[offset]);
                size = strlen(&buffer[12]) + 1;
                buffer32[2] = byteswap(size);
                send = true;
            }
            break;
          case 0x0101003A: // game id 
            errors = FilterString_GameID(&sbuffer[offset]);
            if (errors != 0)
            {
                printf("[Error] character \'%c\' cannot be in a Game ID\n",errors);
            } else {
                if ((sbuffer[offset] != 'G') && (sbuffer[offset] != 'D'))
                {
                    printf("[Error] first character must be either \'G\' or \'D\'\n");
                } else {
                    if ((sbuffer[offset + 3] != 'E') && (sbuffer[offset + 3] != 'P') && (sbuffer[offset + 3] != 'J') && (sbuffer[offset + 3] != 'D') && (sbuffer[offset + 3] != 'U'))
                    {
                        printf("[Error] last character must be either \'E\', \'P\', \'J\', \'D\', or \'U\'\n");
                    } else {
                        memcpy(&buffer[12],&sbuffer[offset],4);
                        size = 4;
                        buffer32[2] = byteswap(4);
                        send = true;
                    }
                }
            }
            break;
          case 0x0401003A: // entire game 
            errors = FilterString_GameCube(&sbuffer[offset + 5]);
            if (errors != 0)
            {
                printf("[Error] character \'%c\' is unsupported\n",errors);
                break;
            }
            sbuffer[offset + 4] = 0;
            errors = FilterString_GameID(&sbuffer[offset]);
            if (errors != 0)
            {
                printf("[Error] character \'%c\' cannot be in a Game ID\n",errors);
                break;
            }
            if ((sbuffer[offset] != 'G') && (sbuffer[offset] != 'D'))
            {
                printf("[Error] first character of Game ID must be either \'G\' or \'D\'\n");
                break;
            }
            if ((sbuffer[offset + 3] != 'E') && (sbuffer[offset + 3] != 'P') && (sbuffer[offset + 3] != 'J') && (sbuffer[offset + 3] != 'D') && (sbuffer[offset + 3] != 'U'))
            {
                printf("[Error] last character of Game ID must be either \'E\', \'P\', \'J\', \'D\', or \'U\'\n");
                break;
            }
            strcpy(&buffer[12 + 0x00],&sbuffer[offset + 5]);
            memcpy(&buffer[12 + 0x40],&sbuffer[offset],4);
            ZeroMemory(&buffer[12 + 0x44],56);
            size = 124;
            buffer32[2] = byteswap(size);
            send = true;
            break;
          case 0x0301003A: // code 
          case 0x0301013A: // script 
            errors = FilterString_Hex(&sbuffer[offset]);
            if (errors != 0)
            {
                printf("[Error] character \'%c\' is not a hexadecimal character\n",errors);
            } else {
                x = 0;
                size = 0;
                for (;;)
                {
                    sscanf(&sbuffer[(x * 18) + offset],"%8X",&buffer32[(x * 2) + 3]);
                    sscanf(&sbuffer[(x * 18) + offset + 9],"%8X",&buffer32[(x * 2) + 4]);
                    buffer32[(x * 2) + 3] = byteswap(buffer32[(x * 2) + 3]);
                    buffer32[(x * 2) + 4] = byteswap(buffer32[(x * 2) + 4]);
                    size += 8;
                    if ((buffer32[(x * 2) + 3] == 0) && (buffer32[(x * 2) + 4] == 0)) break;
                    x++;
                }
                size -= 8;
                buffer32[2] = byteswap(size);
                send = true;
            }
            break;
          case 0x07020134:
            carduploadfile(&sbuffer[9]);
            break;
          case 0x08020134:
            buffer32[1] = 0x07020134;
            carduploadimage(&sbuffer[10]);
            break;
          case 0x09020134:
            buffer32[1] = 0x07020134;
            off_gcc = 8;
            size = strlen(sbuffer);
            for (x = 8; x < size; x++)
            {
                if (sbuffer[x] == ' ')
                {
                    sbuffer[x] = 0;
                    off_com1 = x + 1;
                    break;
                }
            }
            for (x = off_com1; x < size; x++)
            {
                if (sbuffer[x] == '*')
                {
                    sbuffer[x] = 0;
                    off_com2 = x + 1;
                    break;
                }
            }
            for (x = off_com2; x < size; x++)
            {
                if ((sbuffer[x] == '*') || (sbuffer[x] == 0))
                {
                    sbuffer[x] = 0;
                    off_name = x + 1;
                    break;
                }
            }
            sprintf(buffer,"%04X%04X%04X%04X",rand(),rand(),rand(),rand());
            prepcardfile(buffer,&sbuffer[off_name],&sbuffer[off_gcc],&sbuffer[off_com1],&sbuffer[off_com2]);
            carduploadfile(buffer);
            DeleteFile(buffer);
            break;
          case 0xFFFFFFFC:
            loaddol(&sbuffer[5]);
            break;
          case 0xFFFFFFFE:
            buffer32[1] = 0x00000000;
            buffer32[2] = 0xFFFFFFFF;
            size = 0;
            send = true;
            break;
          case 1:
            break;
          default:
            printf("[Error] unimplemented request; code %02X%02X%02X%02X\n",buffer[4],buffer[5],buffer[6],buffer[7]);
        }
        if (send)
        {
            asize = sendto(sock,(char*)(&buffer[0]),size + 12,0,(sockaddr*)&sin,sizeof(sockaddr_in));
            if (asize != (size + 12))
            {
                printf("[Error] sendto failed; target %s:%d, codes %d, %d, %d\n",ip,22065,size,buffer32[2] + 12,WSAGetLastError);
                return 0;
            }
        }
        if (!console) break;
    }

    if (!console) printf("The command was executed successfully.\n\n");

    closesocket(sock);
    WSACleanup();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////

char header[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x40,0x00,0x00,0x00,0x11,0x11,0xFF,0xFF,0x00,0x00,0x00,0x00};

bool prepcardfile(char* dest,char* src,char* gcc,char* com1,char* com2)
{
    DWORD x;
    bool isdol = false;
    char buffer[0x1000];
    char coms[0x40];
    HANDLE df = CreateFile(dest,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
    if (df == INVALID_HANDLE_VALUE) return false;
    HANDLE sf = CreateFile(src,GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
    if (sf == INVALID_HANDLE_VALUE)
    {
        CloseHandle(df);
        return false;
    }

    /*for (x = strlen(src); x > 0; x++) if (src[x] = '.') break;
    if (!strcmp(&src[x + 1],"dol")) isdol = true;
    if (!strcmp(&src[x + 1],"DOL")) isdol = true; */
    isdol = true;

    strcpy(&coms[0x00],com1);
    strcpy(&coms[0x20],com2);
    memcpy(&header[0],gcc,6);
    strcpy(&header[0x08],src);
    if (isdol) *(DWORD*)((DWORD)header + 0x3C) = 0x00010000;
    else *(DWORD*)((DWORD)header + 0x3C) = 0x00000000;
    DWORD size = GetFileSize(sf,NULL);
    *(WORD*)(&header[0x38]) = byteswaps((size + 0x1FFF + (isdol ? 0x40 : 0)) / 0x2000);
    WriteFile(df,header,0x40,&x,NULL);

    if (isdol)
    {
        ReadFile(sf,buffer,0x100,&x,NULL);
        for (x = 0; x < 18; x++) if (*(DWORD*)(&buffer[x * 4]) != 0) *(DWORD*)(&buffer[x * 4]) += 0x40000000;
        WriteFile(df,buffer,0x100,&x,NULL);
        WriteFile(df,coms,0x40,&x,NULL);
        size -= 0x100;
    }

    while (size > 0)
    {
        ReadFile(sf,buffer,(size > 0x1000 ? 0x1000 : size),&x,NULL);
        WriteFile(df,buffer,(size > 0x1000 ? 0x1000 : size),&x,NULL);
        size -= (size > 0x1000 ? 0x1000 : size);
    }
    CloseHandle(sf);
    CloseHandle(df);
    return true;
}

DWORD handlethread()
{
    DWORD x = 0,size,asize;
    char ip[512];
    char errors;
    DWORD offset = sizeof(sockaddr_in);
    DWORD buffer2[256];
    sockaddr_in sin3;
    int sin_size;
    for (;;)
    {
        ZeroMemory(buffer2,1024);
        do {
            sin_size = sizeof(sockaddr_in);
            size = recvfrom(sock,(char*)(buffer2),2048,0,(sockaddr*)&sin3,&sin_size);
            Sleep(10);
            while (!active) Sleep(100);
        } while (size == SOCKET_ERROR);
        if ((size < 12) || (sin3.sin_addr.s_addr != saddr->sin_addr.s_addr)) continue;

        switch (buffer2[3])
        {
          case 0x000000FF:
            ripper(buffer2[4],byteswap(buffer2[5]));
            break;
          case 0x00000042:
            if (buffer2[4] == checksum(&buffer2[5],0x40)) carddownloadfile(&buffer2[5]);
            else carddownloadfile(NULL);
            break;
          case 0x00000043:
            carddownloadimage(byteswap(buffer2[4]));
            break;
          default:
            break;
        }
    }
    return 0;
}
