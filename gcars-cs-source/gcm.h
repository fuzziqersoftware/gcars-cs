typedef struct {
    u32 gameid;
    u16 company;
    u8 diskid;
    u8 version;
    u8 streaming;
    u8 streamsize;
    u8 unused1[0x12];
    u32 identifier;
    char name[0x3E0];
    u32 debugmonitoroffset;
    u32 debugmonitoraddr;
    u8 unused2[0x18];
    u32 dolfileoffset;
    u32 fstoffset;
    u32 fstsize;
    u32 fstmaxsize;
    u32 userposition;
    u32 userlength;
    u32 unknown2;
    u32 unknown3;
    u8 unknown4[0x2000]; } GCM_HEADER;

typedef struct {
    char date[0x10];
    u32 entrypoint;
    u32 size;
    u32 trailersize;
    u32 unused; } APPLOADER_HEADER;

typedef struct {
    GCM_HEADER gcmheader;
    APPLOADER_HEADER apploaderheader; } GCM_IMAGE;

typedef struct {
    u32 nameoffset;
    u32 offset;
    u32 size; } FST_DISC_ENTRY;

