typedef struct {
    void* data;
    u32 size;
} ALLOC;

void setmdebug(bool d);
bool setalign(u32 nalign);
void* malloc(u32 size);
void* mallocset(void* ptr,u32 size);
void free(void* data);

char* strcpy(char* dest,const char* src);
int strcmp(const char* cs,const char* ct);
size_t strlen(const char* s);
void* memset(void* s,int c,size_t count);
void* memcpy(void* dest,const void* src,size_t count);
void* memmove(void* dest,const void* src,size_t count);
int memcmp(const void* cs,const void* ct,size_t count);

