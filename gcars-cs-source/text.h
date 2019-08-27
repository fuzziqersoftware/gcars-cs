/*
    This file (and debug.c) includes DOXYGEN (www.doxygen.com) compatible documentation!

    --------------------------------------------------------------------------------------
    Version    Date              Author                        Log
    --------------------------------------------------------------------------------------
    0.0        20th June 2003    Peter (www.console-dev.de)    Inital Release
    0.1        21th June 2003    Peter (www.console-dev.de)    + DEBUG_Print() added
                                                               - NULL defined removed.
                                                                 Its in types.h yet
    0.2        22nd June 2003    Peter (www.console-dev.de)    + more documentation
                                                               + added #ifdef __cplusplus stuff
    0.3        23rd June 2003    Peter (www.console-dev.de)    + ASSERT functions added
                                                               + DEBUG_Print improved. Can now
                                                                 also handle "\n" for a new line
                                                               + Debug font includes an underscore
                                                                 now! "_"

*/
/*+----------------------------------------------------------------------------------------------+*/
#ifndef DEBUG_H
#define DEBUG_H
/*+----------------------------------------------------------------------------------------------+*/

/*+----------------------------------------------------------------------------------------------+*/
#define DEBUG_ENABLE_ASSERTIONS  ///< Comment this out to disable assertsion checking
/*+----------------------------------------------------------------------------------------------+*/
#ifdef DEBUG_ENABLE_ASSERTIONS
/*!
\brief Check an assertion statement

\param Assertion
   The condition that you expect to hold true

\param pMessage
   The message which is being display if the condition is not true

   An assertion statement specifies a condition that you expect to hold true at some
   particular point in your program.

   If that condition does not hold true, the assertion fails, execution of your
   program is interrupted, and the "Assertion Failed" screen appears!
*/
         #define	ASSERT(Assertion, pMessage)         \
                           DEBUG_Assert((Assertion),  \
                           (pMessage),                \
                           #Assertion,                \
                           __FILE__,                  \
                           __LINE__)
#else
   #define ASSERT(Assertion, Message)
#endif
/*+----------------------------------------------------------------------------------------------+*/
#define DEBUG_TEXT_CHAR_WIDTH          (16)  ///< Width of debug font
#define DEBUG_TEXT_CHAR_HEIGHT         (16)  ///< Height of debug font
#define DEBUG_TEXT_CHAR_SIZE           (DEBUG_TEXT_CHAR_WIDTH*DEBUG_TEXT_CHAR_HEIGHT)  ///< Size of one debugfont character in bytes. (height*width)
/*+----------------------------------------------------------------------------------------------+*/
#define DEBUG_GET_1ST_U8_FROM_U32(n)  ((u8)((n>>24)&0xff))     ///< Returns the first u8, seen from the left, of an u32. \n For example: 0x12345678 would return 0x12
#define DEBUG_GET_2ND_U8_FROM_U32(n)  ((u8)((n>>16)&0x00ff))   ///< Returns the second u8, seen from the left, of an u32. \n For example: 0x12345678 would return 0x34
#define DEBUG_GET_3RD_U8_FROM_U32(n)  ((u8)((n>>8)&0x0000ff))  ///< Returns the third u8, seen from the left, of an u32. \n For example: 0x12345678 would return 0x56
#define DEBUG_GET_4TH_U8_FROM_U32(n)  ((u8)((n)&0x000000ff))   ///< Returns the fourth u8, seen from the left, of an u32. \n For example: 0x12345678 would return 0x78
#define DEBUG_GET_1ST_U16_FROM_U32(n) ((u16)((n>>16)&0xffff))  ///< Returns the first u16, seen from the left, of an u32. \n For example: 0x12345678 would return 0x1234
#define DEBUG_GET_2ND_U16_FROM_U32(n) ((u16)((n)&0x0000ffff))  ///< Returns the second u16, seen from the left, of an u32. \n For example: 0x12345678 would return 0x5678
#define DEBUG_GET_1ST_U8_FROM_U16(n)  ((u8)((n>>8)&0xff))      ///< Returns the first u8, seen from the left, of an u16. \n For example: 0x1234 would return 0x12
#define DEBUG_GET_2ND_U8_FROM_U16(n)  ((u8)((n)&0x00ff))       ///< Returns the second u8, seen from the left, of an u16. \n For example: 0x1234 would return 0x34
/*+----------------------------------------------------------------------------------------------+*/
#ifdef __cplusplus
   extern "C" {
#endif /* __cplusplus */
/*+----------------------------------------------------------------------------------------------+*/
extern void DEBUG_InitText      (u32 *pFrameBuffer, u32 Color);
extern void DEBUG_SetFrameBuffer(u32 *pFrameBuffer);
extern void DEBUG_SetTextColor  (u32 Color);

extern void DEBUG_ShowValueU8   (short x, short y, u8  ValueToShow);
extern void DEBUG_ShowValueU16  (short x, short y, u16 ValueToShow);
extern void DEBUG_ShowValueU32  (short x, short y, u32 ValueToShow);

extern void DEBUG_Print(short x, short y, char *pText);
extern void DEBUG_PrintChar(short x, short y, char CharToPrint);

extern void DEBUG_Assert(u8 Assertion, char* pMessage, char* pExpression, char* pFileName, u32 LineNo);
/*+----------------------------------------------------------------------------------------------+*/
#ifdef __cplusplus
   }
#endif /* __cplusplus */
/*+----------------------------------------------------------------------------------------------+*/



#endif /* DEBUG_H */

/* END OF FILE */
