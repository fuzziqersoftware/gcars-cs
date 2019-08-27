extern void dcache_flush(void*,int);

#ifndef __IS_HOOK

extern void dcache_inv(void*,int);
extern void flush_code(void*,int);
extern u32 checksum(void*,u32);

bool DisableInterrupts();
bool RestoreInterrupts(bool);
long long gettime_s();
void gettime_p(void*);
unsigned long GetMSR();
void SetMSR(unsigned long);
unsigned long addressfetch();
unsigned long mftbl();
unsigned long mftbu();
void reset();
void debug_hook();

u32 byteswapl(u32);
u16 byteswaps(u16);
u32 bsReadWord(void*);
u16 bsReadShort(void*);
void bsWriteWord(void*,u32);
void bsWriteShort(void*,u32);

#endif

