#include "exi.h"

#ifndef __IS_HOOK
void exi_init(void)
{
    while (EXI_DMACNT0 & EXI_CONTROL_ENABLE);
    while (EXI_DMACNT1 & EXI_CONTROL_ENABLE);
    while (EXI_DMACNT2 & EXI_CONTROL_ENABLE);
    EXI_STATUS0 = 0;
    EXI_STATUS1 = 0;
    EXI_STATUS2 = 0;
}

int exi_probe(unsigned long channel)
{
	if (*(unsigned long*)(&EXI_STATUS0 + (channel * 5)) & 0x1000) return 1;
    else return 0;
}
#endif

void exi_select(unsigned long channel, unsigned long device, unsigned long frequency)
{
    *(unsigned long*)(&EXI_STATUS0 + (channel * 5)) = (*(unsigned long*)(&EXI_STATUS0 + channel * 5) & 0x405) | (0x80 << device) | (frequency << 4) | 0x800;
}

void exi_deselect(unsigned long channel)
{
    *(unsigned long*)(&EXI_STATUS0 + (channel * 5)) &= 0x405;
}

asm (".globl exi_imm");
asm ("exi_imm:");
asm ("    mulli   3,3,20");
asm ("    oris    3,3,0xCC00");
asm ("    lwz     7,0x6800(3)");
asm ("    ori     7,7,0x0008");
asm ("    stw     7,0x6800(3)");
asm ("    subi    5,5,1");
asm ("    slwi    7,5,4");
asm ("    mtctr   6");
asm ("    slwi    6,6,2");
asm ("    or      6,6,7");
asm ("    ori     6,6,1");
asm ("    lwz     7,0(4)");
asm ("    stw     7,0x6810(3)");
asm ("    stw     6,0x680C(3)");
asm ("  exi_imm_WaitDoneLoop:");
asm ("    lwz     6,0x680C(3)");
asm ("    andi.   6,6,1");
asm ("    bne     exi_imm_WaitDoneLoop");
asm ("    bdz     exi_imm_SkipRead");
asm ("    lwz     6,0x6810(3)");
asm ("    subfic  7,5,3");
asm ("    slwi    7,7,3");
asm ("    srw     6,6,7");
asm ("  exi_imm_ReadDataLoop:");
asm ("    stbx    6,4,5");
asm ("    srwi    6,6,8");
asm ("    subic.  5,5,1");
asm ("    bge     exi_imm_ReadDataLoop");
asm ("  exi_imm_SkipRead:");
asm ("    blr");

void exi_imm_ex(unsigned long channel,void *abuffer, unsigned long size, unsigned long type)
{
    int i;
    if (size > 4)
    {
        for (i = 0; i < (size & ~3); i += 4) exi_imm (channel, abuffer + i, 4, type);
        if (size & 3) exi_imm (channel, abuffer + i, size & 3, type);
    } else exi_imm (channel, abuffer, size, type);
}

#ifndef __IS_HOOK
void exi_dma(unsigned long channel,void *abuffer, unsigned long size, unsigned long type)
{
    *(unsigned long*)(&EXI_DMABUF0 + (channel * 5)) = (unsigned long)abuffer & 0x3FFFFE0;
    *(unsigned long*)(&EXI_DMALEN0 + (channel * 5)) = size & 0x3FFFFE0;
    *(unsigned long*)(&EXI_DMACNT0 + (channel * 5)) = EXI_CONTROL_ENABLE | EXI_CONTROL_DMA | (type << 2);
    while (*(volatile unsigned long*)(&EXI_DMACNT0 + channel * 5) & EXI_CONTROL_ENABLE);
}

unsigned long exi_deviceid(unsigned long channel, unsigned long device)
{
    int i;
    unsigned long tID = 0;
    if (exi_probe (channel))
    {
        exi_select (channel, device, 0);
        exi_imm (channel, (unsigned char*)&tID, 2, EXI_CONTROL_TYPE_WRITE);
        exi_imm (channel, (unsigned char*)&tID, 4, EXI_CONTROL_TYPE_READ);
        exi_deselect (channel);
        return tID;
    } else return 0;
}

char* exi_devicename(int type)
{
    if (type == 0) return "No device present";
    if (type & 0xFFFF0000)
    {
        switch (type)
        {
          case EXI_USB_ADAPTER: return "USB Adapter";
          case EXI_IS_VIEWER: return "IS Viewer";
          case EXI_MODEM: return "56k Modem Adapter";
          case EXI_ETHER: return "Ethernet Adapter";
        }
    } else {
        if (type & 0x0000FF03)
        {
            switch (type & 0x000000FC)
            {
              case EXI_MEMORY_CARD_59: return "3P Memory Card 59";
              case EXI_MEMORY_CARD_123: return "3P Memory Card 123";
              case EXI_MEMORY_CARD_251: return "3P Memory Card 251";
              case EXI_MEMORY_CARD_507: return "3P Memory Card 507";
              case EXI_MEMORY_CARD_1019: return "3P Memory Card 1019";
              case EXI_MEMORY_CARD_2043: return "3P Memory Card 2043";
            }
        } else {
            switch (type & 0x000000FC)
            {
              case EXI_MEMORY_CARD_59: return "N Memory Card 59";
              case EXI_MEMORY_CARD_123: return "N Memory Card 123";
              case EXI_MEMORY_CARD_251: return "N Memory Card 251";
              case EXI_MEMORY_CARD_507: return "N Memory Card 507";
              case EXI_MEMORY_CARD_1019: return "N Memory Card 1019";
              case EXI_MEMORY_CARD_2043: return "N Memory Card 2043";
            }
        }
    }
    return "Unknown device present";
}
#endif

