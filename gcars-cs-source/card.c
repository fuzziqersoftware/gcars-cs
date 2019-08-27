#include "gamecube.h"

/*CARD_DirBlock dirblock[2] __attribute__((aligned(32)));
CARD_FatBlock fatblock[2] __attribute__((aligned(32)));
unsigned char sysarea[CARD_SYSAREA*CARD_SECTORSIZE] __attribute__((aligned(32)));
unsigned long curslot,numsectors;
int card_init = 0,curdir,curfat; */

CARD_DirBlock* dirblock = NULL;
CARD_FatBlock* fatblock = NULL;
unsigned char* sysarea = NULL;
unsigned long curslot,numsectors;
int card_init = 0,curdir,curfat;

unsigned short CARD_RetrieveID(unsigned long channel)
{
    int i;
    unsigned char cbuf[4];
    exi_select(channel,0,4);
    cbuf[0] = 0x85;
    cbuf[1] = 0x00;
    exi_imm(channel,cbuf,2,EXI_WRITE);
    exi_imm(channel,cbuf,2,EXI_READ);
    exi_deselect(channel);
    return *(unsigned short*)cbuf;
}

unsigned char CARD_ReadStatus(unsigned long channel)
{
    unsigned char cbuf[4];
    exi_select(channel,0,4);
    cbuf[0] = 0x83;
    cbuf[1] = 0x00;
    exi_imm(channel,cbuf,2,EXI_WRITE);
    exi_imm(channel,cbuf,1,EXI_READ);
    exi_deselect(channel);
    return cbuf[0];
}

void CARD_ClearStatus(unsigned long channel)
{
    int i;
    unsigned char cbuf[4];
    exi_select(channel,0,4);
    cbuf[0] = 0x89;
    exi_imm(channel,cbuf,1,EXI_WRITE);
    exi_deselect(channel);
}

void CARD_ChipErase(unsigned long channel)
{
    int i;
    unsigned char cbuf[3];
    exi_select(channel,0,4);
    cbuf[0] = 0xF4;
    cbuf[1] = 0x00;
    cbuf[2] = 0x00;
    exi_imm(channel,cbuf,3,EXI_WRITE);
    exi_deselect(channel);
}

void CARD_SectorErase(unsigned long channel,unsigned long sector)
{
    int i;
    unsigned char cbuf[3];
    while (!(CARD_ReadStatus(channel) & 1));
    exi_select(channel,0,4);
    cbuf[0] = 0xF1;
    cbuf[1] =(sector >> 17) & 0x7F;
    cbuf[2] =(sector >> 9) & 0xFF;
    exi_imm(channel,cbuf,3,EXI_WRITE);
    exi_deselect(channel);
    //while (CARD_ReadStatus(channel) & 0x8000);
    while (CARD_ReadStatus(channel) & 0x80);
}

void CARD_SectorProgram(unsigned long channel,unsigned char *abuf,unsigned long address,unsigned long size)
{
    int i;
    unsigned char cbuf[4];
    unsigned char *bbuf = abuf;

    for (i = 0;i < size / CARD_WRITESIZE;i++)
    {
        while (!(CARD_ReadStatus(channel) & 1));
        exi_select(channel,0,4);
        cbuf[0] = 0xF2;
        cbuf[1] =(address >> 17) & 0x3F;
        cbuf[2] =(address >> 9) & 0xFF;
        cbuf[3] =(address >> 7) & 3;
        exi_imm(channel,cbuf,4,EXI_WRITE);
        cbuf[0] = address & 0x7F;
        exi_imm(channel,cbuf,1,EXI_WRITE);
        exi_dma(channel,bbuf,CARD_WRITESIZE,EXI_WRITE);
        exi_deselect(channel);
        //while (CARD_ReadStatus(channel) & 0x8000);
        while (CARD_ReadStatus(channel) & 0x80);
        address += CARD_WRITESIZE;
        bbuf += CARD_WRITESIZE;
    }
}

