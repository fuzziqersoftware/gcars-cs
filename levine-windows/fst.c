#include <windows.h>
#include <stdio.h>
#include "gamecube.h"
#include "fst.h"

void* fst_alloc(u32 size) { return malloc(size); }
void fst_free(void* vp) { free(vp); }

FST_FILE* fst_add_file(FST_DIR* dir,FST_FILE* file)
{
    FST_FILE* t = (FST_FILE*)fst_alloc(sizeof(FST_FILE) * (dir->numFiles + 1));
    if (dir->files != NULL)
    {
        memcpy(t,dir->files,sizeof(FST_FILE) * dir->numFiles);
        fst_free(dir->files);
    }
    memcpy(&t[dir->numFiles],file,sizeof(FST_FILE));
    dir->files = t;
    dir->numFiles++;
    return &dir->files[dir->numFiles - 1];
}

FST_DIR* fst_add_dir(FST_DIR* dir,FST_DIR* add)
{
    u32 x;
    FST_DIR* t = (FST_DIR*)fst_alloc(sizeof(FST_DIR) * (dir->numDirs + 1));
    if (dir->dirs != NULL)
    {
        memcpy(t,dir->dirs,sizeof(FST_DIR) * dir->numDirs);
        fst_free(dir->dirs);
    }
    memcpy(&t[dir->numDirs],add,sizeof(FST_DIR));
    dir->dirs = t;
    dir->numDirs++;
    return &dir->dirs[dir->numDirs - 1];
}

void fst_remove_file(FST_DIR* dir,FST_FILE* rem)
{
    u32 x;
    for (x = 0; x < dir->numFiles; x++)
    {
        if (&dir->files[x] == rem) break;
    }
    if (x == dir->numFiles) return;

    if (dir->numFiles == 1)
    {
        fst_free(dir->files);
        return;
    }

    FST_FILE* t = (FST_FILE*)fst_alloc(sizeof(FST_FILE) * (dir->numFiles - 1));
    memcpy(&t[0],&dir->files[0],sizeof(FST_FILE) * x);
    memcpy(&t[x],&dir->files[x + 1],sizeof(FST_FILE) * ((dir->numFiles - x) - 1));
    fst_free(dir->files);
    dir->files = t;
    dir->numFiles--;
}

void fst_remove_dir(FST_DIR* dir,FST_DIR* rem)
{
    u32 x;
    for (x = 0; x < dir->numDirs; x++)
    {
        if (&dir->dirs[x] == rem) break;
    }
    if (x == dir->numDirs) return;

    if (dir->numDirs == 1)
    {
        fst_free(dir->dirs);
        return;
    }

    FST_DIR* t = (FST_DIR*)fst_alloc(sizeof(FST_DIR) * (dir->numDirs - 1));
    memcpy(&t[0],&dir->dirs[0],sizeof(FST_DIR) * x);
    memcpy(&t[x],&dir->dirs[x + 1],sizeof(FST_DIR) * ((dir->numDirs - x) - 1));
    fst_free(dir->dirs);
    dir->dirs = t;
    dir->numDirs--;
}

u32 fst_count_objects(FST_DIR* root)
{
    u32 x,total = root->numFiles + root->numDirs;
    for (x = 0; x < root->numDirs; x++) total += fst_count_objects(root);
    return total;
}

u32 fst_total_strlen(FST_DIR* root)
{
    u32 x,total = 0;
    for (x = 0; x < root->numFiles; x++) total += (strlen(root->files[x].name) + 1);
    for (x = 0; x < root->numDirs; x++)
    {
        total += (strlen(root->dirs[x].name) + 1);
        total += fst_total_strlen(root);
    }
    return total;
}

////////////////////////////////////////////////////////////////////////////////

int unparsedircount = 0,deletedircount = 0,parsedircount = 0,fstnum = 0,stringbase = 0,stringbase_o = 0;

