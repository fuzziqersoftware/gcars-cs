#ifndef __time_h
#define __time_h

typedef struct {
    u32 u;
    u32 l; } tb_t;

unsigned long tb_diff_sec(tb_t *end, tb_t *start);
unsigned long tb_diff_msec(tb_t *end, tb_t *start);
unsigned long tb_diff_usec(tb_t *end, tb_t *start);
void udelay(unsigned int us);
void mdelay(unsigned int ms);
void delay(unsigned int s);

#endif