void CARD_ReadArray(unsigned long channel,unsigned char *abuf,unsigned long address,unsigned long size)
{
    int i;
    unsigned char cbuf[4];
    unsigned char *bbuf = abuf;

    for (i = 0;i < size / CARD_READSIZE;i++)
    {
        while (!(CARD_ReadStatus(channel) & 1));
        exi_select(channel,0,4);
        cbuf[0] = 0x52;
        cbuf[1] =(address >> 17) & 0x3F;
        cbuf[2] =(address >> 9) & 0xFF;
        cbuf[3] =(address >> 7) & 3;
        exi_imm(channel,cbuf,4,EXI_WRITE);
        cbuf[0] = address & 0x7F;
        exi_imm(channel,cbuf,1,EXI_WRITE);
        cbuf[0] = 0;
        cbuf[1] = 0;
        cbuf[2] = 0;
        cbuf[3] = 0;
        exi_imm(channel,cbuf,4,EXI_WRITE);
        exi_dma(channel,bbuf,CARD_READSIZE,EXI_READ);
        exi_deselect(channel);
        address += CARD_READSIZE;
        bbuf += CARD_READSIZE;
    }
}

int CARD_IsPresent(unsigned long channel)
{
    unsigned long id;
    id = exi_deviceid(channel,0);
    if ((id & 0xffff0000) ||(id & 3)) return 0;
    return id;
}

void CARD_Checksum(unsigned short *buf,int len,unsigned short *cs1,unsigned short *cs2)
{
    unsigned short val;
    int i;
    *cs1 = 0;
    *cs2 = 0;
    len /= 2;
    for (i = 0;i < len;i++)
    {
        val = *cs1;
        *cs1 = buf[i] + val;
        val = *cs2;
        *cs2 = ~buf[i] + val;
    }
    if (*cs1 == 0xffff) *cs1 = 0;
    if (*cs2 == 0xffff) *cs2 = 0;
}

void CARD_UpdateDir(unsigned long channel)
{
    dirblock[curdir].num++;
    CARD_Checksum((unsigned short *)&dirblock[curdir],0x1ffc,&dirblock[curdir].checksum1,&dirblock[curdir].checksum2);
    dcache_flush(&dirblock[curdir],sizeof(CARD_DirBlock));
    CARD_SectorErase(channel,curdir*0x2000+0x2000);
    CARD_SectorProgram(channel,(unsigned char *)&dirblock[curdir],curdir*0x2000+0x2000,0x2000);
    curdir ^= 1;
    memcpy(&dirblock[curdir],&dirblock[curdir^1],sizeof(CARD_DirBlock));
}

void CARD_UpdateFat(unsigned long channel)
{
    fatblock[curfat].num++;
    CARD_Checksum((unsigned short *)&fatblock[curfat]+2,0x1ffc,&fatblock[curfat].checksum1,&fatblock[curfat].checksum2);
    dcache_flush(&fatblock[curfat],sizeof(CARD_FatBlock));
    CARD_SectorErase(channel,curfat*0x2000+0x6000);
    CARD_SectorProgram(channel,(unsigned char *)&fatblock[curfat],curfat*0x2000+0x6000,0x2000);
    curfat ^= 1;
    memcpy(&fatblock[curfat],&fatblock[curfat^1],sizeof(CARD_FatBlock));
}

