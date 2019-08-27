#include "gamecube.h"

u32 malloc_count;
u32 align = 0;
void* alloc_ptr = (void*)0x81200000;
ALLOC* allocs;
bool mdebug = false;

void setmdebug(bool d) { mdebug = d; }

bool setalign(u32 nalign) // make sure nalign has only one bit set 
{
    if (nalign == 0)
    {
        malloc_count = 1;
        setalign(0x00000020);
        allocs = (ALLOC*)0x81500000;
        allocs[0].data = allocs;
        allocs[0].size = 0;
    } else {
        int x,y = 0;
        for (x = 0; x < 32; x++) if (((nalign >> x) & 1) != 0) y++;
        if (y > 1) return 0;
        align = nalign;
    }
}

void calcallocptr()
{
    u32 x,ap = 0x81200000;
    for (x = 0; x < malloc_count; x++) if ((u32)allocs[x].data < ap) ap = (u32)allocs[x].data;
    alloc_ptr = (void*)ap;
}

void* malloc(u32 size)
{
    ALLOC alloc;
    u32 mbegin,mend;
    u32 tbegin,tend;
    int x,y;

    size = (size + (align - 1)) & ~(align - 1);
    if (mdebug) GCARSErrorNum("malloc: size",size);
    for (x = 0; x < malloc_count; x++)
    {
        mbegin = ((u32)allocs[x].data - size);
        mend = mbegin + size;
        for (y = 0; y < malloc_count; y++)
        {
            tbegin = (u32)allocs[y].data;
            tend = tbegin + allocs[y].size;
            if ((mbegin >= tbegin) && (mbegin < tend)) break; // 'begin' within bounds; failure 
            if ((mend >= tbegin) && (mend < tend)) break; // 'end' within bounds; failure 
            if ((mbegin <= tbegin) && (mend > tend)) break; // straddles bounds, failure
        }
        if (y == malloc_count) break;
    }
    if (x == malloc_count)
    {
        calcallocptr();
        alloc_ptr = (void*)((u32)alloc_ptr - size);
        mbegin = (u32)alloc_ptr;
        if (mdebug) GCARSErrorNum("malloc: extended heap",mbegin);
    } else if (mdebug) GCARSErrorNum("malloc: found unused",mbegin);
    alloc.data = (void*)mbegin;
    alloc.size = size;
    memcpy(&allocs[malloc_count],&alloc,sizeof(ALLOC));
    malloc_count++;
    return alloc.data;
}

void* mallocset(void* ptr,u32 size)
{
    ALLOC alloc;
    u32 mbegin,mend;
    u32 tbegin,tend;
    int y;

    ptr = (void*)(((u32)ptr & 0x3FFFFFFF) | 0x80000000);
    size = (size + (align - 1)) & ~(align - 1);
    mbegin = (u32)ptr;
    mend = mbegin + size;
    for (y = 0; y < malloc_count; y++)
    {
        tbegin = ((u32)allocs[y].data - size);
        tend = tbegin + allocs[y].size;
        if (((tbegin < 0x80000000) || (tbegin >= 0x81800000)) && mdebug) GCARSErrorNum("malloc: bad allocation",tbegin);
        if ((mbegin >= tbegin) && (mbegin < tend)) break; // 'begin' within bounds; failure 
        if ((mend >= tbegin) && (mend < tend)) break; // 'end' within bounds; failure 
        if ((mbegin <= tbegin) && (mend > tend)) break; // straddles bounds, failure
    }
    if (y < malloc_count) return NULL;
    alloc.data = (void*)mbegin;
    alloc.size = size;
    memcpy(&allocs[malloc_count],&alloc,sizeof(ALLOC));
    malloc_count++;
    return alloc.data;
}

void free(void* ptr)
{
    u32 x,data;
    data = (u32)ptr & 0x3FFFFFFF;
    for (x = 0; x < (vu32)malloc_count; x++)
    {
        if (((u32)allocs[x].data & 0x3FFFFFFF) == data)
        {
            malloc_count--;
            memcpy(&allocs[x],&allocs[x + 1],sizeof(ALLOC) * (malloc_count - x));
        }
    }
}

////////////////////////////////////////////////////////////////////////////////

char toupper(char c)
{
    if ((c >= 'a') && (c <= 'z')) return (c - ('a' - 'A'));
    return c;
}

char tolower(char c)
{
    if ((c >= 'A') && (c <= 'Z')) return (c + ('a' - 'A'));
    return c;
}

int strnicmp(const char *s1, const char *s2, size_t len)
{
    unsigned char c1, c2;
    c1 = 0;
    c2 = 0;
    if (len)
    {
        do {
            c1 = *s1;
            c2 = *s2;
            s1++;
            s2++;
            if (!c1) break;
            if (!c2) break;
            if (c1 == c2) continue;
            c1 = tolower(c1);
            c2 = tolower(c2);
            if (c1 != c2) break;
        } while (--len);
    }
    return (int)c1 - (int)c2;
}

char* strcpy(char * dest,const char *src)
{
    char *tmp = dest;
    while ((*dest++ = *src++) != '\0');
    return tmp;
}

size_t strlen(const char * s)
{
    const char *sc;
    for (sc = s; *sc != '\0'; ++sc);
    return sc - s;
}

void* memset(void * s,int c,size_t count)
{
    char *xs = (char *) s;
    while (count--) *xs++ = c;
    return s;
}

void* memcpy(void * dest,const void *src,size_t count)
{
    char *tmp = (char *) dest, *s = (char *) src;
    while (count--) *tmp++ = *s++;
    return dest;
}

void* memmove(void * dest,const void *src,size_t count)
{
    char *tmp, *s;
    if (dest <= src)
    {
        tmp = (char *) dest;
        s = (char *) src;
        while (count--) *tmp++ = *s++;
    } else {
        tmp = (char *) dest + count;
        s = (char *) src + count;
        while (count--) *--tmp = *--s;
    }
    return dest;
}

int memcmp(const void* cs,const void* ct,size_t count)
{
    const unsigned char *su1, *su2;
    int res = 0;
    for( su1 = (unsigned char*)cs, su2 = (unsigned char*)ct; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}

