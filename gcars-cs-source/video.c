#include "gamecube.h"

static const u32 FS_VIDEO_Mode640X480NtscYUV16[32] = {
   0x0F060001, 0x476901AD, 0x02EA5140, 0x00030018,
   0x00020019, 0x410C410C, 0x40ED40ED, 0x00435A4E,
   0x00000000, 0x00435A4E, 0x00000000, 0x00000000,
   0x110701AE, 0x10010001, 0x00010001, 0x00010001,
   0x00000000, 0x00000000, 0x28500100, 0x1AE771F0,
   0x0DB4A574, 0x00C1188E, 0xC4C0CBE2, 0xFCECDECF,
   0x13130F08, 0x00080C0F, 0x00FF0000, 0x00000000,
   0x02800000, 0x000000FF, 0x00FF00FF, 0x00FF00FF};

static const u32 FS_VIDEO_Mode640X480Pal50YUV16[32] = {
   0x11F50101, 0x4B6A01B0, 0x02F85640, 0x00010023,
   0x00000024, 0x4D2B4D6D, 0x4D8A4D4C, 0x00435A4E,
   0x00000000, 0x00435A4E, 0x00000000, 0x013C0144,
   0x113901B1, 0x10010001, 0x00010001, 0x00010001,
   0x00000000, 0x00000000, 0x28500100, 0x1AE771F0,
   0x0DB4A574, 0x00C1188E, 0xC4C0CBE2, 0xFCECDECF,
   0x13130F08, 0x00080C0F, 0x00FF0000, 0x00000000,
   0x02800000, 0x000000FF, 0x00FF00FF, 0x00FF00FF};

static const u32 FS_VIDEO_Mode640X480Pal60YUV16[32] = {
   0x0F060001, 0x476901AD, 0x02EA5140, 0x00030018,
   0x00020019, 0x410C410C, 0x40ED40ED, 0x00435A4E,
   0x00000000, 0x00435A4E, 0x00000000, 0x00050176,
   0x110701AE, 0x10010001, 0x00010001, 0x00010001,
   0x00000000, 0x00000000, 0x28500100, 0x1AE771F0,
   0x0DB4A574, 0x00C1188E, 0xC4C0CBE2, 0xFCECDECF,
   0x13130F08, 0x00080C0F, 0x00FF0000, 0x00000000,
   0x02800000, 0x000000FF, 0x00FF00FF, 0x00FF00FF};

void FS_VIDEO_Init(u32 VideoMode)
{
    u32 Counter=0;
    u32 *pSrcAddr = (u32*)NULL;
    u32 *pDstAddr = MEM_FS_VIDEO_BASE_PTR;

    switch(VideoMode)
    {
      case FS_VIDEO_640X480_NTSC_YUV16:
        *(u32*)(0x800000CC) = 0;
        pSrcAddr = (u32*)FS_VIDEO_Mode640X480NtscYUV16;
        break;
      case FS_VIDEO_640X480_PAL50_YUV16:
        *(u32*)(0x800000CC) = 1;
        pSrcAddr = (u32*)FS_VIDEO_Mode640X480Pal50YUV16;
        break;
      case FS_VIDEO_640X480_PAL60_YUV16:
        *(u32*)(0x800000CC) = 5;
        pSrcAddr = (u32*)FS_VIDEO_Mode640X480Pal60YUV16;
        break;
    }
    for(Counter=0; Counter<32; Counter++) pDstAddr[Counter] = pSrcAddr[Counter];
}

void FS_VIDEO_SetFrameBuffer(u32 Which, u32 FrameBufferAddr)
{
    switch(Which)
    {
      case FS_VIDEO_FRAMEBUFFER_1:
        R_FS_VIDEO_FRAMEBUFFER_1 = FrameBufferAddr;
        break;
      case FS_VIDEO_FRAMEBUFFER_2:
        R_FS_VIDEO_FRAMEBUFFER_2 = FrameBufferAddr;
        break;
      case FS_VIDEO_FRAMEBUFFER_BOTH:
        R_FS_VIDEO_FRAMEBUFFER_1 = FrameBufferAddr;
        R_FS_VIDEO_FRAMEBUFFER_2 = FrameBufferAddr;
        break;
    }
}

void FS_VIDEO_WaitVSync()
{
    while (R_FS_VIDEO_HALFLINE_1 >= 200);
    while (R_FS_VIDEO_HALFLINE_1 <  200);
}

u32 FS_VIDEO_RGBToYCbYCr(u8 r,u8 g,u8 b)
{
    float Y,Cb,Cr;

    Y  =  0.257 * (float)r + 0.504 * (float)g + 0.098 * (float)b +  16.0 + 0.5;
    Cb = -0.148 * (float)r - 0.291 * (float)g + 0.439 * (float)b + 128.0 + 0.5;
    Cr =  0.439 * (float)r - 0.368 * (float)g - 0.071 * (float)b + 128.0 + 0.5;

    //Y  = CLAMP(Y , 16, 235);
    //Cb = CLAMP(Cb, 16, 240);
    //Cr = CLAMP(Cr, 16, 240);

    return (unsigned long)(((unsigned char)Y << 24) | ((unsigned char)Cb << 16) | ((unsigned char)Y << 8) | (unsigned char)Cr);
}

/*void FS_VIDEO_WaitSeconds(u32 s)
{
    u8 fps = 60;
    u32 x,y;
    if (*(u32*)(0x800000CC) == 1) fps = 50;
    for (x = 0; x < s; x++)
    {
        for (y = 0; y < fps; y++) FS_VIDEO_WaitVSync();
    }
} */