int CARD_Check(unsigned long channel,int *fixed)
{
    unsigned short checksum1,checksum2;
    int i,bad = 0;

    *fixed = 0;

    for (i = 0;i < 2;i++)
    {
        CARD_Checksum((unsigned short *)&dirblock[i],0x1ffc,&checksum1,&checksum2);
        if (checksum1 != dirblock[i].checksum1 || checksum2 != dirblock[i].checksum2) bad |= (i + 1);
    }

    if (bad & 3)
    {
        if (bad == 3) return CARD_ERROR_CORRUPT;
        if (bad == 1)
        {
            memcpy(&dirblock[0],&dirblock[1],sizeof(CARD_DirBlock));
            curdir = 0;
        } else {
            memcpy(&dirblock[1],&dirblock[0],sizeof(CARD_DirBlock));
            curdir = 1;
        }
        *fixed |= 1;
    } else {
        if (dirblock[0].num < dirblock[1].num)
        {
            memcpy(&dirblock[0],&dirblock[1],sizeof(CARD_DirBlock));
            curdir = 0;
        } else {
            memcpy(&dirblock[1],&dirblock[0],sizeof(CARD_DirBlock));
            curdir = 1;
        }
    }
    bad = 0;
    for (i = 0;i < 2;i++)
    {
        CARD_Checksum((unsigned short *)&fatblock[i]+2,0x1ffc,&checksum1,&checksum2);
        if (checksum1 != fatblock[i].checksum1 || checksum2 != fatblock[i].checksum2) bad |=(i + 1);
    }

    if (bad & 3)
    {
        if (bad == 3) return CARD_ERROR_CORRUPT;
        if (bad == 1)
        {
            memcpy(&fatblock[0],&fatblock[1],sizeof(CARD_FatBlock));
            curfat = 0;
        } else {
            memcpy(&fatblock[1],&fatblock[0],sizeof(CARD_FatBlock));
            curfat = 1;
        }
        *fixed |= 2;
    } else {
        if (fatblock[0].num < fatblock[1].num)
        {
            memcpy(&fatblock[0],&fatblock[1],sizeof(CARD_FatBlock));
            curfat = 0;
        } else {
            memcpy(&fatblock[1],&fatblock[0],sizeof(CARD_FatBlock));
            curfat = 1;
        }
    }
    return CARD_ERROR_NONE;
}

int CARD_GetOffsets(CARD_File *cfile,CARD_Offsets *offsets)
{
    unsigned long addr,bsize,isize;
    int i,format,needs_tlut = 0;

    if (card_init != 1) return CARD_ERROR_INIT;
    if (cfile->filenum < 0 || cfile->filenum >= CARD_MAXFILES) return CARD_ERROR_FATAL;

    addr = cfile->icon_addr;
    bsize = CARD_BANNER_W * CARD_BANNER_H;
    isize = CARD_ICON_W * CARD_ICON_H;

    if (cfile->banner_fmt & CARD_BANNER_CI)
    {
        offsets->banner = addr;
        offsets->banner_tlut = addr + bsize;
        addr += bsize + 512;
    } else if (cfile->banner_fmt & CARD_BANNER_RGB)
    {
        offsets->banner = addr;
        offsets->banner_tlut = 0xffff;
        addr += bsize * 2;
    } else {
        offsets->banner = 0xffff;
        offsets->banner_tlut = 0xffff;
    }

    for (i = 0;i < CARD_MAXICONS;i++)
    {
        format = cfile->icon_fmt >> (i * 2);
        if (format & CARD_ICON_CI)
        {
            offsets->icons[i] = addr;
            addr += isize;
            needs_tlut = 1;
        } else if (format & CARD_ICON_RGB)
        {
            offsets->icons[i] = addr;
            addr += isize * 2;
        } else offsets->icons[i] = 0xffff;
    }

    if (needs_tlut)
    {
        offsets->icons_tlut = addr;
        addr += 512;
    } else offsets->icons_tlut = 0xffff;
    offsets->data = addr;
    return CARD_ERROR_NONE;
}

