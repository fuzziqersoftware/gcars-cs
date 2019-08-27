#include <windows.h>
#include "gamecube.h"

DWORD byteswap(DWORD x)
{
    BYTE* a = (BYTE*)(&x);
    return ((a[0] << 24) | (a[1] << 16) | (a[2] << 8) | (a[3]));
}

WORD byteswaps(WORD x)
{
    BYTE* a = (BYTE*)(&x);
    return ((a[0] << 8) | a[1]);
}

u32 checksum(void* data,u32 size)
{
    u32 x,cs = 0;
    for (x = 0; x < size; x += 4) cs ^= *(u32*)((u32)data + x);
    return cs;
}

char FilterString_GameCube(char* s)
{
    char* abc = " _!\"#$\%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[µ]°°°ABCDEFGHIJKLMNOPQRSTUVWXYZ+|";
    DWORD xl = strlen(s);
    DWORD yl = strlen(abc);
    DWORD x,y;
    for (x = 0; x < xl; x++)
    {
        for (y = 0; y < yl; y++)
        {
            if (s[x] == abc[y]) break;
        }
        if (y == yl) return s[x];
    }
    return 0;
}

char FilterString_GameID(char* s)
{
    char* abc = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    DWORD xl = strlen(s);
    DWORD yl = strlen(abc);
    DWORD x,y;
    for (x = 0; x < xl; x++)
    {
        for (y = 0; y < yl; y++)
        {
            if (s[x] == abc[y]) break;
        }
        if (y == yl) return s[x];
    }
    return 0;
}

char FilterString_Hex(char* s)
{
    char* abc = "0123456789abcdefABCDEF ";
    DWORD xl = strlen(s);
    DWORD yl = strlen(abc);
    DWORD x,y;
    for (x = 0; x < xl; x++)
    {
        for (y = 0; y < yl; y++)
        {
            if (s[x] == abc[y]) break;
        }
        if (y == yl) return s[x];
    }
    return 0;
}

char FilterString_IP(char* s)
{
    char* abc = ".0123456789";
    DWORD xl = strlen(s);
    DWORD yl = strlen(abc);
    DWORD x,y;
    for (x = 0; x < xl; x++)
    {
        for (y = 0; y < yl; y++)
        {
            if (s[x] == abc[y]) break;
        }
        if (y == yl) return s[x];
    }
    return 0;
}
