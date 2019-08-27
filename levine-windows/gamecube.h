#ifndef _TYPES_H
#define _TYPES_H

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned long u32;
typedef unsigned long long u64;
typedef signed char s8;
typedef signed short s16;
typedef signed long s32;
typedef signed long long s64;

typedef volatile unsigned char vu8;
typedef volatile unsigned short vu16;
typedef volatile unsigned long vu32;
typedef volatile unsigned long long vu64;
typedef volatile signed char vs8;
typedef volatile signed short vs16;
typedef volatile signed long vs32;
typedef volatile signed long long vs64;

typedef unsigned char u48[6];
typedef signed char s48[6];
typedef volatile unsigned char vu48[6];
typedef volatile signed char vs48[6];

#endif

DWORD byteswap(DWORD x);
WORD byteswaps(WORD x);
u32 checksum(void* data,u32 size);
char FilterString_GameCube(char* s);
char FilterString_GameID(char* s);
char FilterString_Hex(char* s);
char FilterString_IP(char* s);

