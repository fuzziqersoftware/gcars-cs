#ifndef __EXI_H__
#define __EXI_H__

#define EXI_BASE					 0xCC006800
#define EXI_STATUS0	*(unsigned long*)0xCC006800
#define EXI_DMABUF0	*(unsigned long*)0xCC006804
#define EXI_DMALEN0	*(unsigned long*)0xCC006808
#define EXI_DMACNT0	*(unsigned long*)0xCC00680C
#define EXI_IMMDAT0	*(unsigned long*)0xCC006810
#define EXI_STATUS1	*(unsigned long*)0xCC006814
#define EXI_DMABUF1	*(unsigned long*)0xCC006818
#define EXI_DMALEN1	*(unsigned long*)0xCC00681C
#define EXI_DMACNT1	*(unsigned long*)0xCC006820
#define EXI_IMMDAT1	*(unsigned long*)0xCC006824
#define EXI_STATUS2	*(unsigned long*)0xCC006828
#define EXI_DMABUF2	*(unsigned long*)0xCC00682C
#define EXI_DMALEN2	*(unsigned long*)0xCC006830
#define EXI_DMACNT2	*(unsigned long*)0xCC006834
#define EXI_IMMDAT2	*(unsigned long*)0xCC006838
		
#define EXI_CONTROL_TYPE_READ        0
#define EXI_CONTROL_TYPE_WRITE       1
#define EXI_READ                     0
#define EXI_WRITE                    1
#define EXI_CONTROL_IMM              0
#define EXI_CONTROL_DMA              2
#define EXI_CONTROL_ENABLE           1

#define EXI_MEMORY_CARD_59      0x00000004
#define EXI_MEMORY_CARD_123     0x00000008
#define EXI_MEMORY_CARD_251     0x00000010
#define EXI_MEMORY_CARD_507     0x00000020
#define EXI_MEMORY_CARD_1019    0x00000040
#define EXI_MEMORY_CARD_2043    0x00000080

#define EXI_USB_ADAPTER         0x01010000
#define EXI_IS_VIEWER           0x05070000
#define EXI_MODEM               0x02020000
#define EXI_ETHER               0x04020200

void exi_init(void);
int exi_probe(unsigned long channel);
void exi_select(unsigned long channel, unsigned long device, unsigned long frequency);
void exi_deselect(unsigned long channel);
void exi_imm(unsigned long channel,void* abuffer, unsigned long size, unsigned long type);
void exi_imm_ex(unsigned long channel,void* abuffer, unsigned long size, unsigned long type);
void exi_dma(unsigned long channel,void* abuffer, unsigned long size, unsigned long type);
unsigned long exi_deviceid(unsigned long channel, unsigned long device);
char* exi_devicename(int type);

#endif
