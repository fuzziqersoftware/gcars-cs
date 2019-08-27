#include "hook.h"

asm (".globl _start");
asm ("_start:"); // global calls 
asm ("    b       hooked");
asm ("    b       ecodes");
asm ("    b       rcodes");

asm ("hooked:"); // stacking 
asm ("    stwu    1,-0x90(1)");
asm ("    stmw    2,0x10(1)");
asm ("    stw     0,0x0C(1)");
asm ("    mflr    0");
asm ("    stw     0,0x08(1)");

asm ("  paused:"); // GCARS codes 
asm ("    lis     3,0x817F");
asm ("    ori     3,3,0xF800");
asm ("    bl      ecodes");

asm ("    lis     6,0x817F"); // Control Simulator, if enabled 
asm ("    ori     6,6,0xF7F8");
asm ("    lwz     5,0(6)");
asm ("    cmpwi   5,0");
asm ("    bnel    controlsim");

asm ("    lis     6,0x817F"); // check if paused 
asm ("    ori     6,6,0xF7F4");
asm ("    lwz     5,0(6)");
asm ("    cmpwi   5,0");
asm ("    beq     ret");

asm ("    bl      vsync"); // if paused, do vsync and decrement pause counter 
asm ("    cmpwi   5,-1");
asm ("    beq     paused");
asm ("    subi    5,5,1");
asm ("    stw     5,0(6)");
asm ("    b       paused");

asm ("  ret:"); // stacking 
asm ("    lwz     0,0x08(1)");
asm ("    mtlr    0");
asm ("    lwz     0,0x0C(1)");
asm ("    lmw     2,0x10(1)");
asm ("    addi    1,1,0x90");
asm ("    blr");

asm ("vsync:"); // video halfline waiting.... 
asm ("    lis    4,0xCC00");
asm ("  vsync_loop1:");
asm ("    lhz    3,0x202C(4)");
asm ("    cmpwi  3,200");
asm ("    bge    vsync_loop1");
asm ("  vsync_loop2:");
asm ("    lhz    3,0x202C(4)");
asm ("    cmpwi  3,200");
asm ("    blt    vsync_loop2");
asm ("    blr");

////////////////////////////////////////////////////////////////////////////////
// A few needed functions.... //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

asm (".globl dcache_flush");
asm ("dcache_flush:");
asm ("    cmplwi   4,0");
asm ("    blelr");
asm ("    clrlwi.  5,3,27");
asm ("    beq      dcf_1");
asm ("    addi     4,4,0x20");
asm ("dcf_1:");
asm ("    addi     4,4,0x1f");
asm ("    srwi     4,4,5");
asm ("    mtctr    4");
asm ("dcf_2:");
asm ("    dcbst    0,3");
asm ("    addi     3,3,0x20");
asm ("    bdnz     dcf_2");
asm ("    blr");

asm (".globl dcache_inv");
asm ("dcache_inv:");
asm ("    cmplwi  4,0");
asm ("    blelr");
asm ("    clrlwi. 5,3,27");
asm ("    beq     dci_1");
asm ("    addi    4,4,0x20");
asm ("dci_1:");
asm ("    addi    4,4,0x1f");
asm ("    srwi    4,4,5");
asm ("    mtctr   4");
asm ("dci_2:");
asm ("    dcbi    0,3");
asm ("    addi    3,3,0x20");
asm ("    bdnz    dci_2");
asm ("    blr");

asm (".globl flush_code");
asm ("flush_code:");
asm ("    lis    5,0xFFFF");
asm ("    ori    5,5,0xFFF1");
asm ("    and    5,5,3");
asm ("    subf   3,5,3");
asm ("    add    4,4,3");
asm ("fc_1:");
asm ("    dcbst  0,5");
asm ("    sync");
asm ("    icbi   0,5");
asm ("    addic  5,5,8");
asm ("    subic. 4,4,8");
asm ("    bge    fc_1");
asm ("    isync");
asm ("    blr");

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
 * C4 hooks with hook types 1, 2, or 3 
 * T1-7 ST 3 
 * Kenobi's OR and AND codes */ 

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
        for (x = address; ; x += 8) if ((*(u32*)x == 0) && (*(u32*)(x + 4) == 0x40000000)) break;
        return x - oad + 8;
      case 3:
        //GCARS_CONDITIONAL = address;
        return 0;
    }
}

