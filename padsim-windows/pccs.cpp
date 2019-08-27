#include <windows.h>
#include <stdio.h>
#include "pccs.h"

DWORD byteswap(DWORD e) { return (((e >> 24) & 0xFF) | (((e >> 16) & 0xFF) << 8) | (((e >> 8) & 0xFF) << 16) | ((e & 0xFF) << 24)); }
WORD byteswap(WORD e) { return (((e >> 8) & 0xFF) | ((e & 0xFF) << 8)); }

u32 CSChecksum(CSPacket* csp)
{
    u32 x,value = 0;
    value = byteswap(csp->gcGameID);
    value ^= byteswap(csp->size);
    value ^= byteswap(csp->padspresent);
    value ^= byteswap(csp->numCommands);
    for (x = ((u32)&csp + sizeof(CSPacket)); x < byteswap(csp->size); x += 4) value ^= byteswap(*(u32*)(x));
    return byteswap(value);
}

void PAD_ReadState(CSPADStatus* pad)
{
    ZeroMemory(pad,sizeof(CSPADStatus));

    if (GetAsyncKeyState(VK_RETURN)   & 0x8000) pad->buttons |= 0x1000;
    if (GetAsyncKeyState(VK_LMENU)    & 0x8000) pad->buttons |= 0x0800;
    if (GetAsyncKeyState(VK_LCONTROL) & 0x8000) pad->buttons |= 0x0400;
    if (GetAsyncKeyState(VK_TAB)      & 0x8000) pad->buttons |= 0x0200;
    if (GetAsyncKeyState(VK_SPACE)    & 0x8000) pad->buttons |= 0x0100;
    if (GetAsyncKeyState(VK_NUMPAD0)  & 0x8000) pad->buttons |= 0x0040;
    if (GetAsyncKeyState(VK_DECIMAL)  & 0x8000) pad->buttons |= 0x0020;
    if (GetAsyncKeyState(VK_LSHIFT)   & 0x8000) pad->buttons |= 0x0010;
    if (GetAsyncKeyState('I')         & 0x8000) pad->buttons |= 0x0008;
    if (GetAsyncKeyState('K')         & 0x8000) pad->buttons |= 0x0004;
    if (GetAsyncKeyState('L')         & 0x8000) pad->buttons |= 0x0002;
    if (GetAsyncKeyState('J')         & 0x8000) pad->buttons |= 0x0001;
    pad->buttons = byteswap(pad->buttons);

    if (GetAsyncKeyState(VK_LEFT)     & 0x8000) pad->controlX = 0x90;
    if (GetAsyncKeyState(VK_RIGHT)    & 0x8000) pad->controlX = 0x70;
    if (GetAsyncKeyState(VK_UP)       & 0x8000) pad->controlY = 0x70;
    if (GetAsyncKeyState(VK_DOWN)     & 0x8000) pad->controlY = 0x90;

    if (GetAsyncKeyState(VK_NUMPAD4)  & 0x8000) pad->cstickX = 0x90;
    if (GetAsyncKeyState(VK_NUMPAD6)  & 0x8000) pad->cstickX = 0x70;
    if (GetAsyncKeyState(VK_NUMPAD8)  & 0x8000) pad->cstickY = 0x70;
    if (GetAsyncKeyState(VK_NUMPAD2)  & 0x8000) pad->cstickY = 0x90;
}

