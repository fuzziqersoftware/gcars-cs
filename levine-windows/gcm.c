#include <windows.h>
#include <stdio.h>
#include "gamecube.h"
#include "fst.h"
#include "gcm.h"

// fst.bin     : reconstructed by Levine 
// appldr.bin  : must be sent (resident at 0x2440 on disk) 
// default.dol : must be sent (resident at $dolfileoffset on disk) 

u32 _dolsize(u32* buffer)
{
    u32 x,latest = 0;
    for (x = 0; x < 18; x++) if (byteswap(buffer[x]) > byteswap(buffer[latest])) latest = x;
    return (byteswap(buffer[latest]) + byteswap(buffer[latest + 36]));
}

GCM_IMAGE* gcm_init(char* name)
{
    u32 buffer[0x40];
    u32 bytesread;
    FST_FILE file;
    GCM_IMAGE* gcm = (GCM_IMAGE*)malloc(sizeof(GCM_IMAGE));
    ZeroMemory(gcm,sizeof(GCM_IMAGE));

    gcm->file = CreateFile(name,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,0,NULL);
    if (gcm->file == INVALID_HANDLE_VALUE)
    {
        free(gcm);
        return NULL;
    }

    ReadFile(gcm->file,&gcm->gcmheader,sizeof(GCM_HEADER),&bytesread,NULL);
    ReadFile(gcm->file,&gcm->apploaderheader,sizeof(APPLOADER_HEADER),&bytesread,NULL);

    void* fstbuf = malloc(byteswap(gcm->gcmheader.fstsize));
    SetFilePointer(gcm->file,byteswap(gcm->gcmheader.fstoffset),NULL,FILE_BEGIN);
    ReadFile(gcm->file,fstbuf,byteswap(gcm->gcmheader.fstsize),&bytesread,NULL);
    fst_parse_dir(&gcm->rootdir,fstbuf,byteswap(gcm->gcmheader.fstsize));
    free(fstbuf);

    SetFilePointer(gcm->file,byteswap(gcm->gcmheader.dolfileoffset),NULL,FILE_BEGIN);
    ReadFile(gcm->file,buffer,0x100,&bytesread,NULL);
    file.size   = _dolsize(buffer);
    file.offset = byteswap(gcm->gcmheader.dolfileoffset);
    file.name   = "disc-hs-startup.dol";
    fst_add_file(&gcm->rootdir,&file);

    file.size   = byteswap(gcm->apploaderheader.size);
    file.offset = 0x00002440;
    file.name   = "disc-hs-startup.bin";
    fst_add_file(&gcm->rootdir,&file);

    return gcm;
}

void gcm_exit(GCM_IMAGE* image)
{
    fst_delete_dir(&image->rootdir);
    CloseHandle(image->file);
    free(image);
}

////////////////////////////////////////////////////////////////////////////////

GCM_FILE* gcm_open_file(GCM_IMAGE* image,char* name)
{
    char buffer[100];
    FST_DIR* dir = &image->rootdir;
    FST_FILE* file;
    GCM_FILE* gfile;
    u32 x,y,num,fname = 0;

    for (x = 0; x < strlen(name); x++)
    {
        if (name[x] == '\\')
        {
            ZeroMemory(buffer,100);
            for (y = 0; y < x - fname; y++) buffer[y] = name[fname + y];
            fname = x + 1;
            num = dir->numDirs;
            for (y = 0; y < num; y++)
            {
                if (!strcmp(dir->dirs[y].name,buffer))
                {
                    dir = &dir->dirs[y];
                    break;
                }
            }
            if (y == num) return NULL;
        }
    }

    for (y = 0; y < dir->numFiles; y++)
    {
        if (!strcmp(dir->files[y].name,&name[fname]))
        {
            file = &dir->files[y];
            break;
        }
    }
    if (y == dir->numFiles) return NULL;

    gfile = (GCM_FILE*)malloc(sizeof(GCM_FILE));
    gfile->image = image;
    gfile->file = image->file;
    strcpy(gfile->filename,&name[fname]);
    gfile->offset = file->offset;
    gfile->position = 0;
    gfile->size = file->size;

    return gfile;
}

void gcm_seek_file(GCM_FILE* file,s32 value,u32 mode)
{
    switch (mode)
    {
      case GCM_SEEK_BEGIN:
        file->position = value;
        break;
      case GCM_SEEK_CURR:
        file->position += value;
        break;
      case GCM_SEEK_END:
        file->position = file->size - value;
        break;
    }
}

void gcm_read_file(GCM_FILE* file,void* data,u32 size)
{
    u32 bytesread;
    SetFilePointer(file->file,file->offset + file->position,NULL,FILE_BEGIN);
    ReadFile(file->file,data,size,&bytesread,NULL);
    file->position += size;
}

void gcm_write_file(GCM_FILE* file,void* data,u32 size)
{
    u32 bytesread;
    SetFilePointer(file->file,file->offset + file->position,NULL,FILE_BEGIN);
    WriteFile(file->file,data,size,&bytesread,NULL);
    file->position += size;
}

void gcm_close_file(GCM_FILE* file) { free(file); }