int CARD_Begin(unsigned long channel,int *fixed)
{
    int i,id,ret;
    card_init = 0;
    if (channel < 0 || channel >= 2) return CARD_ERROR_FATAL;
    id = CARD_IsPresent(channel);
    if (id == 0) return CARD_ERROR_NOCARD;

    // 3 lines for replacement
    if (!dirblock) dirblock = (CARD_DirBlock*)malloc(2 * sizeof(CARD_DirBlock));
    if (!fatblock) fatblock = (CARD_FatBlock*)malloc(2 * sizeof(CARD_FatBlock));
    if (!sysarea) sysarea = (unsigned char*)malloc(CARD_SYSAREA * CARD_SECTORSIZE);

    dcache_inv(sysarea,CARD_SYSAREA*CARD_SECTORSIZE);
    CARD_ReadArray(channel,sysarea,0,CARD_SYSAREA*CARD_SECTORSIZE);
    dcache_flush(sysarea,CARD_SYSAREA*CARD_SECTORSIZE);

    memcpy(&dirblock[0],sysarea+0x2000,sizeof(CARD_DirBlock));
    memcpy(&dirblock[1],sysarea+0x4000,sizeof(CARD_DirBlock));
    dcache_flush(dirblock,sizeof(CARD_DirBlock)*2);

    memcpy(&fatblock[0],sysarea+0x6000,sizeof(CARD_FatBlock));
    memcpy(&fatblock[1],sysarea+0x8000,sizeof(CARD_FatBlock));
    dcache_flush(fatblock,sizeof(CARD_FatBlock)*2);

    ret = CARD_Check(channel,fixed);
    if (ret != CARD_ERROR_NONE) return ret;

    if (*fixed & 1) CARD_UpdateDir(channel);
    if (*fixed & 2) CARD_UpdateFat(channel);

    card_init = 1;
    curslot = channel;
    numsectors = (id & 0xFF) << 4;

    return CARD_ERROR_NONE;
}

int CARD_CreateFile(CARD_File *cfile,CARD_Offsets *offsets)
{
    CARD_DirEntry *entry;
    unsigned short block,prevblock,curblock;
    int i,count,filenum = -1;

    cfile->filenum = -1;
    if (card_init != 1) return CARD_ERROR_INIT;

    if (strlen(cfile->filename) > CARD_FILENAME || strlen(cfile->gamecode) != 4 || strlen(cfile->company) != 2) return CARD_ERROR_FATAL;
    if (cfile->size == 0 || cfile->size % CARD_SECTORSIZE) return CARD_ERROR_FATAL;
    if ((fatblock[curfat].freeblocks * CARD_SECTORSIZE) < cfile->size) return CARD_ERROR_NOSPACE;

    entry = dirblock[curdir].entries;
    for (i = 0;i < CARD_MAXFILES;i++)
    {
        if (entry[i].gamecode[0] == 0xff)
        {
            if (filenum == -1) filenum = i;
        } else {
            if (memcmp(entry[i].gamecode,cfile->gamecode,4) == 0 && memcmp(entry[i].company,cfile->company,2) == 0 && memcmp(entry[i].filename,cfile->filename,CARD_FILENAME) == 0) return CARD_ERROR_EXISTS;
        }
    }

    if (filenum == -1) return CARD_ERROR_NOENTRY;

    block = 0xffff;
    curblock = fatblock[curfat].last_alloc;
    i = cfile->size / CARD_SECTORSIZE;
    count = 0;

    for (;;)
    {
        if (i == 0)
        {
            fatblock[curfat].freeblocks -= cfile->size / CARD_SECTORSIZE;
            fatblock[curfat].last_alloc = curblock;
            CARD_UpdateFat(curslot);
            break;
        }
        count++;
        if (count >= (numsectors - CARD_SYSAREA)) return CARD_ERROR_CORRUPT;
        curblock++;

        if (curblock < 5 || curblock >= numsectors) curblock = 5;

        if (fatblock[curfat].fat[curblock-5] == 0)
        {
            if (block != 0xffff) fatblock[curfat].fat[prevblock-5] = curblock;
            else block = curblock;
            fatblock[curfat].fat[curblock-5] = 0xffff;
            prevblock = curblock;
            i--;
        }
    }
    entry = &dirblock[curdir].entries[filenum];
    memcpy(entry->filename,cfile->filename,CARD_FILENAME);
    memcpy(entry->gamecode,cfile->gamecode,4);
    memcpy(entry->company,cfile->company,2);
    entry->padding_ff = 0xff;
    entry->banner_fmt = cfile->banner_fmt;
    entry->time = gettime_s() / (*(volatile unsigned long *)(0x800000f8) / 4);
    entry->icon_addr = cfile->icon_addr;
    entry->icon_fmt = cfile->icon_fmt;
    entry->icon_speed = cfile->icon_speed;
    entry->attrib = cfile->attrib;
    if (entry->attrib == 0) entry->attrib = CARD_ATTRIB_PUBLIC;
    entry->padding_00 = 0;
    entry->block = block;
    entry->length = cfile->size / CARD_SECTORSIZE;
    entry->padding_ffff = 0xffff;
    entry->comment_addr = cfile->comment_addr;
    cfile->filenum = filenum;
    cfile->time = entry->time;
    CARD_UpdateDir(curslot);
    CARD_GetOffsets(cfile,offsets);
    return CARD_ERROR_NONE;
}

