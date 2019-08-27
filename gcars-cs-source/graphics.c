#define FUZZIQER_TYPES
#include "types.h"
#include "graphics.h"

u32* fb;

void GRAPH_SetFramebuffer(u32* frames) { fb = frames; }

void GRAPH_Rectangle(u32 x,u32 y,u32 w,u32 h,u32 color)
{
    u32 a,b;
    u32 right,bottom;
    right = x + w;
    bottom = y + h;

    for (b = y; b < bottom; b++)
    {
        for (a = x; a < right; a++)
        {
            fb[(b * 320) + a] = color;
        }
    }
}

