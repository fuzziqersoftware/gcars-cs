//////////////////////////////////////////////////////////////////////
// Action Replay Project /////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////

/* GCN AR Code Types: 
 * T 0 
 *   ST 0 RAM Write/Fill 
 *   ST 1 Write to Pointer 
 *   ST 2 Add Code 
 *   ST 3 (size=2) Hook Code & (size=3) Write to CC00XXXX 
 * T 1 If Equal 
 *   ST 0 Next code 
 *   ST 1 Next 2 codes 
 *   ST 2 All codes until z4 code 
 *   ST 3 Wait until true 
 * T 2 If not equal 
 *   Same subtypes as T 1 
 * T 3 If signed less 
 *   Same subtypes as T 1 
 * T 4 If signed greater 
 *   Same subtypes as T 1 
 * T 5 If unsigned less 
 *   Same subtypes as T 1 
 * T 6 If unsigned greater 
 *   Same subtypes as T 1 
 * T 7 If AND (game value & code value != 0) 
 *   Same subtypes as T 1 */ 

/* Unsupported objects: 
 * C4 hooks with hooks types 1, 2, or 3 
 * T1-7 ST 3 
 * Kenobi's OR and AND codes */ 

#include "types.h"

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

#define CODE_LIST_START 0x817FF800 // 511 max codes active at once.
#define CONDITION_REGISTER (*(u32*)0x817FF7FC)
#define CONTROL_SIMULATOR (*(u32*)0x817FF7F8)

int _start(int);
//int main();
void ecodes(u32);
void rcodes(u32);
u32 ecode(u32);
u32 rcode(u32);
u32 cond_skip_codes(u32,u8);

asm (".globl _start");
asm ("_start:");
asm ("    b       hooked");
asm ("    b       ecodes");
asm ("    b       rcodes");
asm ("hooked:");
asm ("    stwu    1,-0x30(1)");
asm ("    mflr    0");
asm ("    stw     0,0x2C(1)");
asm ("    stw     3,0x08(1)");
asm ("    stmw    26,0x0C(1)");
asm ("    lis     3,0x817F");
asm ("    ori     3,3,0xF800");
asm ("    bl      ecodes");
asm ("    lis     4,0x817F");
asm ("    ori     4,4,0xF7F8");
asm ("    lwz     5,0(4)");
asm ("    cmpwi   5,0");
asm ("    beq     end");
asm ("    bl      main");
asm ("end:");
asm ("    lmw     26,0x0C(1)");
asm ("    lwz     3,0x08(1)");
asm ("    lwz     0,0x2C(1)");
asm ("    mtlr    0");
asm ("    addi    1,1,0x30");
asm ("    blr");

u32 cond_skip_codes(u32 address,u8 subtype)
{
    u32 x,oad;
    switch (subtype)
    {
      case 0:
        return 16;
      case 1:
        return 24;
      case 2:
        oad = address;
        for (x = address; true; x += 8)
            if ((*(u32*)x == 0) && (*(u32*)(x + 4) == 0x40000000)) break;
        return x - oad + 8;
      case 3:
        CONDITION_REGISTER = address;
        return 0;
    }
}

////////////////////////////////////////////////////////////////////////////////

void ecodes(u32 address)
{
    u32 add = 0;
    for (;;)
    {
        add = ecode(address);
        if (add == 0) break;
        else address = address + add;
    }
}