u32 cond_parse_code(u32 codeaddr,u8 type,u8 subtype,u8 size,u32 address,u32 value)
{
    switch (type)
    {
      case CODE_TYPE_IF_EQUAL:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(address) == (u8)(value & 0xFF)) return 8;
            break;
          case SIZE_HALFWORD:
            if (*(u16*)(address) == (u16)(value & 0xFFFF)) return 8;
            break;
          case SIZE_WORD:
            if (*(u32*)(address) == (u32)value) return 8;
        }
        return cond_skip_codes(codeaddr,subtype);
      case CODE_TYPE_IF_NOT_EQUAL:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(address) != (value & 0xFF)) return 8;
            break;
          case SIZE_HALFWORD:
            if (*(u16*)(address) != (value & 0xFFFF)) return 8;
            break;
          case SIZE_WORD:
            if (*(u32*)(address) != value) return 8;
        }
        return cond_skip_codes(codeaddr,subtype);
      case CODE_TYPE_IF_LESS_SIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(s8*)(address) < (s8)(value & 0xFF)) return 8;
            break;
          case SIZE_HALFWORD:
            if (*(s16*)(address) < (s16)(value & 0xFFFF)) return 8;
            break;
          case SIZE_WORD:
            if (*(s32*)(address) < (s32)value) return 8;
        }
        return cond_skip_codes(codeaddr,subtype);
      case CODE_TYPE_IF_GREATER_SIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(s8*)(address) > (s8)(value & 0xFF)) return 8;
            break;
          case SIZE_HALFWORD:
            if (*(s16*)(address) > (s16)(value & 0xFFFF)) return 8;
            break;
          case SIZE_WORD:
            if (*(s32*)(address) > (s32)value) return 8;
        }
        return cond_skip_codes(codeaddr,subtype);
      case CODE_TYPE_IF_LESS_UNSIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(address) < (value & 0xFF)) return 8;
            break;
          case SIZE_HALFWORD:
            if (*(u16*)(address) < (value & 0xFFFF)) return 8;
            break;
          case SIZE_WORD:
            if (*(u32*)(address) < value) return 8;
        }
        return cond_skip_codes(codeaddr,subtype);
      case CODE_TYPE_IF_GREATER_UNSIGNED:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(address) > (value & 0xFF)) return 8;
            break;
          case SIZE_HALFWORD:
            if (*(u16*)(address) > (value & 0xFFFF)) return 8;
            break;
          case SIZE_WORD:
            if (*(u32*)(address) > value) return 8;
        }
        return cond_skip_codes(codeaddr,subtype);
      case CODE_TYPE_IF_AND:
        switch (size)
        {
          case SIZE_BYTE:
            if (*(u8*)(address) & (u8)(value & 0xFF)) return 8;
            break;
          case SIZE_HALFWORD:
            if (*(u16*)(address) & (u16)(value & 0xFFFF)) return 8;
            break;
          case SIZE_WORD:
            if (*(u32*)(address) & value) return 8;
        }
        return cond_skip_codes(codeaddr,subtype);
    }
    return 8;
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
    u32 r3,r4;
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
                arcode[1] = ((address[1] & 0x01FFFFFF) | 0x80000000);
                address2 = arcode[1];
                address = (u32*)((u32)address + 8);
                arcode[0] = address[0];
                arcode[1] = address[1];
                arcode[1]++;
                for (x = 0; x < arcode[1]; x++) *(u8*)arcode[0] = address2;
            }
            return 16;
          case 2:
            GCARS_CONDITIONAL = 0;
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
            arcode[1] = ((address[1] & 0x01FFFFFF) | 0x80000000);
            address = (u32*)((u32)address + 8);
            r3 = address[0];
            r4 = address[1];
            ((void (*)(u32,u32))arcode[1])(r3,r4);
            return 16;
          case 6:
            if (!GCARS_ENABLE_CS) break;
            if (address[0] & 3)
            {
                if (address[0] & 1) *(u8*)((address[1] & 0x01FFFFFF) | 0x80000000) = CS_DATA->localPad;
                else *(u16*)((address[1] & 0x01FFFFFF) | 0x80000000) = CS_DATA->localPad;
            } else *(u32*)((address[1] & 0x01FFFFFF) | 0x80000000) = CS_DATA->localPad;
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
                for (x = 0; x < (((arcode[1] >> 8) & 0x00FFFFFF) + 1); x++) ((u8*)(arcode[0]))[x] = (u8)(arcode[1] & 0xFF);
                break;
              case SIZE_HALFWORD:
                for (x = 0; x < (((arcode[1] >> 16) & 0x0000FFFF) + 1); x++) ((u16*)(arcode[0]))[x] = (u16)(arcode[1] & 0xFFFF);
                break;
              case SIZE_WORD:
                *(u32*)(arcode[0]) = arcode[1];
                break;
            }
            break;
          case 1:
            opcode = *(u32*)(arcode[0]);
            if ((opcode >= 0x80000000) && (opcode < 0x81800000))
            {
                switch (size)
                {
                  case SIZE_BYTE:
                    *(u8*)(opcode + ((arcode[1] >> 8) & 0x00FFFFFF)) = (arcode[1] & 0xFF);
                    break;
                  case SIZE_HALFWORD:
                    *(u16*)(opcode + ((arcode[1] >> 15) & 0x0001FFFE)) = (arcode[1] & 0xFFFF);
                    break;
                  case SIZE_WORD:
                    *(u32*)(opcode) = arcode[1];
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
                dest = (u32)(ENTRYPOINT);
                value = dest - arcode[0];
                value = value & 0x03FFFFFC;
                value = value | 0x48000000;
                *(u32*)(arcode[0]) = value;
                break;
              case 3:
                arcode[0] |= 0xCC000000;
                if (arcode[0] & 0x01000000) *(u32*)(arcode[0] & 0xCC00FFFF) = arcode[1];
                else *(u16*)(arcode[0]) = (arcode[1] & 0xFFFF);
                break;
            }
            break;
        }
        break;
      default:
        return cond_parse_code(param,type,subtype,size,arcode[0],arcode[1]);
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

