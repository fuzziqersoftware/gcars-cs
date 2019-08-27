#include "gamecube.h"

void sram_checksum(SRAM* sram)
{
    int i;
    u16* buf = (u16*)sram;
    sram->checksum[0] = 0;
    sram->checksum[1] = 0;
    for (i = 0; i < 4; ++i)
    {
        sram->checksum[0] += buf[0x06 + i];
        sram->checksum[1] += (buf[0x06 + i] ^ 0xFFFF);
    }
}

void sram_read(SRAM* data)
{
    unsigned long val = 0x20000100;
    exi_select(0,1,3);
    exi_imm(0,&val,4,EXI_WRITE);
    exi_imm_ex(0,data,64,EXI_READ);
    exi_deselect(0);
}

void sram_write(SRAM* data)
{
    sram_checksum(data);
    unsigned long val = 0xA0000100;
    exi_select(0,1,3);
    exi_imm(0,&val,4,EXI_WRITE);
    exi_imm_ex(0,data,64,EXI_WRITE);
    exi_deselect(0);
}

