.globl _start
_start:
    ori    12,3,0
    lis    1,0x8170

    # clear memory before buffer
    lis    3,0x8000
    ori    3,3,0x3000
    li     4,0
    sub    5,12,3
    bl     memset

    # clear memory after buffer
    mr     3,12
    bl     dolsize
    add    3,3,12
    lis    6,0x8180
    sub    5,6,3
    li     4,0
    bl     memset

    # relocate the sections....
    li     6,0
  reloc_section:
    mulli  7,6,4
    add    7,7,12
    lwz    4,0(7)
    cmpwi  4,0
    beq    reloc_section_skip
    add    4,4,12    # section file position
    lwz    3,0x48(7) # section mem addr
    lwz    5,0x90(7) # section size
    bl     memmove
    lwz    3,0x48(7) # section mem addr
    lwz    4,0x90(7) # section size
    bl     flush_code
  reloc_section_skip:
    addi   6,6,1
    cmpwi  6,18
    blt    reloc_section

    # clear the BSS
    lwz    3,0xD8(12)
    cmpwi  3,0
    beq    skip_bss
    li     4,0
    lwz    5,0xDC(12)
    bl     memset
    lwz    3,0xD8(12)
    lwz    4,0xDC(12)
    bl     flush_code
  skip_bss:

    # launch the DOL file!
    lwz    3,0xE0(12)
    mtlr   3
    blr

memmove:
    cmpw   3,4
    blt    memmove_reverse
    subi   3,3,1
    subi   4,4,1
  memmove_forward_again:
    lbzu   0,1(4)
    stbu   0,1(3)
    subi   5,5,1
    cmpwi  5,0
    bne    memmove_forward_again
    blr

  memmove_reverse:
    addi   3,3,1
    addi   4,4,1
    add    3,3,5
    add    3,3,5
  memmove_reverse_again:
    lbzu   0,-1(4)
    stbu   0,-1(3)
    subi   5,5,1
    cmpwi  5,0
    bne    memmove_reverse_again
    blr

memset:
    subi   3,3,1
  memset_again:
    stbu   4,1(3)
    subi   5,5,1
    cmpwi  5,0
    bne    memset_again
    blr

dolsize:
    li     6,0
    li     7,0
  dolsize_section:
    mulli  5,6,4
    lwzx   4,5,3
    mulli  5,7,4
    lwzx   5,5,3
    cmpwi  4,5
    ble    dolsize_skip_section
    mr     7,6
  dolsize_skip_section:
    addi   6,6,1
    cmpwi  6,18
    blt    dolsize_section
    mulli  5,7,4
    add    5,5,3
    lwz    4,0(5)
    lwz    3,0x90(5)
    add    3,3,4
    blr

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
