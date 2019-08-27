/*
    Muchas graciás to DesktopMan (http://www.fritidsproblemer.no) for his YCbYCr documentation,
    which comes along with his pong game! This helped a lot :)


    This file (and color.c) also includes DOXYGEN (www.doxygen.com) compatible documentation!

    --------------------------------------------------------------------------------------
    Version    Date              Author                        Log
    --------------------------------------------------------------------------------------
    0.0        21st June 2003    Peter (www.console-dev.de)    Inital Release

*/
/*+----------------------------------------------------------------------------------------------+*/
#ifndef COLOR_H
#define COLOR_H
/*+----------------------------------------------------------------------------------------------+*/
/*+----------------------------------------------------------------------------------------------+*/
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define COLOR_BLACK        (0x00800080)
#define COLOR_MAROON       (0x266A26C0)
#define COLOR_GREEN        (0x4B554B4A)
#define COLOR_OLIVE        (0x7140718A)
#define COLOR_NAVY         (0x0EC00E75)
#define COLOR_PURPLE       (0x34AA34B5)
#define COLOR_TEAL         (0x59955940)
#define COLOR_GRAY25       (0x40804080)
#define COLOR_GRAY         (0x80808080)
#define COLOR_SILVER       (0xC080C080)
#define COLOR_RED          (0x4C544CFF)
#define COLOR_LIME         (0x952B9515)
#define COLOR_YELLOW       (0xE100E194)
#define COLOR_BLUE         (0x1DFF1D6B)
#define COLOR_FUCHSIA      (0x69D469EA)
#define COLOR_AQUA         (0xB2ABB200)
#define COLOR_CYAN         COLOR_AQUA
#define COLOR_WHITE        (0xFF80FF80)
#define COLOR_MONEYGREEN   (0xD076D074)
#define COLOR_SKYBLUE      (0xC399C36A)
#define COLOR_CREAM        (0xFA79FA82)
#define COLOR_MEDGRAY      (0xA082A07F)

#define COLOR_PINK         (FS_VIDEO_RGBToYCbYCr(0x80,0xFF,0xFF))
#define COLOR_QRHYGREEN    (FS_VIDEO_RGBToYCbYCr(0x00,0xFF,0x80))
#define COLOR_ORANGE       (FS_VIDEO_RGBToYCbYCr(0xFF,0x80,0x00))

#define COLOR_GRAY50       COLOR_GRAY
#define COLOR_GRAY75       COLOR_SILVER

#endif /* DOXYGEN_SHOULD_SKIP_THIS */
/*+----------------------------------------------------------------------------------------------+*/
/*+----------------------------------------------------------------------------------------------+*/
/*+----------------------------------------------------------------------------------------------+*/
//#ifdef __cplusplus
//   extern "C" {
//#endif /* __cplusplus */
/*+----------------------------------------------------------------------------------------------+*/
//extern u8 COLOR_GetRValue(u32 YCbYCr);
//extern u8 COLOR_GetGValue(u32 YCbYCr);
//extern u8 COLOR_GetBValue(u32 YCbYCr);
/*+----------------------------------------------------------------------------------------------+*/

/*+----------------------------------------------------------------------------------------------+*/


//#ifdef __cplusplus
//   }
//#endif /* __cplusplus */

#endif /* COLOR_H */

/* END OF FILE */
