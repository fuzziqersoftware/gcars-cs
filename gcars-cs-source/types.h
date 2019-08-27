#ifndef _TYPES_H
#define _TYPES_H

#ifdef FUZZIQER_TYPES

#define false 0
#define true 1

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

typedef unsigned long size_t;
typedef unsigned char bool;

#define NULL (void*)0

#endif

typedef unsigned char u48[6];
typedef signed char s48[6];
typedef volatile unsigned char vu48[6];
typedef volatile signed char vs48[6];

#endif