int CARD_OpenFile(CARD_File *cfile,CARD_Offsets *offsets)
{
    CARD_DirEntry *entry;
    int i;

    cfile->filenum = -1;
    if (card_init != 1) return CARD_ERROR_INIT;
    if (strlen(cfile->filename) > CARD_FILENAME || strlen(cfile->gamecode) != 4 || strlen(cfile->company) != 2) return CARD_ERROR_FATAL;

    entry = dirblock[curdir].entries;
    for (i = 0;i < CARD_MAXFILES;i++)
    {
        if (entry[i].gamecode[0] != 0xff)
        {
            if (memcmp(entry[i].gamecode,cfile->gamecode,4) == 0 && memcmp(entry[i].company,cfile->company,2) == 0 && memcmp(entry[i].filename,cfile->filename,CARD_FILENAME) == 0) break;
        }
    }
    if (i >= CARD_MAXFILES) return CARD_ERROR_NOENTRY;

    memcpy(cfile->filename,entry[i].filename,CARD_FILENAME);
    memcpy(cfile->gamecode,entry[i].gamecode,4);
    memcpy(cfile->company,entry[i].company,2);
    cfile->filename[32] = '\0';
    cfile->gamecode[4] = '\0';
    cfile->company[2] = '\0';
    cfile->banner_fmt = entry[i].banner_fmt;
    cfile->time = entry[i].time;
    cfile->icon_addr = entry[i].icon_addr;
    cfile->icon_fmt = entry[i].icon_fmt;
    cfile->icon_speed = entry[i].icon_speed;
    cfile->attrib = entry[i].attrib;
    cfile->comment_addr = entry[i].comment_addr;
    cfile->size = entry[i].length * CARD_SECTORSIZE;
    cfile->filenum = i;
    CARD_GetOffsets(cfile,offsets);
    return CARD_ERROR_NONE;
}

int CARD_OpenFilenum(CARD_File *cfile,CARD_Offsets *offsets,int filenum)
{
    CARD_DirEntry *entry;
    if (card_init != 1) return CARD_ERROR_INIT;
    if (filenum < 0 || filenum >= CARD_MAXFILES) return CARD_ERROR_FATAL;
    entry = &dirblock[curdir].entries[filenum];
    memcpy(cfile->gamecode,entry->gamecode,4);
    memcpy(cfile->company,entry->company,2);
    memcpy(cfile->filename,entry->filename,CARD_FILENAME);
    cfile->gamecode[4] = '\0';
    cfile->company[2] = '\0';
    cfile->filename[CARD_FILENAME] = '\0';
    return CARD_OpenFile(cfile,offsets);
}

