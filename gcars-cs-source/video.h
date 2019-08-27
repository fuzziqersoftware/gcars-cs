/*
    This file bases on the "GAMECUBE LOW LEVEL INFO" document and sourceode released
    by Titanik of Crazy Nation. Without these informations this file would not exist.
    Great work Titanik!

    This file (and video.c) includes DOXYGEN (www.doxygen.com) compatible documentation!

    --------------------------------------------------------------------------------------
    Version    Date              Author                        Log
    --------------------------------------------------------------------------------------
    0.0        22nd June 2003    Peter (www.console-dev.de)    Inital Release
    0.1        23rd June 2003    Peter (www.console-dev.de)    + assertions added

*/
#ifndef FS_VIDEO_H
#define FS_VIDEO_H
/*+----------------------------------------------------------------------------------------------+*/
/*+----------------------------------------------------------------------------------------------+*/
#define MEM_FS_VIDEO_BASE               (0xCC002000)           ///< Memory address of Video Interface
#define MEM_FS_VIDEO_BASE_PTR           (u32*)MEM_FS_VIDEO_BASE   ///< Pointer to Video Interface
/*+----------------------------------------------------------------------------------------------+*/
#define R_FS_VIDEO_FRAMEBUFFER_1        *(vu32*)(MEM_FS_VIDEO_BASE+0x1C)   ///< Framebuffer1 register location. Includes typecasting for direct c writes.
#define R_FS_VIDEO_FRAMEBUFFER_2        *(vu32*)(MEM_FS_VIDEO_BASE+0x24)   ///< Framebuffer2 register location. Includes typecasting for direct c writes.
#define R_FS_VIDEO_HALFLINE_1           *(vu16*)(MEM_FS_VIDEO_BASE+0x2C)   ///< HalfLine1 register location. Includes typecasting for direct c writes.
#define R_FS_VIDEO_HALFLINE_2           *(vu16*)(MEM_FS_VIDEO_BASE+0x2E)   ///< HalfLine2 register location. Includes typecasting for direct c writes.
#define R_FS_VIDEO_STATUS               *(vu16*)(MEM_FS_VIDEO_BASE+0x6C)   ///< VideoStatus register location. Includes typecasting for direct c writes.
/*+----------------------------------------------------------------------------------------------+*/
#define FS_VIDEO_640X480_NTSC_YUV16     (0)  ///< Helper define. Can be used with FS_VIDEO_Init()
#define FS_VIDEO_640X480_PAL50_YUV16    (1)  ///< Helper define. Can be used with FS_VIDEO_Init()
#define FS_VIDEO_640X480_PAL60_YUV16    (2)  ///< Helper define. Can be used with FS_VIDEO_Init()
/*+----------------------------------------------------------------------------------------------+*/
#define FS_VIDEO_FRAMEBUFFER_1          (1)  ///< Helper define. Can be used with FS_VIDEO_SetFrameBuffer()
#define FS_VIDEO_FRAMEBUFFER_2          (2)  ///< Helper define. Can be used with FS_VIDEO_SetFrameBuffer()
#define FS_VIDEO_FRAMEBUFFER_BOTH       (0)  ///< Helper define. Can be used with FS_VIDEO_SetFrameBuffer()
/*+----------------------------------------------------------------------------------------------+*/
#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */
/*+----------------------------------------------------------------------------------------------+*/
//extern void FS_VIDEO_Init            (u32 VideoMode);
//extern void FS_VIDEO_SetFrameBuffer  (u32 Which, u32 FrameBufferAddr);
//extern void FS_VIDEO_WaitVSync       (void);
extern u32 FS_VIDEO_RGBToYCbYCr      (u8,u8,u8);
//extern void FS_VIDEO_WaitSeconds     (u32);
/*+----------------------------------------------------------------------------------------------+*/
#ifdef __cplusplus
   }
#endif /* __cplusplus */
/*+----------------------------------------------------------------------------------------------+*/
#endif /* FS_VIDEO_H */


/* END OF FILE */
