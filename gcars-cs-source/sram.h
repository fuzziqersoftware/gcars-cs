typedef struct {
    u16 checksum[2];
    u32 ead0;
    u32 ead1;
    u32 counterbias;
    u8 displayoffset;
    u8 ntd;
    u8 language;
    u8 flags;
    u8 flashID[0x18];
    u32 wirelessKbID;
    u32 wirelessPadID[2];
    u8 lastDVDError;
    u8 unused1;
    u16 flashIDChecksum[2];
    u16 unused2; } SRAM;

void sram_checksum(SRAM*);
void sram_read(SRAM*);
void sram_write(SRAM*);

