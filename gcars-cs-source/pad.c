#include "gamecube.h"

void FS_PAD_Init()
{
    *(vu32*)0xCC006430 = 0x00000000;
    *(vu32*)0xCC006438 = 0x80000000;
    *(vu32*)0xCC006430 = 0x00F70200;
    *(vu32*)0xCC006438 = 0x80000000;
    *(vu32*)0xCC006480 = 0x00000000;
    *(vu32*)0xCC006434 = 0xC0010301;
    *(vu32*)0xCC006438 = 0x00000000;
    (void)*(volatile unsigned long*)0xCC006434;
    *(vu32*)0xCC006430 |= 0xF0; // enable all four controller ports
    *(vu32*)0xCC006400 = 0x00400300;
    *(vu32*)0xCC00640C = 0x00400300;
    *(vu32*)0xCC006418 = 0x00400300;
    *(vu32*)0xCC006424 = 0x00400300;
    FS_VIDEO_WaitVSync();

    *(vu32*)0xCC006430 = 0x00000000;
    *(vu32*)0xCC006438 = 0x80000000;
    *(vu32*)0xCC006430 = 0x00F70200;
    *(vu32*)0xCC006438 = 0x80000000;
    *(vu32*)0xCC006480 = 0x00000000;
    *(vu32*)0xCC006434 = 0xC0010301;
    *(vu32*)0xCC006438 = 0x00000000;
    (void)*(volatile unsigned long*)0xCC006434;
    *(vu32*)0xCC006430 |= 0xF0; // enable all four controller ports
    *(vu32*)0xCC006400 = 0x00400300;
    *(vu32*)0xCC00640C = 0x00400300;
    *(vu32*)0xCC006418 = 0x00400300;
    *(vu32*)0xCC006424 = 0x00400300;
}

void FS_PAD_ReadAllState(PAD* pPad)
{
    //*(u32*)(0x80001800) = (u32)pPad;

    PAD pads[4];
    s32 analog,x;
    //memset(pPad,0,sizeof(PAD));
    //memset(&pads[0],0,sizeof(PAD) * 4);
#ifdef PAD_DEBUG
    GCARSClearFrameBuffer(COLOR_BLACK);
#endif
    for (x = 0; x < 4; x++) FS_PAD_ReadState(&pads[x],x);

    pPad->Digital.Left  = pads[0].Digital.Left | pads[1].Digital.Left | pads[2].Digital.Left | pads[3].Digital.Left;
    pPad->Digital.Right = pads[0].Digital.Right | pads[1].Digital.Right | pads[2].Digital.Right | pads[3].Digital.Right;
    pPad->Digital.Down  = pads[0].Digital.Down | pads[1].Digital.Down | pads[2].Digital.Down | pads[3].Digital.Down;
    pPad->Digital.Up    = pads[0].Digital.Up | pads[1].Digital.Up | pads[2].Digital.Up | pads[3].Digital.Up;
    pPad->Digital.Z     = pads[0].Digital.Z | pads[1].Digital.Z | pads[2].Digital.Z | pads[3].Digital.Z;
    pPad->Digital.R     = pads[0].Digital.R | pads[1].Digital.R | pads[2].Digital.R | pads[3].Digital.R;
    pPad->Digital.L     = pads[0].Digital.L | pads[1].Digital.L | pads[2].Digital.L | pads[3].Digital.L;
    pPad->Digital.A     = pads[0].Digital.A | pads[1].Digital.A | pads[2].Digital.A | pads[3].Digital.A;
    pPad->Digital.B     = pads[0].Digital.B | pads[1].Digital.B | pads[2].Digital.B | pads[3].Digital.B;
    pPad->Digital.X     = pads[0].Digital.X | pads[1].Digital.X | pads[2].Digital.X | pads[3].Digital.X;
    pPad->Digital.Y     = pads[0].Digital.Y | pads[1].Digital.Y | pads[2].Digital.Y | pads[3].Digital.Y;
    pPad->Digital.Start = pads[0].Digital.Start | pads[1].Digital.Start | pads[2].Digital.Start | pads[3].Digital.Start;
    analog = pads[0].Analog.X + pads[1].Analog.X + pads[2].Analog.X + pads[3].Analog.X;
    if (analog >  0x0000007F) analog =  0x0000007F;
    if (analog < -0x0000007F) analog = -0x0000007F;
    pPad->Analog.X = analog;
    analog = pads[0].Analog.Y + pads[1].Analog.Y + pads[2].Analog.Y + pads[3].Analog.Y;
    if (analog >  0x0000007F) analog =  0x0000007F;
    if (analog < -0x0000007F) analog = -0x0000007F;
    pPad->Analog.Y = analog;
    analog = pads[0].Trigger.L + pads[1].Trigger.L + pads[2].Trigger.L + pads[3].Trigger.L;
    if (analog >  0x0000007F) analog =  0x0000007F;
    if (analog < -0x0000007F) analog = -0x0000007F;
    pPad->Trigger.L = analog;
    analog = pads[0].Trigger.R + pads[1].Trigger.R + pads[2].Trigger.R + pads[3].Trigger.R;
    if (analog >  0x0000007F) analog =  0x0000007F;
    if (analog < -0x0000007F) analog = -0x0000007F;
    pPad->Trigger.R = analog;
    analog = pads[0].AnalogC.X + pads[1].AnalogC.X + pads[2].AnalogC.X + pads[3].AnalogC.X;
    if (analog >  0x0000007F) analog =  0x0000007F;
    if (analog < -0x0000007F) analog = -0x0000007F;
    pPad->AnalogC.X = analog;
    analog = pads[0].AnalogC.Y + pads[1].AnalogC.Y + pads[2].AnalogC.Y + pads[3].AnalogC.Y;
    if (analog >  0x0000007F) analog =  0x0000007F;
    if (analog < -0x0000007F) analog = -0x0000007F;
    pPad->AnalogC.X = analog;
#ifdef PAD_DEBUG
    DEBUG_ShowValueU32(40 ,360,*(u32*)((u32)pPad + 0x00));
    DEBUG_ShowValueU32(168,360,*(u32*)((u32)pPad + 0x04));
    DEBUG_ShowValueU32(296,360,*(u32*)((u32)pPad + 0x08));
    DEBUG_ShowValueU32(40 ,380,*(u32*)((u32)pPad + 0x0C));
    DEBUG_ShowValueU32(168,380,*(u32*)((u32)pPad + 0x10));
#endif
}

