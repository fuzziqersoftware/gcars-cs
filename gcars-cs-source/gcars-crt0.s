.text
.org 0x0
.globl _start
.globl CacheInit

_start:
    ori      3,3,0
	bl       InitGPRS # Initialize the General Purpose Registers
	bl       InitHardware # Initialize the GameCube Hardware (Floating Point Registers, Caches, etc.) 
	bl       SystemInit # Initialize more cache aspects, clear a few SPR's, and disable interrupts.

.extern __bss_start, _end

	# Clear the BSS section!
	#lis    3,__bss_start@h
	#ori    3,3,__bss_start@l
	#li     4,0
	#lis    5,_end@h
	#ori    5,5,_end@l
	#sub    5,5,3
	#subi   3,3,4
	#mtctr  5
#BSSCLoop:
	#stwu  4,4(3)
	#bdnz  BSSCLoop

	bl main
	bl reset

InitGPRS:
	# Clear all of the GPR's to 0
	li       0,0
	li       3,0
	li       4,0
	li       5,0
	li       6,0
	li       7,0
	li       8,0
	li       9,0
	li       10,0
	li       11,0
	li       12,0
	li       14,0
	li       15,0
	li       16,0
	li       17,0
	li       18,0
	li       19,0
	li       20,0
	li       21,0
	li       22,0
	li       23,0
	li       24,0
	li       25,0
	li       26,0
	li       27,0
	li       28,0
	li       29,0
	li       30,0
	li       31,0
	lis		 1, 0x8170
	lis      2,_SDA2_BASE_@h
	ori      2,2,_SDA2_BASE_@l # Set the Small Data 2 (Read Only) base register.
	lis      13,_SDA_BASE_@h
	ori      13,13,_SDA_BASE_@l # Set the Small Data (Read\Write) base register.	
	blr

InitHardware:
	mflr     31 # Store the link register in r31
	
	bl	   PSInit 	# Initialize Paired Singles
	bl       FPRInit 	# Initialize the FPR's
	bl       CacheInit 	# Initialize the system caches

	mtlr     31 # Retreive the link register from r31
	blr

PSInit:
	mfspr    3, 920 # (HID2)
	oris     3, 3, 0xA000
	mtspr    920, 3 # (HID2)

	# Set the Instruction Cache invalidation bit in HID0
	mfspr    3,1008
	ori      3,3,0x0800
	mtspr    1008,3

	sync

	# Clear various Special Purpose Registers
	li       3,0
	mtspr    912,3
	mtspr    913,3
	mtspr    914,3
	mtspr    915,3
	mtspr    916,3
	mtspr    917,3
	mtspr    918,3
	mtspr    919,3

	# Return 
	blr

FPRInit:
	# Enable the Floating Point Registers
	mfmsr    3
	ori      3,3,0x2000
	mtmsr    3
	
	# Clear all of the FPR's to 0
	lis	   3, zfloat@h
	ori	   3, 3, zfloat@l
	lfd	   0, 0(3)
	fmr      1,0
	fmr      2,0
	fmr      3,0
	fmr      4,0
	fmr      5,0
	fmr      6,0
	fmr      7,0
	fmr      8,0
	fmr      9,0
	fmr      10,0
	fmr      11,0
	fmr      12,0
	fmr      13,0
	fmr      14,0
	fmr      15,0
	fmr      16,0
	fmr      17,0
	fmr      18,0
	fmr      19,0
	fmr      20,0
	fmr      21,0
	fmr      22,0
	fmr      23,0
	fmr      24,0
	fmr      25,0
	fmr      26,0
	fmr      27,0
	fmr      28,0
	fmr      29,0
	fmr      30,0
	fmr      31,0
	mtfsf    255,0

	# Return
	blr

CacheInit:
	mflr     0
	stw      0, 4(1)
	stwu     1, -16(1)
	stw      31, 12(1)
	stw      30, 8(1)

	mfspr    3,1008 # (HID0)
	rlwinm   0, 3, 0, 16, 16
	cmplwi   0, 0x0000 # Check if the Instruction Cache has been enabled or not.
	bne      ICEnabled

	# If not, then enable it.
	isync
	mfspr    3, 1008
	ori      3, 3, 0x8000
	mtspr    1008, 3

