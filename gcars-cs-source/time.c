#define FUZZIQER_TYPES
#include "types.h"
#include "time.h"

#define mftb(rval) ({unsigned long u; do { \
     asm volatile ("mftbu %0" : "=r" (u)); \
     asm volatile ("mftb %0" : "=r" ((rval)->l)); \
     asm volatile ("mftbu %0" : "=r" ((rval)->u)); \
     } while(u != ((rval)->u)); })

#define TB_CLOCK  40500000

unsigned long tb_diff_sec(tb_t *end, tb_t *start)
{
    unsigned long upper, lower;
    upper = end->u - start->u;
    if (start->l > end->l) upper--;
    lower = end->l - start->l;
    return ((upper*((unsigned long)0x80000000/(TB_CLOCK/2))) + (lower/ TB_CLOCK));
}

unsigned long tb_diff_msec(tb_t *end, tb_t *start)
{
    unsigned long upper, lower;
    upper = end->u - start->u;
    if (start->l > end->l) upper--;
    lower = end->l - start->l;
    return ((upper*((unsigned long)0x80000000/(TB_CLOCK/2000))) + (lower/(TB_CLOCK/1000)));
}

unsigned long tb_diff_usec(tb_t *end, tb_t *start)
{
    unsigned long upper, lower;
    upper = end->u - start->u;
    if (start->l > end->l) upper--;
    lower = end->l - start->l;
    return ((upper*((unsigned long)0x80000000/(TB_CLOCK/2000000))) + (lower/(TB_CLOCK/1000000)));
}

void udelay(unsigned int us)
{
    tb_t start, end;
    mftb(&start);
    for (;;)
    {
        mftb(&end);
        if (tb_diff_usec(&end, &start) >= us) break;
    }
}

void mdelay(unsigned int ms)
{
    tb_t start, end;
    mftb(&start);
    for (;;)
    {
        mftb(&end);
        if (tb_diff_msec(&end, &start) >= ms) break;
    }
}

void delay(unsigned int s)
{
    tb_t start, end;
    mftb(&start);
    for (;;)
    {
        mftb(&end);
        if (tb_diff_sec(&end, &start) >= s) break;
    }
}