void FS_PAD_ReadState(PAD *pPad, u8 PadChannel)
{
   u32 Value32[3];
   u8 Value8[2];
   s8 ValueS8[2];
   u32 PadChannelAddr = (0xCC006400 + (12 * PadChannel));
   FS_PAD_BUTTONS_DIGITAL *pDigital = &pPad->Digital;

   Value32[0] = *(u32*)(PadChannelAddr + 0);
   Value32[1] = *(u32*)(PadChannelAddr + 4);
   Value32[2] = *(u32*)(PadChannelAddr + 8);
#ifdef PAD_DEBUG
   DEBUG_ShowValueU32(40 ,280 + (PadChannel * 20),Value32[0]);
   DEBUG_ShowValueU32(168,280 + (PadChannel * 20),Value32[1]);
   DEBUG_ShowValueU32(296,280 + (PadChannel * 20),Value32[2]);
#endif
   if (Value32[1] & 0x80000000)
   {
       memset(pPad,0,sizeof(PAD));
       return;
   }
   Value8[0] = (Value32[1] >> 16);
   Value8[1] = (Value32[1] >> 16) >> 8;
   pDigital->Left  = Value8[0] & 0x01;
   pDigital->Right = Value8[0] & 0x02;
   pDigital->Down  = Value8[0] & 0x04;
   pDigital->Up    = Value8[0] & 0x08;
   pDigital->Z     = Value8[0] & 0x10;
   pDigital->R     = Value8[0] & 0x20;
   pDigital->L     = Value8[0] & 0x40;
   pDigital->A     = Value8[1] & 0x01;
   pDigital->B     = Value8[1] & 0x02;
   pDigital->X     = Value8[1] & 0x04;
   pDigital->Y     = Value8[1] & 0x08;
   pDigital->Start = Value8[1] & 0x10;
   ValueS8[0] = ((Value32[1]) & 0xff);
   ValueS8[1] = ((Value32[1]) & 0xff00) >> 8;
   pPad->Analog.X = 0x80 + ValueS8[1];
   pPad->Analog.Y = 0x80 - ValueS8[0];
   Value8[0] = ((Value32[2]) & 0xff);
   Value8[1] = ((Value32[2]) & 0xff00) >> 8;
   pPad->Trigger.L = Value8[0];
   pPad->Trigger.R = Value8[1];
   ValueS8[0] = (Value32[2] >> 16);
   ValueS8[1] = (Value32[2] >> 16) >> 8;
   pPad->AnalogC.X = 0x80 + ValueS8[1];
   pPad->AnalogC.Y = 0x80 - ValueS8[0];
}

void FS_PAD_WaitEmptyAll()
{
    PAD controller;
    FS_PAD_ReadAllState(&controller);
    while (!FS_PAD_IsEmpty(&controller))
    {
        FS_PAD_ReadAllState(&controller);
        FS_VIDEO_WaitVSync();
    }
}

/*void FS_PAD_WaitEmpty(u8 chan)
{
    PAD controller;
    FS_PAD_ReadState(&controller,chan);
    while (!FS_PAD_IsEmpty(&controller))
    {
        FS_PAD_ReadState(&controller,chan);
        FS_VIDEO_WaitVSync();
    }
} */

bool FS_PAD_IsEmpty(PAD* controller)
{
    if (controller->Digital.Up ||
        controller->Digital.Down ||
        controller->Digital.Left ||
        controller->Digital.Right ||
        controller->Digital.L ||
        controller->Digital.R ||
        controller->Digital.A ||
        controller->Digital.B ||
        controller->Digital.X ||
        controller->Digital.Y ||
        controller->Digital.Z ||
        controller->Digital.Start ||
        (controller->AnalogC.X >= 0x40) ||
        (controller->AnalogC.X <= -0x40) ||
        (controller->AnalogC.Y >= 0x40) ||
        (controller->AnalogC.Y <= -0x40) ||
        (controller->Analog.X >= 0x40) ||
        (controller->Analog.X <= -0x40) ||
        (controller->Analog.Y >= 0x40) ||
        (controller->Analog.Y <= -0x40))
        return false;
    return true;
}