void rcodes(u32 address,u32 dest)
{
    u32 add = 0;
    if (dest)
    {
        for (;;)
        {
            *(u32*)dest = *(u32*)address;
            *(u32*)(dest + 4) = *(u32*)(address + 4);
            add = rcode(dest);
            if (add == 0) break;
            if (add == 8) dest += add;
            address += add;
        }
    } else {
        for (;;)
        {
            add = rcode(address);
            if (add == 0) break;
            address += add;
        }
    }
}

u32 rcode(u32 param)
{
    u32 x;
    u32 opcode,val32;
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
          case 5:
          case 6:
          case 7:
            return 8;
          case 1:
          case 4:
            return 16;
        }
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
        //if (arcode[1] != 0) reset();
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
            opcode = *(u32*)(new_arcode0);
            if ((opcode >= 0x80000000) && (opcode <= 0x81800000))
            {
                switch (size)
                {
                  case SIZE_BYTE: // 40XXXXXX YYYYYY00 
                    arcode[1] |= *(u8*)(opcode + ((arcode[1] >> 8) & 0x00FFFFFF));
                    break;
                  case SIZE_HALFWORD: // 42XXXXXX YYYY0000 
                    arcode[1] |= *(u16*)(opcode + ((arcode[1] >> 15) & 0x0001FFFE));
                    break;
                  case SIZE_WORD: // 44XXXXXX 00000000 
                    arcode[1] |= *(u32*)(opcode);
                    break;
                }
            }
            break;
        }
        break;
      default:
        return cond_parse_code(param,type,subtype,size,new_arcode0,arcode[1]);
    }
    return 8;
}