int CARD_WriteFile(CARD_File *cfile,void *buf)
{
    CARD_DirEntry *entry;
    unsigned short block;
    int i;
    if (card_init != 1) return CARD_ERROR_INIT;
    if (cfile->filenum < 0 || cfile->filenum >= CARD_MAXFILES) return CARD_ERROR_FATAL;
    entry = &dirblock[curdir].entries[cfile->filenum];
    block = entry->block;
    dcache_flush(buf,cfile->size);
    for (i = 0;i < entry->length;i++)
    {
        if (block < CARD_SYSAREA || block >= numsectors) return CARD_ERROR_CORRUPT;
        CARD_SectorErase(curslot,block * CARD_SECTORSIZE);
        CARD_SectorProgram(curslot,buf + i * CARD_SECTORSIZE,block * CARD_SECTORSIZE,CARD_SECTORSIZE);
        block = fatblock[curfat].fat[block - 5];
    }
    memcpy(entry->filename,cfile->filename,CARD_FILENAME);
    memcpy(entry->gamecode,cfile->gamecode,4);
    memcpy(entry->company,cfile->company,2);
    entry->banner_fmt = cfile->banner_fmt;
    entry->time = *(volatile unsigned long*)(0x800000F8) / 4;
    entry->time = gettime_s() / entry->time;
    entry->icon_addr = cfile->icon_addr;
    entry->icon_fmt = cfile->icon_fmt;
    entry->icon_speed = cfile->icon_speed;
    entry->attrib = cfile->attrib;
    entry->comment_addr = cfile->comment_addr;
    cfile->time = entry->time;
    CARD_UpdateDir(curslot);
    return CARD_ERROR_NONE;
}
        
int CARD_ReadFile(CARD_File *cfile,void *buf)
{
    CARD_DirEntry *entry;
    unsigned short block;
    int i;
    if (card_init != 1) return CARD_ERROR_INIT;
    if (cfile->filenum < 0 || cfile->filenum >= CARD_MAXFILES) return CARD_ERROR_FATAL;
    entry = &dirblock[curdir].entries[cfile->filenum];
    block = entry->block;
    dcache_inv(buf,cfile->size);
    for (i = 0;i < entry->length;i++)
    {
        if (block < CARD_SYSAREA || block >= numsectors) return CARD_ERROR_CORRUPT;
        CARD_ReadArray(curslot,buf+i*CARD_SECTORSIZE,block*CARD_SECTORSIZE,CARD_SECTORSIZE);
        block = fatblock[curfat].fat[block-5];
    }
    dcache_flush(buf,cfile->size);
    return CARD_ERROR_NONE;
}

int CARD_ReadFirstBlock(CARD_File *cfile,void *buf)
{
    CARD_DirEntry *entry;
    unsigned short block;
    if (card_init != 1) return CARD_ERROR_INIT;
    if (cfile->filenum < 0 || cfile->filenum >= CARD_MAXFILES) return CARD_ERROR_FATAL;
    entry = &dirblock[curdir].entries[cfile->filenum];
    block = entry->block;
    dcache_inv(buf,cfile->size);
    if (block < CARD_SYSAREA || block >= numsectors) return CARD_ERROR_CORRUPT;
    CARD_ReadArray(curslot,buf,block*CARD_SECTORSIZE,CARD_SECTORSIZE);
    dcache_flush(buf,cfile->size);
    return CARD_ERROR_NONE;
}

int CARD_DeleteFile(CARD_File *cfile)
{
    unsigned short block,prevblock;
    if (card_init != 1) return CARD_ERROR_INIT;
    if (cfile->filenum < 0 || cfile->filenum >= CARD_MAXFILES) return CARD_ERROR_FATAL;
    block = dirblock[curdir].entries[cfile->filenum].block;
    while (1)
    {
        if (block == 0xFFFF) break;
        if (block < CARD_SYSAREA || block >= numsectors) return CARD_ERROR_CORRUPT;
        prevblock = block;
        block = fatblock[curfat].fat[prevblock-5];
        fatblock[curfat].fat[prevblock-5] = 0;
        fatblock[curfat].freeblocks++;
    }
    CARD_UpdateFat(curslot);
    memset(&dirblock[curdir].entries[cfile->filenum],0xFF,sizeof(CARD_DirEntry));
    CARD_UpdateDir(curslot);
    cfile->filenum = -1;
    return CARD_ERROR_NONE;
}