void fst_parse_dir(FST_DIR* root,void* data,u32 size)
{
    parsedircount++;

    FST_DISC_ENTRY* fst = (FST_DISC_ENTRY*)data;
    u32 y,num,first;
    FST_FILE file;
    FST_DIR dir;

    if (stringbase == 0)
    {
        fstnum = first = 0;
        num = byteswap(fst[fstnum].size);
        y = size - (12 * num);
        root->name = (char*)malloc(y + 5);
        memcpy(&root->name[5],(void*)((12 * num) + (u32)data),y);
        strcpy(root->name,"root");
        stringbase = (u32)root->name + 5;
    } else {
        root->name = (char*)(stringbase + (byteswap(fst[fstnum].nameoffset) & 0x00FFFFFF));
        num = byteswap(fst[fstnum].size);
        first = fstnum;
    }
    printf("[%04X] [%d opendir:%s %04X]\n",fstnum,parsedircount,root->name,num);
    fstnum++;

    while (fstnum < num)
    {
        //Sleep(15);
        if (byteswap(fst[fstnum].nameoffset) & 0xFF000000)
        {
            ZeroMemory(&dir,sizeof(FST_DIR));
            fst_parse_dir(&dir,data,0);
            fst_add_dir(root,&dir);
        } else {
            printf("[%04X] %08X %08X %s\n",fstnum,byteswap(fst[fstnum].offset),byteswap(fst[fstnum].size),(char*)(stringbase + byteswap(fst[fstnum].nameoffset) & 0x00FFFFFF));
            ZeroMemory(&file,sizeof(FST_FILE));
            file.name = (char*)(stringbase + byteswap(fst[fstnum].nameoffset) & 0x00FFFFFF);
            file.offset = byteswap(fst[fstnum].offset);
            file.size = byteswap(fst[fstnum].size);
            fst_add_file(root,&file);
            fstnum++;
        }
    }
    printf("       [%d closedir:%s %04X]\n",parsedircount,root->name,num);
    parsedircount--;
    if (parsedircount == 0) stringbase = 0;
}

u32 fst_unparse_dir(FST_DIR* root,void* data,u32 parent)
{
    unparsedircount++;

    FST_DISC_ENTRY* fst = (FST_DISC_ENTRY*)data;
    u32 x,y,num,first;
    FST_FILE file;
    FST_DIR dir;
    char* name;

    if (parent == 0xFFFFFFFF)
    {
        parent = fstnum = first = 0;
        num = fst_count_objects(root) + 1;
        stringbase_o = stringbase = ((u32)data + (num * 12));
        fst[fstnum].nameoffset = byteswap(0x01000000);
        fst[fstnum].offset = byteswap(0x00000000);
        fst[fstnum].size = byteswap(num);
    } else {
        first = fstnum;
        num = byteswap(fst[fstnum].size);
        name = (char*)stringbase;
        strcpy(name,root->name);
        fst[fstnum].nameoffset = byteswap(0x01000000 | ((u32)stringbase - (u32)stringbase_o));
        fst[fstnum].offset = byteswap(parent);
        fst[fstnum].size = byteswap(parent + fst_count_objects(root) + 1);
        stringbase += (strlen(root->name) + 1);
    }
    printf("[%04X] [%d opendir:%s %04X]\n",fstnum,parsedircount,root->name,num);
    fstnum++;

    for (x = 0; x < root->numDirs; x++)
    {
        Sleep(15);
        fst_unparse_dir(&root->dirs[x],data,first);
    }
    for (x = 0; x < root->numFiles; x++)
    {
        Sleep(15);
        name = (char*)stringbase;
        strcpy(name,root->files[x].name);
        fst[fstnum].nameoffset = byteswap((u32)stringbase - (u32)stringbase_o);
        fst[fstnum].offset = byteswap(root->files[x].offset);
        fst[fstnum].size = byteswap(root->files[x].size);
        stringbase += (strlen(name) + 1);
        printf("[%04X] %08X %08X %08X %s\n",fstnum,byteswap(fst[fstnum].offset),byteswap(fst[fstnum].size),byteswap(fst[fstnum].nameoffset),name);
        fstnum++;
    }
    printf("       [%d closedir:%s %04X]\n",parsedircount,root->name,num);
    unparsedircount--;
    if (unparsedircount == 0)
    {
        x = (num * 12) + (stringbase - stringbase_o);
        stringbase = 0;
    } else x = 0;
    return x;
}

void fst_delete_dir(FST_DIR* del)
{
    u32 x;
    for (x = 0; x < del->numDirs; x++) fst_delete_dir(&del->dirs[x]);
    free(del->files);
    if (!strcmp(del->name,"root")) free(del->name);
}

FST_DIR* fst_up(FST_DIR* root,FST_DIR* src)
{
    u32 x;
    FST_DIR* dir;
    for (x = 0; x < root->numDirs; x++)
    {
        if (src == &root->dirs[x]) return root;
        dir = fst_up(&root->dirs[x],src);
        if (dir != NULL) return dir;
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////////

void fst_print_dir(FST_DIR* dir)
{
    u32 x;
    printf("[dir %08X:%s]\n",dir,dir->name);
    for (x = 0; x < dir->numDirs; x++) printf("  [sub  %s]\n",dir->dirs[x].name);
    for (x = 0; x < dir->numFiles; x++) printf("  [file %08X %08X %s]\n",dir->files[x].offset,dir->files[x].size,dir->files[x].name);
    for (x = 0; x < dir->numDirs; x++) fst_print_dir(&dir->dirs[x]);
}

