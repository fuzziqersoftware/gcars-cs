#ifndef __CARD_H__
#define __CARD_H__

#define CARD_FILENAME     32
#define CARD_MAXFILES    127
#define CARD_MAXICONS      8
#define CARD_READSIZE    512
#define CARD_SECTORSIZE 8192
#define CARD_SLOTA         0
#define CARD_SLOTB         1
#define CARD_SYSAREA       5
#define CARD_WRITESIZE   128

/* Banner & Icon defines */

#define CARD_BANNER_NONE  0
#define CARD_BANNER_CI    1
#define CARD_BANNER_RGB   2
#define CARD_BANNER_W    96
#define CARD_BANNER_H    32

#define CARD_ICON_NONE    0
#define CARD_ICON_CI      1
#define CARD_ICON_RGB     2
#define CARD_ICON_LOOP    0
#define CARD_ICON_BOUNCE  4
#define CARD_ICON_W      32
#define CARD_ICON_H      32

#define CARD_SPEED_END 0
#define CARD_SPEED_4   1
#define CARD_SPEED_8   2
#define CARD_SPEED_12  3

/* File attribute defines */

#define CARD_ATTRIB_PUBLIC 0x04
#define CARD_ATTRIB_NOCOPY 0x08
#define CARD_ATTRIB_NOMOVE 0x10

/* Errors */

#define CARD_ERROR_NONE     0
#define CARD_ERROR_NOCARD  -1
#define CARD_ERROR_CORRUPT -2
#define CARD_ERROR_FATAL   -3
#define CARD_ERROR_INIT    -4
#define CARD_ERROR_NOSPACE -5
#define CARD_ERROR_NOENTRY -6
#define CARD_ERROR_EXISTS  -7

typedef struct {

  unsigned char gamecode[4];         // Game code
  unsigned char company[2];          // Company code
  unsigned char padding_ff;          // Always 0xff
  unsigned char banner_fmt;          // Banner format
  unsigned char filename[CARD_FILENAME];  // Filename
  unsigned long time;                // Time of last modification
  unsigned long icon_addr;           // Offset of image data (must be 0-511)
  unsigned short icon_fmt;           // Icon formats
  unsigned short icon_speed;         // Icon speeds
  unsigned char attrib;              // File attribute
  char padding_00;                   // Always 0x00
  unsigned short block;              // Block/sector file starts at
  unsigned short length;             // Length in blocks/sectors
  unsigned short padding_ffff;       // Always 0xffff;
  unsigned long comment_addr;        // Offset of comment 
} CARD_DirEntry;

typedef struct {

  CARD_DirEntry entries[CARD_MAXFILES];    // Entries for files
  unsigned char padding[58];                  // Unused bytes after last entry
  unsigned short num;                         // Increases with each update
  unsigned short checksum1;                   // 1st part of checksum
  unsigned short checksum2;                   // 2nd part of checksum
} CARD_DirBlock;

typedef struct {

  unsigned short checksum1;                   // 1st part of checksum
  unsigned short checksum2;                   // 2nd part of checksum
  unsigned short num;                         // Increases with each update
  unsigned short freeblocks;                  // Amount of free blocks
  unsigned short last_alloc;                  // Last allocated block
  unsigned short fat[0xffb];                  // Table of allocated blocks
} CARD_FatBlock;

typedef struct {

  unsigned char gamecode[5];         // Game code
  unsigned char company[3];          // Company code
  unsigned char banner_fmt;          // Banner format
  unsigned char filename[CARD_FILENAME+1]; // Filename
  unsigned long time;                // Time of last modification
  unsigned long icon_addr;           // Offset of image data (must be 0-511)
  unsigned short icon_fmt;           // Icon formats
  unsigned short icon_speed;         // Icon speeds
  unsigned char attrib;              // File attribute
  unsigned long comment_addr;        // Offset of comment
  unsigned long size;                // Size in bytes
  int filenum;                       // File number
} CARD_File;

typedef struct {

  unsigned long banner;                       // Offset to banner data
  unsigned long banner_tlut;                  // Offset to banner's TLUT
  unsigned long icons[CARD_MAXICONS];         // Offsets for the icons
  unsigned long icons_tlut;                   // Offset of the icons' TLUT
  unsigned long data;                         // Offset to the save data
} CARD_Offsets;


/* Get icon attribute macros */

#define CARD_GetIconFmt(cfile, icon)   ((cfile->icon_fmt >> (icon * 2)) & 3)
#define CARD_GetIconSpeed(cfile, icon) ((cfile->icon_speed >> (icon * 2)) & 3)
#define CARD_GetIconAnim(cfile)        (cfile->banner_fmt & 4)

unsigned short CARD_RetrieveID(unsigned long channel);
unsigned char CARD_ReadStatus(unsigned long channel);
void CARD_ClearStatus(unsigned long channel);
void CARD_ChipErase(unsigned long channel);
void CARD_SectorErase(unsigned long channel,unsigned long sector);
void CARD_SectorProgram(unsigned long channel,unsigned char *abuf,unsigned long address,unsigned long size);
void CARD_ReadArray(unsigned long channel,unsigned char *abuf,unsigned long address,unsigned long size);
int CARD_IsPresent(unsigned long channel);

int CARD_Begin(unsigned long channel, int *fixed);
int CARD_GetOffsets(CARD_File *cfile, CARD_Offsets *offsets);
int CARD_CreateFile(CARD_File *cfile, CARD_Offsets *offsets);
int CARD_OpenFile(CARD_File *cfile, CARD_Offsets *offsets);
int CARD_OpenFilenum(CARD_File *cfile, CARD_Offsets *offsets, int filenum);
int CARD_WriteFile(CARD_File *cfile, void *buf);
int CARD_ReadFile(CARD_File *cfile, void *buf);
int CARD_ReadFirstBlock(CARD_File *cfile, void *buf);
int CARD_DeleteFile(CARD_File *cfile);
int CARD_FormatCard(unsigned long channel); // returns card size

void CARD_SetIcon(CARD_File *cfile, int icon, int fmt, int speed);
void CARD_SetIconAnim(CARD_File *cfile, int anim);
int CARD_CalcSize(CARD_File *cfile, int datalen);
int CARD_FreeSpace();
int CARD_DrawIcon(CARD_File *cfile, CARD_Offsets *offsets, int icon, void *buf, unsigned long *fb, int x, int y);

#endif
