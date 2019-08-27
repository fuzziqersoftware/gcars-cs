#define FUZZIQER_TYPES
#include "types.h"
#include "text.h"
#include "textfont.raw.c"

static u32 *s_pFrameBuffer;
static u32 s_Color;
static u8 *s_pLetterPointer[256];
static char s_Abc[98] = "_!\"#$\%&'()*+,-./0123456789:;<=>?@abcdefghijklmnopqrstuvwxyz[µ]°°°ABCDEFGHIJKLMNOPQRSTUVWXYZ+|{}¿";

void DEBUG_SetFrameBuffer(u32 *pFrameBuffer) { s_pFrameBuffer = pFrameBuffer; }
void DEBUG_SetTextColor(u32 Color) { s_Color = Color; }

void DEBUG_InitText(u32 *pFrameBuffer, u32 Color)
{
   int i;
   int iAbcLen=sizeof(s_Abc);

   for(i=0; i<256; i++)
   {
      s_pLetterPointer[i] = (u8*)NULL;
   }

   for(i=0; i<iAbcLen; i++)
   {
      if(NULL == s_pLetterPointer[(u8)s_Abc[i]])
      {
         s_pLetterPointer[(u8)s_Abc[i]] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*i];
      }
   }

   for(i=0; i<256; i++)
   {
      if(NULL == s_pLetterPointer[i])
      {
         s_pLetterPointer[i] = s_pLetterPointer[' '];
      }
   }

   s_pLetterPointer[0x0] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(16)]; // 0
   s_pLetterPointer[0x1] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(17)]; // 1
   s_pLetterPointer[0x2] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(18)]; // 2
   s_pLetterPointer[0x3] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(19)]; // 3
   s_pLetterPointer[0x4] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(20)]; // 4
   s_pLetterPointer[0x5] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(21)]; // 5
   s_pLetterPointer[0x6] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(22)]; // 6
   s_pLetterPointer[0x7] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(23)]; // 7
   s_pLetterPointer[0x8] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(24)]; // 8
   s_pLetterPointer[0x9] = (u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(25)]; // 9
   s_pLetterPointer[0xa] = s_pLetterPointer['A'];//(u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(33)]; // a
   s_pLetterPointer[0xb] = s_pLetterPointer['B'];//(u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(34)]; // b
   s_pLetterPointer[0xc] = s_pLetterPointer['C'];//(u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(35)]; // c
   s_pLetterPointer[0xd] = s_pLetterPointer['D'];//(u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(36)]; // d
   s_pLetterPointer[0xe] = s_pLetterPointer['E'];//(u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(37)]; // e
   s_pLetterPointer[0xf] = s_pLetterPointer['F'];//(u8*)&DEBUG_FontBitmap[DEBUG_TEXT_CHAR_SIZE*(38)]; // f

   DEBUG_SetFrameBuffer (pFrameBuffer);
   DEBUG_SetTextColor   (Color);
}

void DEBUG_ShowValueU8(short x, short y, u8 ValueToShow)
{
   int xx,yy, act=0;
   u8 *pLetter = (u8*)NULL;
   u8 _1, _2;

   ValueToShow &= 0xff;

   _1 = (u8)(ValueToShow>>4);
   _2 = (u8)(ValueToShow&0xf);

   pLetter = s_pLetterPointer[_1];

   for(yy=0; yy<DEBUG_TEXT_CHAR_HEIGHT; yy++)
   {
      for(xx=0; xx<DEBUG_TEXT_CHAR_WIDTH; xx++)
      {
         if(0 != pLetter[act])
         {
            s_pFrameBuffer[(320*(y+yy))+((xx+x)>>1)] = s_Color;
         }
         act++;
      }
   }

   act=0;
   _1 = (u8)(ValueToShow>>4);
   _2 = (u8)(ValueToShow&0xf);

   pLetter = s_pLetterPointer[_2];

   for(yy=0; yy<DEBUG_TEXT_CHAR_HEIGHT; yy++)
   {
      for(xx=0; xx<DEBUG_TEXT_CHAR_WIDTH; xx++)
      {
         if(0 != pLetter[act])
         {
            s_pFrameBuffer[(320*(y+yy))+((xx+DEBUG_TEXT_CHAR_WIDTH+x)>>1)] = s_Color;
         }
         act++;
      }
   }
}

void DEBUG_ShowValueU16(short x, short y, u16 ValueToShow)
{
    u8 _1, _2;

    ValueToShow &= 0xffff;

    _1 = DEBUG_GET_1ST_U8_FROM_U16(ValueToShow);
    _2 = DEBUG_GET_2ND_U8_FROM_U16(ValueToShow);

    DEBUG_ShowValueU8(   x, y, _1);
    DEBUG_ShowValueU8(32+x, y, _2);
}

void DEBUG_ShowValueU32(short x, short y, u32 ValueToShow)
{
    u16 _1, _2;

    ValueToShow &= 0xffffffff;

    _1 = DEBUG_GET_1ST_U16_FROM_U32(ValueToShow);
    _2 = DEBUG_GET_2ND_U16_FROM_U32(ValueToShow);

    DEBUG_ShowValueU16(   x, y, _1);
    DEBUG_ShowValueU16(64+x, y, _2);
}

void DEBUG_PrintChar(short x, short y, char CharToPrint)
{
    int xx,yy, act=0;
    u8 *pLetter = (u8*)NULL;

    pLetter = s_pLetterPointer[(u8)CharToPrint];

    for(yy=0; yy<DEBUG_TEXT_CHAR_HEIGHT; yy++)
    {
        for(xx=0; xx<DEBUG_TEXT_CHAR_WIDTH; xx++)
        {
            if(0 != pLetter[act]) s_pFrameBuffer[(320*(y+yy))+((xx+x)>>1)] = s_Color;
            act++;
        }
    }
}

void DEBUG_Print(short x,short y,char *pText)
{
    int i;
    int iCurX = x,iCurY = y;
    for (i = 0; i < strlen(pText); i++)
    {
        iCurX += DEBUG_TEXT_CHAR_WIDTH;
        if (iCurX >= (640 - DEBUG_TEXT_CHAR_WIDTH))
        {
            iCurX = x;
            iCurY += 20;
        }
        if (' ' != pText[i])
        {
            if ('\n' == pText[i])
            {
                iCurX = x;
                iCurY += 20;
            } else DEBUG_PrintChar(iCurX,iCurY,pText[i]);
        }
    }
}

