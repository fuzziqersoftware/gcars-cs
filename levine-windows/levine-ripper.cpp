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

void ripper(DWORD gameid,DWORD numfiles)
{
    printf("        GCARS Ripper inited\n");

    char* cgid = (char*)(&gameid);
    char* names = (char*)malloc(131072);
    void* fstbuf = (char*)malloc(262144);
    DWORD nameoffset = 0;
    GCM_IMAGE* gcm = (GCM_IMAGE*)malloc(sizeof(GCM_IMAGE));
    FST_FILE ffile;
    FST_DIR fdir,*current;
    HANDLE file;
    DWORD x,y,fsize,size,asize,offset = 0,buffer32[512],buffer2[512],bytesread;
    DWORD checksums[64],windowpos,window = 1;
    char cbuffer[128];
    sockaddr_in sin3;
    int sin_size;
    bool send = false;

    ZeroMemory(gcm,sizeof(GCM_IMAGE));

    sprintf(cbuffer,"%c%c%c%c.GCM",cgid[0],cgid[1],cgid[2],cgid[3]);
    file = CreateFile(cbuffer,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,0,NULL);
    offset = 0;
    size = 4;
    buffer32[0] = 0x89286847;
    buffer32[1] = 0x22222222;
    buffer32[2] = byteswap(size);
    buffer32[3] = 0x00FF0010;
    current = &gcm->rootdir;
    printf("[FF]    Opened GCM image file: %s\n",cbuffer);
    asize = sendto(sock,(char*)buffer32,size + 12,0,(sockaddr*)saddr,sizeof(sockaddr_in));
    if (asize != (size + 12))
    {
        printf("[Error] sendto failed; codes %d, %d, %d\n",asize,size + 12,WSAGetLastError());
        return;
    }

    for (;;)
    {
        send = true;
        ZeroMemory(&buffer32[0],2048);
        ZeroMemory(&buffer2[0],2048);
        buffer32[0] = 0x89286847;
        buffer32[1] = 0x22222222;

        sin_size = sizeof(sockaddr_in);
        do {
            size = recvfrom(sock,(char*)(buffer2),2048,0,(sockaddr*)&sin3,&sin_size);
            if (size != SOCKET_ERROR)Sleep(10);
        } while (size == SOCKET_ERROR);
        if ((size < 12) || (sin3.sin_addr.s_addr != saddr->sin_addr.s_addr)) continue;

        if (buffer2[0] != 0x89286847) continue;
        if (buffer2[1] != 0x22222222) continue;
        switch (byteswap(buffer2[3]))
        {
          case 0x20000000:
            memcpy(&gcm->gcmheader,&buffer2[5],byteswap(buffer2[4]));
            SetFilePointer(file,0,NULL,FILE_BEGIN);
            WriteFile(file,&buffer2[5],byteswap(buffer2[4]),&bytesread,NULL);
            size = 8;
            buffer32[2] = byteswap(size);
            buffer32[3] = 0x00200010;
            buffer32[4] = buffer2[4];
            printf("[20]    Wrote GCM header: %08X %08X\n",offset,byteswap(buffer2[4]));
            offset += byteswap(buffer2[4]);
            break;
          case 0x31000000:
            memcpy(&gcm->gcmheader.unknown4[byteswap(buffer2[5])],&buffer2[6],byteswap(buffer2[4]));
            //SetFilePointer(file,0x440 + byteswap(buffer2[5]),NULL,FILE_BEGIN);
            WriteFile(file,&buffer2[5],byteswap(buffer2[4]),&bytesread,NULL);
            size = 8;
            buffer32[2] = byteswap(size);
            buffer32[3] = 0x00310010;
            buffer32[4] = buffer2[4];
            printf("[31]    Wrote unuse zone: %08X %08X\n",offset,byteswap(buffer2[4]));
            offset += byteswap(buffer2[4]);
            break;
          case 0x21000000:
            memcpy(&gcm->apploaderheader,&buffer2[5],byteswap(buffer2[4]));
            WriteFile(file,&buffer2[5],byteswap(buffer2[4]),&bytesread,NULL);
            size = 8;
            buffer32[2] = byteswap(size);
            buffer32[3] = 0x00210010;
            buffer32[4] = buffer2[4];
            printf("[21]    Wrote apploader header: %08X %08X\n",offset,byteswap(buffer2[4]));
            offset += byteswap(buffer2[4]);
            break;
          case 0x30000000:
            //checksums[windowpos] = 0;
            for (y = 0; y < (byteswap(buffer2[4]) / 4); y++) checksums[windowpos] ^= byteswap(buffer2[6 + y]);
            /*if (buffer2[5] != byteswap(checksums[windowpos])) checksums[windowpos] = 0;
            else checksums[windowpos] = 1; */
            WriteFile(file,&buffer2[6],byteswap(buffer2[4]),&bytesread,NULL);
            //printf("        > windowpos: %d, window: %d\n",windowpos,window);

            /*windowpos++;
            if (windowpos >= window)
            {
                windowpos = 0;
                for (y = 0; y < window; y++) if (!checksums[y]) break;
                window = y + 1;
                if (window > 32) window = 32; */
                size = 8;
                buffer32[2] = byteswap(size);
                buffer32[3] = 0x00300010;
                buffer32[4] = buffer2[4];
                //buffer32[5] = byteswap(window);
                send = true;
            //} else send = false;
            printf("[30]    Wrote block: %08X %08X %08X\n",offset,fsize,byteswap(buffer2[4]));
            offset += byteswap(buffer2[4]);
            fsize += byteswap(buffer2[4]);
            break;
          case 0x03000000:
            fsize = 0;
            switch (byteswap(buffer2[4]))
            {
              case 0xFFFFFFFF:
                current = fst_up(&gcm->rootdir,current);
                printf("[03]    Exited to directory: %s\n",current->name);
                break;
              case 0x00000000:
                fdir.numDirs = 0;
                fdir.numFiles = 0;
                fdir.dirs = NULL;
                fdir.files = NULL;
                fdir.name = (char*)((u32)names + nameoffset);
                strcpy(fdir.name,(char*)(&buffer2[5]));
                nameoffset += strlen(fdir.name) + 1;
                current = fst_add_dir(current,&fdir);
                printf("[03]    Created and entered directory: %s\n",current->name);
                break;
              default:
                offset = (offset + 0x1F) & ~0x1F;
                ffile.offset = offset;
                ffile.size = byteswap(buffer2[4]);
                ffile.name = (char*)((u32)names + nameoffset);
                strcpy(ffile.name,(char*)(&buffer2[5]));
                nameoffset += strlen(ffile.name) + 1;
                fst_add_file(current,&ffile);
                printf("[03] -- Created file: %s",ffile.name);
                if (!strcmp(ffile.name,"default.dol"))
                {
                    gcm->gcmheader.dolfileoffset = byteswap(offset);
                    printf(" [DOL file position reset]\n");
                } else printf("\n");
            }
            size = 8;
            buffer32[2] = byteswap(size);
            buffer32[3] = 0x00030010;
            buffer32[4] = buffer2[4];
            break;
          case 0x00000000:
            gcm->gcmheader.fstoffset = byteswap(offset);
            size = fst_unparse_dir(&gcm->rootdir,fstbuf,0xFFFFFFFF);
            SetFilePointer(file,0,NULL,FILE_BEGIN);
            WriteFile(file,gcm,sizeof(GCM_HEADER),&bytesread,NULL);
            SetFilePointer(file,offset,NULL,FILE_BEGIN);
            WriteFile(file,fstbuf,size,&bytesread,NULL);
            CloseHandle(file);
            free(names);
            free(fstbuf);
            free(gcm);
            printf("[00]    Ripping session ended\n");
            return;
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
}

