#ifndef BBA_H
#define BBA_H

#define BBA_COMMAND_WRITE		0xC0000000
#define BBA_COMMAND_READ			0x80000000

#define BBA_COMMAND_WRITES		0x4000
#define BBA_COMMAND_READS		0x0000

unsigned char BBA_ReceiveCommand (unsigned long reg, unsigned char *abuf, unsigned long size);
unsigned char BBA_SendCommand (unsigned long reg, unsigned char *abuf, unsigned long size);
unsigned char BBA_ReceiveCommandShort (unsigned long reg, unsigned char *abuf, unsigned long size);
unsigned char BBA_SendCommandShort (unsigned long reg, unsigned char *abuf, unsigned long size);

unsigned char eth_init(u48*,u8);
unsigned char eth_send(u8*,u16);
unsigned short eth_receive(unsigned char*);
u8 eth_getspeed();
void eth_setspeed(u8 speed);

#endif