u32 ecode(u32 param)
{
    u32 x;
    u32 arcode[2];
    u32 opcode,dest,val32;
    u8 ztype,zsize,val8;
    u16 val16;
    s32 value;
    u32* address = (u32*)param;
    arcode[0] = address[0];
    arcode[1] = address[1];
    address = (u32*)((u32)address + 8);
    u32 address4;
    if (arcode[0] == 0)
    {
        ztype = (arcode[1] >> 29) & 7;
        switch (ztype)
        {
          case 0:
            return 0;
          case 1:
            if (arcode[1] & 0x02000000)
            {
                u32 times_loop = (arcode[1] & 0xFF) << 16;
                for (x = 0; x < times_loop; x++) { }
                return 8;
            } else {
                u32 address2;
                arcode[1] = address[1] & 0x01FFFFFF;
                arcode[1] = address[1] | 0x80000000;
                address2 = arcode[1];
                address = (u32*)((u32)address + 8);
                arcode[0] = address[0];
                arcode[1] = address[1];
                arcode[1]++;
                for (x = 0; x < arcode[1]; x++) *(u8*)arcode[0] = address2;
            }
            return 16;
          case 2:
            CONDITION_REGISTER = 0;
            return 8;
          case 4:
            zsize = (arcode[1] >> 25) & 3;
            address4 = ((arcode[1] & 0x01FFFFFF) | 0x80000000);   // 00000000 841F25C4 
            arcode[0] = address[0];                               // 05050505 00280031 
            arcode[1] = address[1];                               // 00000000 841F25C8 
            address = (u32*)((u32)address + 8);                   // 05050505 00280031 

            s8 val_inc; 
            u8 num_write;
            s16 addr_inc;
            val_inc = ((arcode[1] >> 24) & 0xFF);
            num_write = ((arcode[1] >> 16) & 0xFF);
            addr_inc = (arcode[1] & 0xFFFF);

            switch (zsize)
            {
              case SIZE_BYTE:
                val8 = (arcode[0] & 0xFF);
                for (x = 0; x < num_write; x++)
                {
                    *(u8*)(address4) = val8;
                    address4 += addr_inc;
                }
                return 16;
              case SIZE_HALFWORD:
                val16 = (arcode[0] & 0xFFFF);
                for (x = 0; x < num_write; x++)
                {
                    *(u16*)(address4) = val16;
                    address4 += addr_inc;
                }
                return 16;
              case SIZE_WORD:
                val32 = arcode[0];
                for (x = 0; x < num_write; x++)
                {
                    *(u32*)(address4) = val32;
                    address4 += addr_inc;
                }
                return 16;
              case SIZE_EXTENDED:
                if (val_inc != 0) arcode[1] = *(u32*)(arcode[1]);
                for (x = 0; x < addr_inc; x++)
                {
                    *(u8*)(address4) = *(u8*)(arcode[1]);
                }
                return 16;
            }
            return 16;
          case 5:
            break;
          case 6:
            break;
          case 7:
            // 00000000 ELLLLLLL 
            // SSSSSSSS DDDDDDDD 
            // L = amount of data to be copied, in bytes 
            // S = source address 
            // D = destination address 
            val32 = arcode[1] & 0x0000FFFF;
            arcode[0] = address[0];
            arcode[1] = address[1];
            for (x = 0; x < val32; x++) ((u8*)(arcode[1]))[x] = ((u8*)(arcode[0]))[x];
            address = (u32*)((u32)address + 8);
            return 16;
        }
        return 8;
    }
    u8 type,subtype,size;
    type    = (arcode[0] >> 27) & 7;
    subtype = (arcode[0] >> 30) & 3;
    size    = (arcode[0] >> 25) & 3;
    arcode[0] = arcode[0] & 0x01FFFFFF;
    arcode[0] = arcode[0] | 0x80000000;
    switch (type)
    {
      case CODE_TYPE_ZERO:
        switch (subtype)
        {
          case 0:
            switch (size)
            {
              case SIZE_BYTE:
                for (x = 0; x < ((arcode[1] >> 8) & 0x00FFFFFF); x++) ((u8*)(arcode[0]))[x] = (u8)(arcode[1] & 0xFF);
                break;
              case SIZE_HALFWORD:
                for (x = 0; x < ((arcode[1] >> 16) & 0x0000FFFF); x++) ((u16*)(arcode[0]))[x] = (u16)(arcode[1] & 0xFFFF);
                break;
              case SIZE_WORD:
                *(u32*)(arcode[0]) = arcode[1];
                break;
            }
            break;
          case 1:
            if ((*(u32*)(arcode[0]) >= 0x80000000) && (*(u32*)(arcode[0]) <= 0x81800000))
            {
                switch (size)
                {
                  case SIZE_BYTE:
                    *(u8*)(*(u32*)(arcode[0]) + ((arcode[1] >> 8) & 0x00FFFFFF)) = (arcode[1] & 0xFF);
                    break;
                  case SIZE_HALFWORD:
                    *(u16*)(*(u32*)(arcode[0]) + ((arcode[1] >> 16) & 0x0000FFFF)) = (arcode[1] & 0xFFFF);
                    break;
                  case SIZE_WORD:
                    *(u32*)(*(u32*)(arcode[0])) = arcode[1];
                    break;
                }
            }
            break;
          case 2:
            switch (size)
            {
              case SIZE_BYTE:
                *(u8*)(arcode[0]) += arcode[1];
                break;
              case SIZE_HALFWORD:
                *(u16*)(arcode[0]) += arcode[1];
                break;
              case SIZE_WORD:
                *(u32*)(arcode[0]) += arcode[1];
                break;
            }
            break;
          case 3:
            switch (size)
            {
              case 2:
                dest = 0x817FD000;
                value = dest - arcode[0];
                value = value & 0x03FFFFFC;
                value = value | 0x48000000;
                *(u32*)(arcode[0]) = value;
                break;
              case 3:
                arcode[0] |= 0xCC000000;
                if (arcode[0] & 0x01000000) *(u32*)(arcode[0]) = arcode[1];
                else *(u16*)(arcode[0]) = (arcode[1] & 0xFFFF);
                break;
            }
            break;
        }
        break;
      case CODE_TYPE_IF_EQUAL:
        switch (size)
        {
          case SIZE_BYTE:
            if (!(*(u8*)(arcode[0]) == (u8)(arcode[1] & 0xFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (!(*(u16*)(arcode[0]) == (u16)(arcode[1] & 0xFFFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (!(*(u32*)(arcode[0]) == (u32)arcode[1])) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_NOT_EQUAL:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(arcode[0]) == (arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(u16*)(arcode[0]) == (arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(u32*)(arcode[0]) == arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_LESS_SIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(s8*)(arcode[0]) >= (s32)(arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(s16*)(arcode[0]) >= (s32)(arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(s32*)(arcode[0]) >= (s32)arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_GREATER_SIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(s8*)(arcode[0]) <= (s32)(arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(s16*)(arcode[0]) <= (s32)(arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(s32*)(arcode[0]) <= (s32)arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_LESS_UNSIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(arcode[0]) >= (arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(u16*)(arcode[0]) >= (arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(u32*)(arcode[0]) >= arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_GREATER_UNSIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(arcode[0]) <= (arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(u16*)(arcode[0]) <= (arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(u32*)(arcode[0]) <= arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_AND:
        switch (size)
        {
          case SIZE_BYTE:
            if (!(*(u8*)(arcode[0]) & (u8)(arcode[1] & 0xFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (!(*(u16*)(arcode[0]) & (u8)(arcode[1] & 0xFFFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (!(*(u32*)(arcode[0]) & arcode[1])) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
    }
    return 8;
}

////////////////////////////////////////////////////////////////////////////////

// All the conditional code types are exactly the same. No other code types are 
// supported except these: 
// 00XXXXXX 00000000 8-bit read 
// 02XXXXXX 00000000 16-bit read 
// 04XXXXXX 00000000 32-bit read 
// 40XXXXXX 00YYYYYY 8-bit pointer read 
// 42XXXXXX 0000YYYY 16-bit pointer read 
// 44XXXXXX 00000000 32-bit pointer read 

void rcodes(u32 address)
{
    u32 add = 0;
    for (;;)
    {
        add = rcode(address);
        if (add == 0) break;
        else address = address + add;
    }
}

u32 rcode(u32 param)
{
    u32 x;
    u32 opcode,dest,val32;
    u8 ztype,zsize,val8;
    u16 val16;
    s32 value;
    u32* arcode = (u32*)param;
    u32 address4;
    if (arcode[0] == 0)
    {
        ztype = (arcode[1] >> 29) & 7;
        switch (ztype)
        {
          case 0:
            return 0;
          case 2:
            return 8;
          case 1:
          case 4:
            return 16;
          case 5:
          case 6:
          case 7:
            break;
        }
        return 8;
    }
    u8 type,subtype,size;
    u32 new_arcode0 = arcode[0];
    type    = (new_arcode0 >> 27) & 7;
    subtype = (new_arcode0 >> 30) & 3;
    size    = (new_arcode0 >> 25) & 3;
    new_arcode0 = new_arcode0 & 0x01FFFFFF;
    new_arcode0 = new_arcode0 | 0x80000000;
    switch (type)
    {
      case CODE_TYPE_ZERO:
        if (arcode[1] != 0) *(u32*)0xCC003024 = 0;
        switch (subtype)
        {
          case 0:
            switch (size)
            {
              case SIZE_BYTE: // 00XXXXXX 00000000 
                arcode[1] = (*(u8*)(new_arcode0) & 0xFF);
                break;
              case SIZE_HALFWORD: // 02XXXXXX 00000000 
                arcode[1] = (*(u16*)(new_arcode0) & 0xFFFF);
                break;
              case SIZE_WORD: // 04XXXXXX 00000000 
                arcode[1] = *(u32*)(new_arcode0);
                break;
            }
            break;
          case 1:
            if ((*(u32*)(new_arcode0) >= 0x80000000) && (*(u32*)(new_arcode0) <= 0x81800000))
            {
                switch (size)
                {
                  case SIZE_BYTE: // 40XXXXXX 00YYYYYY 
                    arcode[1] = *(u8*)(new_arcode0 + arcode[1]);
                    break;
                  case SIZE_HALFWORD: // 42XXXXXX 0000YYYY 
                    arcode[1] = *(u16*)(new_arcode0 + arcode[1]);
                    break;
                  case SIZE_WORD: // 44XXXXXX 00000000 
                    arcode[1] = *(u32*)(new_arcode0);
                    break;
                }
            }
            break;
        }
        break;
      case CODE_TYPE_IF_EQUAL:
        switch (size)
        {
          case SIZE_BYTE:
            if (!(*(u8*)(new_arcode0) == (u8)(arcode[1] & 0xFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (!(*(u16*)(new_arcode0) == (u16)(arcode[1] & 0xFFFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (!(*(u32*)(new_arcode0) == (u32)arcode[1])) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_NOT_EQUAL:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(new_arcode0) == (arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(u16*)(new_arcode0) == (arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(u32*)(new_arcode0) == arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_LESS_SIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(s8*)(new_arcode0) >= (s32)(arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(s16*)(new_arcode0) >= (s32)(arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(s32*)(new_arcode0) >= (s32)arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_GREATER_SIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(s8*)(new_arcode0) <= (s32)(arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(s16*)(new_arcode0) <= (s32)(arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(s32*)(new_arcode0) <= (s32)arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_LESS_UNSIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(new_arcode0) >= (arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(u16*)(new_arcode0) >= (arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(u32*)(new_arcode0) >= arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_GREATER_UNSIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(new_arcode0) <= (arcode[1] & 0xFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (*(u16*)(new_arcode0) <= (arcode[1] & 0xFFFF)) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (*(u32*)(new_arcode0) <= arcode[1]) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
      case CODE_TYPE_IF_AND:
        switch (size)
        {
          case SIZE_BYTE:
            if (!(*(u8*)(new_arcode0) & (u8)(arcode[1] & 0xFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_HALFWORD:
            if (!(*(u16*)(new_arcode0) & (u8)(arcode[1] & 0xFFFF))) return cond_skip_codes(param,subtype);
            else return 8;
          case SIZE_WORD:
            if (!(*(u32*)(new_arcode0) & arcode[1])) return cond_skip_codes(param,subtype);
            else return 8;
        }
        break;
    }
    return 8;
}

