.globl dcache_flush
dcache_flush:
    cmplwi   4,0
    blelr
    clrlwi.  5,3,27
    beq      dcf_1
    addi     4,4,0x20 
dcf_1:
    addi     4,4,0x1f
    srwi     4,4,5
    mtctr    4
dcf_2:
    dcbst    0,3
    addi     3,3,0x20
    bdnz     dcf_2
    blr

#ifndef __IS_HOOK

.globl dcache_inv
dcache_inv:
    cmplwi  4,0
    blelr
    clrlwi. 5,3,27
    beq     dci_1
    addi    4,4,0x20 
dci_1:
    addi    4,4,0x1f
    srwi    4,4,5
    mtctr   4
dci_2:
    dcbi    0,3
    addi    3,3,0x20
    bdnz    dci_2
    blr

.globl flush_code
flush_code:
    lis    5,0xFFFF
    ori    5,5,0xFFF1
    and    5,5,3
    subf   3,5,3
    add    4,4,3
fc_1:
    dcbst  0,5
    sync
    icbi   0,5
    addic  5,5,8
    subic. 4,4,8
    bge    fc_1
    isync
    blr

.globl checksum
checksum:
    li     5,0
    li     6,0
  checksum_again:
    lwzx   7,3,6
    xor    5,5,7
    addi   6,6,4
    cmpw   6,4
    blt    checksum_again
    mr     3,5
    blr

#   u32 x,cs = 0;
#   for (x = 0; x < size; x += 4) cs ^= *(u32*)((u32)data + x);
#   return cs;

################################################################################

.globl GetMSR
GetMSR:
    mfmsr 3
    blr

.globl SetMSR
SetMSR:
    mtmsr 3
    blr

.globl gettime_s
gettime_s:
    mftbu   3
    mftb    4
    mftbu   5
    cmpw    3,5
    bne     gettime_s
    blr

.globl addressfetch
addressfetch:
    mflr 3
    blr

.globl mftbl
mftbl:
    mftb 3
    blr

.globl mftbu
mftbu:
    mftbu 3
    blr

.globl reset
reset:
    li    3,0
    li    4,3
    lis   5,0xCC00
    sth   3,0x2000(5)
    stw   3,0x3024(5)
    b     0

.globl debug_hook
debug_hook:
    nop
    nop
    blr

# endian functions

.globl byteswapl
byteswapl:
    subi    4,1,4
    stw     3,0(4)
    lwbrx   3,0,4
    blr

.globl byteswaps
byteswaps:
    subi    4,1,4
    sth     3,0(4)
    lhbrx   3,0,4
    blr

.globl bsReadWord
bsReadWord:
    lwbrx   3,0,3
    blr

.globl bsReadShort
bsReadShort:
    lhbrx   3,0,3
    blr

.globl bsWriteWord
bsWriteWord:
    stwbrx   4,0,3
    blr

.globl bsWriteShort
bsWriteShort:
    sthbrx   4,0,3
    blr

#endif