ICEnabled:
	mfspr    3, 1008 # bl       PPCMfhid0
	rlwinm   0, 3, 0, 17, 17
	cmplwi   0, 0x0000 # Check if the Data Cache has been enabled or not.
	bne      DCEnabled
	
	# If not, then enable it.
	sync
	mfspr    3, 1008
	ori      3, 3, 0x4000
	mtspr    1008, 3
	
DCEnabled:
	
	mfspr    3, 1017 # (L2CR)
	clrrwi   0, 3, 31 # Clear all of the bits except 31
	cmplwi   0, 0x0000
	bne      L2GISkip # Skip the L2 Global Cache Invalidation process if it has already been done befor.

	# Store the current state of the MSR in r30
	mfmsr    3
	mr       30,3
	
	sync
	
	# Enable Instruction and Data Address Translation
	li       3, 48
	mtmsr    3

	sync
	sync

	# Disable the L2 Global Cache.
	mfspr    3, 1017 # (L2CR)
	clrlwi   3, 3, 1
	mtspr    1017, 3 # (L2CR)
	sync

	# Invalidate the L2 Global Cache.
	bl       L2GlobalInvalidate

	# Restore the previous state of the MSR from r30
	mr       3, 30
	mtmsr    3

	# Enable the L2 Global Cache and disable the L2 Data Only bit and the L2 Global Invalidate Bit.
	mfspr    3, 1017 # (L2CR)
	oris     0, 3, 0x8000
	rlwinm   3, 0, 0, 11, 9
	mtspr    1017, 3 # (L2CR)


L2GISkip:
	# Restore the non-volatile registers to their previous values and return.
	lwz      0, 20(1)
	lwz      31, 12(1)
	lwz      30, 8(1)
	addi     1, 1, 16
	mtlr     0
	blr

L2GlobalInvalidate:
	mflr     0
	stw      0, 4(1)
	stwu     1, -16(1)
	stw      31, 12(1)
	sync

	# Disable the L2 Cache.
	mfspr    3, 1017  # bl       PPCMf1017
	clrlwi   3, 3, 1
	mtspr    1017, 3 # bl       PPCMt1017

	sync

	# Initiate the L2 Cache Global Invalidation process.
	mfspr    3, 1017  # (L2CR)
	oris     3, 3, 0x0020
	mtspr    1017, 3 # (L2CR)

	# Wait until the L2 Cache Global Invalidation has been completed.
L2GICheckComplete:
	mfspr    3, 1017 # (L2CR)
	clrlwi   0, 3, 31
	cmplwi   0, 0x0000
	bne      L2GICheckComplete
	
	# Clear the L2 Data Only bit and the L2 Global Invalidate Bit.
	mfspr    3, 1017  # (L2CR)
	rlwinm   3, 3, 0, 11, 9
	mtspr    1017, 3 # (L2CR)

	# Wait until the L2 Cache Global Invalidation status bit signifies that it is ready.
L2GDICheckComplete:
	mfspr    3, 1017  # (L2CR)
	clrlwi   0, 3, 31
	cmplwi   0, 0x0000
	bne      L2GDICheckComplete

	# Restore the non-volatile registers to their previous values and return.
	lwz      0, 20(1)
	lwz      31, 12(1)
	addi     1, 1, 16
	mtlr     0
	blr

SystemInit:
	mflr    0
	stw     0, 4(1)
	stwu    1, -0x18(1)
	stw     31, 0x14(1)
	stw     30, 0x10(1)
	stw     29, 0xC(1)

	# Disable interrupts!
	mfmsr    3
	rlwinm   4,3,0,17,15
	mtmsr    4

	# Clear various SPR's
	li       3,0
	mtspr    952, 3
	mtspr    956, 3
	mtspr    953, 3
	mtspr    954, 3
	mtspr    957, 3
	mtspr    958, 3

	# Disable Speculative Bus Accesses to non-guarded space from both caches.
	mfspr    3, 1008 # (HID0)
	ori      3, 3, 0x0200
	mtspr    1008, 3

	# Set the Non-IEEE mode in the FPSCR
	mtfsb1   29
	
	mfspr    3,920 # (HID2)
	rlwinm   3, 3, 0, 2, 0
	mtspr    920,3 # (HID2)

	# Restore the non-volatile registers to their previous values and return.
	lwz     0, 0x1C(1)
	lwz     31, 0x14(1)
	lwz     30, 0x10(1)
	lwz     29, 0xC(1)
	addi    1, 1, 0x18
	mtlr    0
	blr

	.balign	8
zfloat:
	.double	0
	.balign	4
