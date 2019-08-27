#include "gamecube.h"
#include "gcars.h"
#include "load-game.h"

void dvd_dumpregs()
{
    GCARSStatus("DVD regs:");
    DEBUG_ShowValueU32(66,170,*(u32*)0xCC006000);
    DEBUG_ShowValueU32(66,190,*(u32*)0xCC006004);
    DEBUG_ShowValueU32(66,210,*(u32*)0xCC006008);
    DEBUG_ShowValueU32(66,230,*(u32*)0xCC00600C);
    DEBUG_ShowValueU32(66,250,*(u32*)0xCC006010);
    DEBUG_ShowValueU32(66,270,*(u32*)0xCC006014);
    DEBUG_ShowValueU32(66,290,*(u32*)0xCC006018);
    DEBUG_ShowValueU32(66,310,*(u32*)0xCC00601C);
    DEBUG_ShowValueU32(66,330,*(u32*)0xCC006020);
    DEBUG_ShowValueU32(66,350,*(u32*)0xCC006024);
    DEBUG_ShowValueU32(66,370,dvd_geterror());
    debugDrawInfo();
    PAD pad;
    for (;;)
    {
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.B || pad.Digital.A)
        {
            FS_PAD_WaitEmptyAll();
            break;
        }
        FS_VIDEO_WaitVSync();
    }
}

void nothing() { }

void* LoadGame_Apploader()
{
    // variables
    int err;
    void *buffer = malloc(0x20);
    void (*app_init)(void (*report)(const char *fmt, ...));
    int (*app_main)(void **dst, int *size, int *offset);
    void *(*app_final)();
    void (*app_entry)(void(**init)(void (*report)(const char *fmt, ...)), int (**main)(void**,int*,int*), void *(**final)());

    // disable interrupts
    u32 msr;
    msr = GetMSR();
    msr &= ~0x8000;
    msr |= 0x2002;
    SetMSR(msr);

    // start disc drive & read apploader
    err = dvd_read(buffer,0x20,0x2440);
    if (err) GCARSError("Could not load apploader header");
    err = dvd_read((void*)0x81200000,((*(unsigned long*)((u32)buffer+0x14)) + 31) &~31,0x2460);
    if (err) GCARSError("Could not load apploader data");

    // run apploader
    app_entry = (void (*)(void(**)(void(*)(const char*,...)),int(**)(void**,int*,int*),void*(**)()))(*(unsigned long*)((u32)buffer + 0x10));
    free(buffer);
    app_entry(&app_init,&app_main,&app_final);
    app_init((void(*)(const char*,...))nothing);
    for (;;)
    {
        void *dst = 0;
        int len = 0,offset = 0;
        int res = app_main(&dst,&len,&offset);
        if (!res) break;
        err = dvd_read(dst,len,offset);
        if (err) GCARSError("Apploader read failed");
        flush_code(dst,len);
    }
    void* entrypoint = app_final();
    return entrypoint;
}

void* LoadGame()
{
    /*int err;
    PAD pad;
    u32 x,wait;
    bool anacondamode = false;
    u8 draw = 1;

closedrive:
    GCARSStatus("Open the disc drive");
    while (!(*((u32*)0xCC006004) & 1))
    {
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.L)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GCARSStatus("Running Anaconda04/Cobra03");
            return anacondaDriveSetup(true);
        }
        if (pad.Digital.R)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GCARSStatus("Running Anaconda04/Cobra04");
            return anacondaDriveSetup(false);
        }
        FS_VIDEO_WaitVSync();
    }

    GCARSStatus("Change disc and close the drive");
    while (*((u32*)0xCC006004) & 1) { }

    GCARSStatus("Starting the disc drive");
    err = dvd_start();
    if (err) GCARSError("Could not start the disc drive.");

    if (*((u32*)0xCC006004) & 1) goto closedrive; */
    LoadGame_SwapDisk();

    GCARSStatus("Loading game");
    return LoadGame_Apploader();
}

void LoadGame_SwapDisk()
{
    int err;
    PAD pad;
    u32 x,wait;
    u8 draw = 1;

closedrive:
    GCARSStatus("Open the disc drive");
    if (debugEnabled())
    {
        void* driveinfo = malloc(0x20);
        dvd_get_drive_info(driveinfo);
        DEBUG_ShowValueU32(66,270,*(u32*)((u32)driveinfo + 0x00));
        DEBUG_ShowValueU32(66,286,*(u32*)((u32)driveinfo + 0x04));
        DEBUG_ShowValueU32(66,302,*(u32*)((u32)driveinfo + 0x08));
        DEBUG_ShowValueU32(66,318,*(u32*)((u32)driveinfo + 0x0C));
        DEBUG_ShowValueU32(66,334,*(u32*)((u32)driveinfo + 0x10));
        DEBUG_ShowValueU32(66,350,*(u32*)((u32)driveinfo + 0x14));
        DEBUG_ShowValueU32(66,366,*(u32*)((u32)driveinfo + 0x18));
        DEBUG_ShowValueU32(66,382,*(u32*)((u32)driveinfo + 0x1C));
    }

    while (!(*((u32*)0xCC006004) & 1))
    {
        FS_PAD_ReadAllState(&pad);
        if (pad.Digital.L)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GCARSStatus("Running Anaconda04/Cobra03");
            anacondaDriveSetup(true);
            return;
        }
        if (pad.Digital.R)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GCARSStatus("Running Anaconda04/Cobra04");
            anacondaDriveSetup(false);
            return;
        }
        FS_VIDEO_WaitVSync();
    }

    GCARSStatus("Change disc and close the drive");
    while (*((u32*)0xCC006004) & 1) { }

    GCARSStatus("Starting the disc drive");
    err = dvd_start();
    if (err) GCARSError("Could not start the disc drive.");

    if (*((u32*)0xCC006004) & 1) goto closedrive;
}

