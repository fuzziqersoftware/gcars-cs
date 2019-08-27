#ifndef __IS_HOOK
#define __IS_HOOK
#endif

#define FUZZIQER_TYPES
#include "types.h"
#include "memorymap.h"

#include "exi.h"
#include "bba.h"

#include "cs.h"

////////////////////////////////////////////////////////////////////////////////

//#define UNPLUG_BUTTONS 0x0504

#define CODE_TYPE_ZERO                 0
#define CODE_TYPE_IF_EQUAL             1
#define CODE_TYPE_IF_NOT_EQUAL         2
#define CODE_TYPE_IF_LESS_SIGNED       3
#define CODE_TYPE_IF_GREATER_SIGNED    4
#define CODE_TYPE_IF_LESS_UNSIGNED     5
#define CODE_TYPE_IF_GREATER_UNSIGNED  6
#define CODE_TYPE_IF_AND               7

#define SIZE_BYTE      0
#define SIZE_HALFWORD  1
#define SIZE_WORD      2
#define SIZE_EXTENDED  3

void vsync();
void dcache_flush(void*,int);
void dcache_inv(void*,int);
void flush_code(void*,int);

int _start(int);
void ecodes(u32);
void rcodes(u32,u32);
u32 ecode(u32);
u32 rcode(u32);
u32 cond_skip_codes(u32,u8);
u32 cond_parse_code(u32,u8,u8,u8,u32,u32);

////////////////////////////////////////////////////////////////////////////////

void controlsim();
void CSBroadcast(CSPacket*);
u32 CSChecksum(CSPacket*);

