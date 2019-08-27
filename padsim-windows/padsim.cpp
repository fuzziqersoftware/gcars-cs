#include <windows.h>
#include <stdio.h>
#include <winsock2.h>

#include "pccs.h"

int main(int argc,char* argv[])
{
    printf("> Fuzziqer Software GCARS-CS Controller Simulator\n\n");

    char rem_ip_addr[16];
    char loc_ip_addr[16];
    DWORD gcGameID,padNum = 1;

    DWORD x,y;
    BYTE args = 0;

    loc_ip_addr[0] = 0;

    for (x = 1; x < argc; x++)
    {
        switch (argv[x][1])
        {
          case 'L':
          case 'l':
            strcpy(loc_ip_addr,&argv[x][2]);
            break;
          case 'R':
          case 'r':
            strcpy(rem_ip_addr,&argv[x][2]);
            args |= 1;
            break;
          case 'G':
          case 'g':
            memcpy(&gcGameID,&argv[x][2],4);
            gcGameID = byteswap(gcGameID);
            args |= 2;
            break;
          case 'P':
          case 'p':
            padNum = (atoi(&argv[x][2]) - 1);
            break;
          case 'H':
          case 'h':
            printf("> Usage: padsim.exe <options>\n");
            printf("> Option: -l<local IP> (not required)\n");
            printf("> Option: -r<remote IP> (required)\n");
            printf("> Option: -g<GC Game ID> (required)\n");
            printf("> Option: -p<player number> (default: player 2)\n");
            printf("\n> Press ENTER to exit....\n");
            getchar();
            return 0;
          default:
            printf("> Illegal argument: %s\n",argv[x]);
            printf("> Run this program with the \"-h\" switch for instructions\n");
            return 0;
        }
    }

    if ((args & 3) != 3)
    {
        printf("> Some arguments are missing; code %d\n",args ^ 3);
        printf("> Run this program with the \"-h\" switch for instructions\n");
        return 0;
    }

    WSADATA wsaData;
    int sock;
    DWORD errors;
    errors = WSAStartup(MAKEWORD(2,2),&wsaData);
    if (errors != NO_ERROR)
    {
        printf("> WSAStartup failed\n");
        return (-1);
    }

    sock = socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP);
    if (sock == INVALID_SOCKET)
    {
        printf("> socket() failed, code %d\n",WSAGetLastError());
        return (-1);
    }

    BYTE yes = true;
    errors = setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)(&yes),1);
    if (errors != NO_ERROR)
    {
        printf("> setsockopt() failed, Code %d\n",WSAGetLastError());
        return (-1);
    }

    sockaddr_in service,their_addr;
    sockaddr connection;
    int connection_size = sizeof(sockaddr_in);

    service.sin_family = AF_INET;
    if (loc_ip_addr[0] == 0) service.sin_addr.s_addr = INADDR_ANY;//FetchLocalAddress();
    else service.sin_addr.s_addr = inet_addr(loc_ip_addr);
    service.sin_port = htons(22064);

    errors = bind(sock,(SOCKADDR*)&service,sizeof(service));
    if (errors == SOCKET_ERROR)
    {
        printf("> bind() failed, code %d\n",WSAGetLastError());
        return (-1);
    }

    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(22064);
    their_addr.sin_addr.s_addr = inet_addr(rem_ip_addr);
    memset(&(their_addr.sin_zero),0,8);
    errors = connect(sock,(sockaddr*)&their_addr,sizeof(sockaddr));
    if (errors != NO_ERROR)
    {
        printf("> connect() failed, code %d\n",WSAGetLastError());
        return (-1);
    }

    DWORD enable = 1;
    ioctlsocket(sock,FIONBIO,&enable);

    CSPacket* csp = (CSPacket*)malloc(0x500);
    if (csp == NULL) return (-1);

    printf("> Press SPACE to change state, ENTER to start game\n");
    printf("> You are now Connected\n");
    BYTE state = 1;
    DWORD time = 0;
    for (;;)
    {
        if (GetAsyncKeyState(VK_SPACE) & 0x8000)
        {
            if (state == 1)
            {
                state = 2;
                printf("> You are now Ready\n");
            } else {
                state = 1;
                printf("> You are now Connected\n");
            }
            while (GetAsyncKeyState(VK_SPACE) & 0x8000) { }
        }
        if (GetAsyncKeyState(VK_RETURN) & 0x8000)
        {
            csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
            csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
            csp->size         = byteswap((DWORD)(sizeof(CSPacket)));
            csp->padspresent  = byteswap((WORD)COMMAND_START);
            csp->gcGameID     = byteswap(gcGameID);
            csp->numCommands  = 0x00000000;
            csp->checksum     = CSChecksum(csp);
            send(sock,(const char*)csp,byteswap(csp->size),0);
            break;
        }
        if ((time + 1000) < GetTickCount())
        {
            time = GetTickCount();
            csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
            csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
            csp->size         = byteswap((DWORD)(sizeof(CSPacket)));
            csp->padspresent  = byteswap((WORD)COMMAND_STATUS | (padNum << 4) | state);
            csp->gcGameID     = byteswap(gcGameID);
            csp->numCommands  = 0x00000000;
            csp->checksum     = CSChecksum(csp);
            send(sock,(const char*)csp,byteswap(csp->size),0);
        }
    }

    printf("> Controller Simulator has started, press ESCAPE to exit\n\n");

    for (;;)
    {
        csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
        csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
        csp->size         = byteswap((DWORD)(sizeof(CSPacket) + sizeof(CSPADStatus)));
        csp->padspresent  = byteswap((WORD)COMMAND_WRITE | padNum);
        csp->gcGameID     = byteswap(gcGameID);
        csp->numCommands  = 0x00000000;
        PAD_ReadState((CSPADStatus*)((DWORD)csp + sizeof(CSPacket)));
        csp->checksum     = CSChecksum(csp);
        send(sock,(const char*)csp,byteswap(csp->size),0);
        //printf("> %04X %02X-%02X %02X-%02X %02X\n",byteswap(csp->padStatus.buttons),(BYTE)csp->padStatus.controlX,(BYTE)csp->padStatus.controlY,(BYTE)csp->padStatus.cstickX,(BYTE)csp->padStatus.cstickY,csp->padStatus.error);
        recv(sock,(char*)csp,0x500,0);
        Sleep(60);
        if (GetAsyncKeyState(VK_ESCAPE) & 0x8000) break;
    }

    if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
    {
        csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
        csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
        csp->size         = byteswap((DWORD)(sizeof(CSPacket) + sizeof(CSPADStatus) + 16));
        csp->padspresent  = byteswap((WORD)COMMAND_WRITE | padNum);
        csp->gcGameID     = byteswap(gcGameID);
        csp->numCommands  = 0x00000001;
        memcpy((void*)((DWORD)csp + sizeof(CSPacket) + sizeof(CSPADStatus)),"\xC7\x00\x30\x24\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",16);
        csp->checksum     = CSChecksum(csp);
        send(sock,(const char*)csp,byteswap(csp->size),0);
        printf("> System reset\n");
    } else if (GetAsyncKeyState(VK_CONTROL) & 0x8000)
    {
        DWORD bytesread,ssize;
        u32 buffer[0x100];
        HANDLE file = CreateFile("gcars.dol",GENERIC_READ,0,NULL,OPEN_EXISTING,0,NULL);
        if (file == INVALID_HANDLE_VALUE) printf("> File could not be opened\n");
        ReadFile(file,buffer,0x100,&bytesread,NULL);
        printf("> Pausing system\n");
        csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
        csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
        csp->size         = byteswap((DWORD)(sizeof(CSPacket) + sizeof(CSPADStatus) + 16));
        csp->padspresent  = byteswap((WORD)COMMAND_DBG_PAUSE);
        csp->gcGameID     = byteswap(gcGameID);
        csp->numCommands  = byteswap((DWORD)0xFFFFFFFF);
        csp->checksum     = CSChecksum(csp);
        send(sock,(const char*)csp,byteswap(csp->size),0);
        for (;;)
        {
            bytesread = recv(sock,(char*)csp,0x500,0);
            if (bytesread > 0)
            {
                printf("incoming command: %04X\n",byteswap(csp->padspresent));
                if (csp->padspresent == byteswap((WORD)COMMAND_DISCONNECT)) break;
            }
        }
        for (x = 0; x < 18; x++)
        {
            printf("> Loading section %d: %08X %08X %08X\n",x,byteswap(buffer[x]),byteswap(buffer[x + 18]),byteswap(buffer[x + 36]));
            SetFilePointer(file,byteswap(buffer[x]),NULL,FILE_BEGIN);
            for (y = 0; y < byteswap(buffer[36 + x]); y += 0x400)
            {
                ssize = (byteswap(buffer[36 + x]) - y);
                if (ssize > 0x400) ssize = 0x400;
                csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
                csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
                csp->size         = byteswap((DWORD)(sizeof(CSPacket)) + ssize + 4);
                csp->padspresent  = byteswap((WORD)COMMAND_DBG_WRITEMEM);
                csp->gcGameID     = byteswap(gcGameID);
                csp->numCommands  = byteswap(byteswap(buffer[18 + x]) + y);
                *(DWORD*)((DWORD)csp + sizeof(CSPacket)) = ssize;
                ReadFile(file,(void*)((DWORD)csp + sizeof(CSPacket) + 4),ssize,&bytesread,NULL);
                csp->checksum     = CSChecksum(csp);
                send(sock,(const char*)csp,byteswap(csp->size),0);
                for (;;)
                {
                    bytesread = recv(sock,(char*)csp,0x500,0);
                    if ((bytesread > 0) & (bytesread < 0x500))
                    {
                        printf("incoming command: %04X\n",byteswap(csp->padspresent));
                        if (csp->padspresent == byteswap((WORD)COMMAND_DBG_WRITEMEMREPLY)) break;
                    }
                    Sleep(10);
                }
                printf("> Data loaded: %08X %08X %08X\n",byteswap(buffer[18 + x]) + y,y,ssize);
                Sleep(80);
            }
        }
        printf("> Launching DOL file at %08X\n",buffer[0x38]);
        csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
        csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
        csp->size         = byteswap((DWORD)(sizeof(CSPacket)));
        csp->padspresent  = byteswap((WORD)COMMAND_DBG_BRANCH);
        csp->gcGameID     = byteswap(gcGameID);
        csp->numCommands  = buffer[0x38]; // entrypoint 
        csp->checksum     = CSChecksum(csp);
        send(sock,(const char*)csp,byteswap(csp->size),0);
        CloseHandle(file);
    } else {
        csp->verifier     = byteswap((DWORD)CONTROLSIM_VERIFIER);
        csp->version      = byteswap((DWORD)CONTROLSIM_VERSION);
        csp->size         = byteswap((DWORD)(sizeof(CSPacket)));
        csp->padspresent  = byteswap((WORD)(COMMAND_DISCONNECT | padNum));
        csp->gcGameID     = byteswap(gcGameID);
        csp->numCommands  = byteswap((DWORD)0x00000000);
        csp->checksum     = CSChecksum(csp);
        send(sock,(const char*)csp,byteswap(csp->size),0);
        printf("> Player %d disconnected\n",padNum);
    }

    closesocket(sock);
    WSACleanup();

    printf("> Press ENTER to continue....\n");
    getchar();

    return 0;
}

