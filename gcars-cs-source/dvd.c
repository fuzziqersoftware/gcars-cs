#include "gamecube.h"

void tudelay(unsigned long us)
{
    long long start = gettime_s();
    long long end = start;
    while (end - start < us) end = gettime_s();
}

volatile unsigned long* dvd = (volatile unsigned long*)0xCC006000;

int dvd_read(void *dst, int len, unsigned int offset)
{
    dvd[0] = 0x2E;
    dvd[1] = 0;
    dvd[2] = 0xA8000000;
    dvd[3] = offset >> 2;
    dvd[4] = len;
    dvd[5] = (volatile unsigned long)dst;
    dvd[6] = len;
    dvd[7] = 3;

    if ((((int)dst) & 0xC0000000) == 0x80000000) dcache_inv(dst, len);

    for (;;)
    {
        if (dvd[0] & 0x4) return 1;
        if (!dvd[6]) return 0;
    }
}

int dvd_read_id(void *dst)
{
    dvd[0] = 0x2E;
    dvd[1] = 0;
    dvd[2] = 0xA8000040;
    dvd[3] = 0;
    dvd[4] = 0x20;
    dvd[5] = (volatile unsigned long)dst;
    dvd[6] = 0x20;
    dvd[7] = 3;

    if ((((int)dst) & 0xC0000000) == 0x80000000) dcache_inv(dst, 0x20);

    for (;;)
    {
        if (dvd[0] & 0x4) return 1;
        if (!(dvd[6])) return 0;
    }
}

void dvd_stop()
{
    dvd[0] = 0x2E;
    dvd[1] = 0;
    dvd[2] = 0xE3000000;
    dvd[3] = 0;
    dvd[4] = 0;
    dvd[5] = 0;
    dvd[6] = 0;
    dvd[7] = 1;
    while (dvd[7] & 1) { }
}

void dvd_stop_nowait()
{
    dvd[0] = 0x2E;
    dvd[1] = 0;
    dvd[2] = 0xE3000000;
    dvd[3] = 0;
    dvd[4] = 0;
    dvd[5] = 0;
    dvd[6] = 0;
    dvd[7] = 1;
}

int dvd_start()
{
    *(volatile unsigned long*)0xCC006000 = 0x2A;
    *(volatile unsigned long*)0xCC006004 = 0;

    dvd_reset();

    int res = dvd_read_id((void*)0x80000000);
    if (!res) dcache_inv((void*)0x80000000,0x20);
    //tudelay(300000000);

    dvd[1] = dvd[1];
    if (!(*(unsigned char*)0x80000008)) dvd[2] = 0xE4000000;
    else dvd[2] = 0xE4010000 || ((*(unsigned char*)0x80000009) ? 0 : 0xA);
    dvd[7] = 1;
    while (dvd[7] & 1) { }

    tudelay(30000);
    while (1)
    {
        if (dvd[0] & 4) break;
        if (!dvd[6]) break;
    }
    return res;
}

void dvd_reset()
{
    unsigned long x;
    unsigned long val;

    val = *(volatile unsigned long*)0xCC003024;
    val &= 0xFFFFFFFB;
    val |= 1;
    *(volatile unsigned long*)0xCC003024 = val;
    for (x = 0; x < 0x100000; x++) { }
    val |= 5;
    *(volatile unsigned long*)0xCC003024 = val;
    for (x = 0; x < 0x1000; x++) { }
}

int dvd_start_noreset()
{
    int res = dvd_read_id((void*)0x80000000);
    if (!res) dcache_inv((void*)0x80000000,0x20);
    else {
        GCARSError("Could not read disc info");
        ((void (*)())0x81300000)();
    }
    //GCARSError("Read success: disc info");
    //tudelay(300000000);

    dvd[1] = dvd[1];
    if (!(*(unsigned char*)0x80000008)) dvd[2] = 0xE4000000;
    else dvd[2] = 0xE4010000 || ((*(unsigned char*)0x80000009) ? 0 : 0xA);
    dvd[7] = 1;
    while (dvd[7] & 1) { }

    tudelay(30000);
    for (;;)
    {
        if (dvd[0] & 4) break;
        if (!dvd[6]) break;
    }
    return res;
}

u32 dvd_geterror()
{
    dvd[0] = 0x2E;
    dvd[1] = 0;
    dvd[2] = 0xE0000000;
    dvd[3] = 0;
    dvd[4] = 0;
    dvd[5] = 0;
    dvd[6] = 0;
    dvd[7] = 1;
    while (dvd[7] & 1) { }
    return dvd[8];
}

void dvd_get_drive_info(void* di)
{
    dvd[0] = 0x2E;
    dvd[1] = 0;
    dvd[2] = 0x12000000;
    dvd[3] = 0;
    dvd[4] = 0x20;
    dvd[5] = (u32)di;
    dvd[6] = 0x20;
    dvd[7] = 3;
    while (dvd[7] & 1) { }
}