int CARD_FormatCard(unsigned long slot)
{
    if (!CARD_IsPresent(slot)) return 0;
    char* system = malloc(0x2000);
    int size = (exi_deviceid(slot,0) & 0xFF); // size in mbits 

    memset(&system[0x0000],0x00,0x000C); // unknown 
    *(unsigned long long*)(&system[0x000C]) = gettime_s(); // time 
    *(u16*)(&system[0x0014]) = CARD_RetrieveID(slot); // unique card ID 
    memset(&system[0x0016],0x00,0x000C); // unknown + padding 
    *(u16*)(&system[0x0022]) = size; // size in mbits 
    *(u16*)(&system[0x0024]) = 0; // text encoding 
    memset(&system[0x0026],0xFF,0x01D4); // unused 
    *(u16*)(&system[0x01FA]) = 0; // update counter 
    CARD_Checksum((u16*)system,0x01FC,(u16*)(&system[0x01FC]),(u16*)(&system[0x01FE])); // checksums 
    memset(&system[0x0200],0xFF,0x1E00); // unused 
    CARD_SectorProgram(slot,system,0x0000,0x2000); // write system sector! 

    memset(system,0xFF,0x2000); // clear dirblock 
    *(u16*)(&system[0x1FFA]) = 0; // update counter 
    CARD_Checksum((u16*)system,0x1FFC,(u16*)(&system[0x1FFC]),(u16*)(&system[0x1FFE])); // checksums 
    CARD_SectorProgram(slot,system,0x2000,0x2000); // write dir block 1! 
    CARD_SectorProgram(slot,system,0x4000,0x2000); // write dir block 2! 

    *(u16*)(&system[0x0004]) = 0; // update counter 
    *(u16*)(&system[0x0006]) = (size << 4) - 5; // free blocks 
    *(u16*)(&system[0x0008]) = 5; // last allocated block 
    memset(&system[0x000A],0x00,0x1FF6); // clear fatblock 
    CARD_Checksum((u16*)(&system[0x0004]),0x1FFC,(u16*)(&system[0x0000]),(u16*)(&system[0x0002]));
    CARD_SectorProgram(slot,system,0x6000,0x2000); // write fat block 1! 
    CARD_SectorProgram(slot,system,0x8000,0x2000); // write fat block 2! 

    free(system);
    return (size << 17); // size in bytes 
}

////////////////////////////////////////////////////////////////////////////////

void CARD_SetIcon(CARD_File *cfile,int icon,int fmt,int speed)
{
    cfile->icon_fmt &= ~(3 << (icon * 2));
    cfile->icon_fmt |= (fmt << (icon * 2));
    cfile->icon_speed &= ~(3 << (icon * 2));
    cfile->icon_speed |= (speed << (icon * 2));
}

void CARD_SetIconAnim(CARD_File *cfile,int anim)
{
    cfile->banner_fmt &= ~4;
    cfile->banner_fmt |= anim;
}

int CARD_CalcSize(CARD_File *cfile,int datalen)
{
    CARD_Offsets offsets;
    int ret;
    ret = CARD_GetOffsets(cfile,&offsets);
    if (ret != CARD_ERROR_NONE) return ret;
    cfile->size = offsets.data + datalen;
    cfile->size += (CARD_SECTORSIZE - (cfile->size % CARD_SECTORSIZE));
    return CARD_ERROR_NONE;
}

int CARD_FreeSpace()
{
    if (card_init != 1) return CARD_ERROR_INIT;
    return fatblock[curfat].freeblocks * CARD_SECTORSIZE;
}

