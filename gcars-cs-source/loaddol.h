typedef struct {
    u32 textpos[7];
    u32 datapos[11];
    u32 textmem[7];
    u32 datamem[11];
    u32 textsize[7];
    u32 datasize[11];
    u32 bssaddr;
    u32 bsssize;
    u32 entrypoint;
    u32 unused[7]; } DOL_HEADER;

u32 _dolsize(DOL_HEADER*);
u8 _dolvalid(DOL_HEADER*);
void dol_load();
void dol_launch(DOL_HEADER*);

