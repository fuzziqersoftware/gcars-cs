#ifndef __context_h
#define __context_h

struct context_s
{
    unsigned long GPR[32];
    unsigned long CR, LR, CTR, XER;
    
    double FPR[32];
    unsigned long FPSCR_pad;
    unsigned long FPSCR;
    unsigned long SRR0;
    unsigned long SRR1;
    unsigned short mode;           // since UIMM is 16 bits in PPC
    unsigned short state;          // OR-ed OS_CONTEXT_STATE_*
    unsigned long GQR[8];
    unsigned long PSF_pad;
    double PSF[32];
};

#define CONTEXT_CURRENT          *(unsigned long*)(0x800000D4)
#define CONTEXT_CURRENT_PHYS     *(unsigned long*)(0x800000C0)

#endif