unsigned long CARD_ConvColor(unsigned short color)
{
    unsigned char r,g,b;
    if (color & 0x8000)
    {
        r =(color >> 7) & 0xf8;
        g =(color >> 2) & 0xf8;
        b =(color << 3) & 0xf8;
    } else {
        r =(color >> 4) & 0xf0;
        g = color & 0xf0;
        b =(color << 4) & 0xf0;
    }
    return FS_VIDEO_RGBToYCbYCr(r,g,b);
}

void CARD_TileRGB(void *buf,unsigned long *fb,int x,int y,int tilenum)
{
    unsigned char cb,cr;
    unsigned short *ptr;
    unsigned long clr1,clr2;
    int i,j;
    ptr = buf + tilenum * 32;
    for (i = y;i < y + 4;i++)
    {
        for (j = x;j < x + 2;j++)
        {
            clr1 = CARD_ConvColor(*ptr++);
            clr2 = CARD_ConvColor(*ptr++);
            cb = (((clr1 >> 16) & 0xFF) + ((clr2 >> 16) & 0xff)) / 2;
            cr = ((clr1 & 0xff) + (clr2 & 0xFF)) / 2;
            fb[i * 320 + j] = (clr1 & 0xff000000) | (cb << 16) | (clr2 & 0xff00) | cr;
        }
    }
}

void CARD_TileC8(void *buf,unsigned short *tlut,unsigned long *fb,int x,int y,int tilenum)
{
    unsigned char *ptr,cb,cr;
    unsigned long clr1,clr2;
    int i,j;
    ptr = buf + tilenum * 32;
    for (i = y;i < y + 4;i++)
    {
        for (j = x;j < x + 4;j++)
        {
            clr1 = CARD_ConvColor(tlut[*ptr++]);
            clr2 = CARD_ConvColor(tlut[*ptr++]);
            cb =(((clr1 >> 16) & 0xff) +((clr2 >> 16) & 0xff)) / 2;
            cr =((clr1 & 0xff) +(clr2 & 0xff)) / 2;
            fb[i * 320 + j] = (clr1 & 0xff000000) | (cb << 16) | (clr2 & 0xff00) | cr;
        }
    }
}
    
int CARD_DrawIcon(CARD_File *cfile,CARD_Offsets *offsets,int icon,void *buf,unsigned long *fb,int x,int y)
{
    void *ptr;
    unsigned short *tlut;
    int i,j,k,w,h,tilew,tileh,format;

    //if (card_init != 1) return CARD_ERROR_INIT;
    if (cfile->filenum < 0 || cfile->filenum >= CARD_MAXFILES) return CARD_ERROR_FATAL;
    if (icon < -1 || icon >= CARD_MAXICONS) return CARD_ERROR_FATAL;

    if (icon == -1)
    {
        w = CARD_BANNER_W;
        h = CARD_BANNER_H;
        ptr = buf + offsets->banner;
        if (cfile->banner_fmt & CARD_BANNER_CI) format = CARD_ICON_CI;
        else if (cfile->banner_fmt & CARD_BANNER_RGB) format = CARD_ICON_RGB;
        else format = CARD_ICON_NONE;
    } else {
        w = CARD_ICON_W;
        h = CARD_ICON_H;
        ptr = buf + offsets->icons[icon];
        format = (cfile->icon_fmt >> (icon * 2)) & 3;
    }

    if (format == CARD_ICON_NONE) return CARD_ERROR_NOENTRY;
    if (format == CARD_ICON_CI)
    {
        tilew = 8;
        tileh = 4;
        if (icon == -1) tlut = buf + offsets->banner_tlut;
        else tlut = buf + offsets->icons_tlut;
    } else {
        tilew = 4;
        tileh = 4;
    }

    k = 0;
    for (i = 0;i < h;i += tileh)
    {
        for (j = 0;j < w;j += tilew)
        {
            if (format == CARD_ICON_CI) CARD_TileC8(ptr,tlut,fb,x + (j / 2),y + i,k++);
            else CARD_TileRGB(ptr,fb,x + (j / 2),y + i,k++);
        }
    }

    return CARD_ERROR_NONE;
}
