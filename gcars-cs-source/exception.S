.globl exception_start, exception_end, exception_patch

.set sprg0,272

exception_start:
    mtspr    sprg0, 4
    lwz      4,0xC0(0)
    stw      3,12(4)
    mfspr    3,sprg0
    stw      3,16(4)
    stw      5,20(4)
    lhz      3,418(4)
    ori      3,3,0x0002
    sth      3,418(4)
    mfcr     3
    stw      3,128(4)
    mflr     3
    stw      3,132(4)
    mfctr    3
    stw      3,136(4)
    mfxer    3
    stw      3,140(4)
    mfsrr0   3
    stw      3,408(4)
    mfsrr1   3
    stw      3,412(4)
    mr       5,3
    nop
    mfmsr    3
    ori      3,3,0x2030
    mtsrr1   3
exception_patch:
    li       3,0
    lwz      4,0xD4(0)
    rlwinm.  5,5,0,30,30
    lis      5,exception_handler_transfer@h
    ori      5,5,exception_handler_transfer@l
    mtsrr0   5
    bne      finish2
    lis      5,exception_handler_default@h
    ori      5,5,exception_handler_default@l
    rfi
finish2:
    clrlslwi 5,3,24,2
    lwz      5,0x3000(5)
    rfi

exception_end:

exception_handler_transfer:
    mtlr     5
    stw      0,0(4);
    stw      1,4(4);
    stw      2,8(4);
    stmw     6,24(4);
    mfspr    0,913;
    stw      0,424(4);
    mfspr    0,914;
    stw      0,428(4);
    mfspr    0,915;
    stw      0,432(4);
    mfspr    0,916;
    stw      0,436(4);
    mfspr    0,917;
    stw      0,440(4);
    mfspr    0,918;
    stw      0,444(4);
    mfspr    0,919;
    stw      0,448(4);
    mfdsisr  5;
    mfdar    6
    mr     14,4
    blrl
    mr     3,14

exception_context_load:
    lwz      0,0(3)
    lwz      1,4(3)
    lwz      2,8(3)
    lhz      4,418(3)
    rlwinm.  5,4,0,30,30
    beq      1f
    rlwinm   4,4,0,31,29
    sth      4,418(3)
    lmw      5,20(3)
    b 2f
1:
    lmw      13,52(3)
2:
    lwz      4,424(3)
    mtspr    913,4
    lwz      4,428(3)
    mtspr    914,4
    lwz      4,432(3)
    mtspr    915,4
    lwz      4,436(3)
    mtspr    916,4
    lwz      4,440(3)
    mtspr    917,4
    lwz      4,444(3)
    mtspr    918,4
    lwz      4,448(3)
    mtspr    919,4
    lwz      4,128(3)
    mtcrf    0xff,4
    lwz      4,132(3)
    mtlr     4
    lwz      4,136(3)
    mtctr    4
    lwz      4,140(3)
    mtxer    4
    mfmsr    4
    rlwinm   4,4,0,17,15
    rlwinm   4,4,0,31,29
    mtmsr    4
    lwz      4,408(3)
    mtsrr0   4
    lwz      4,412(3)
    mtsrr1   4
    lwz      4,16(3)
    lwz      3,12(3)
    rfi

.global decCount
decCount:
    .long 0

.global exception_handler_decrementer

exception_handler_decrementer:
    lis    3,decCount@h
    ori    3,3,decCount@l
    lwz    5,0(3)
    addi   5,5,1
    stw    5,0(3)
    lis    3,0x0269
    ori    3,3,0xFB20
    mtdec  3
    blr

.global exception_handler_system_call

exception_handler_system_call:
    mfspr  3,1008
    ori    4,3,8
    mtspr  1008,4
    isync
    sync
    mtspr  1008,3
    blr

