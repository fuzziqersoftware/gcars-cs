#define GCM_SEEK_BEGIN 0
#define GCM_SEEK_CURR  1
#define GCM_SEEK_END   2

typedef struct {
    // game info 
    u32 gameid;
    u16 company;
    u8 diskid;
    u8 version;
    u8 streaming;
    u8 streamsize;
    u8 unused1[0x12];
    u32 identifier;
    char name[0x3E0];

    // debugmon info 
    u32 debugmonitoroffset;
    u32 debugmonitoraddr;
    u8 unused2[0x18];

    // boot info
    u32 dolfileoffset;
    u32 fstoffset;
    u32 fstsize;
    u32 fstmaxsize;
    u32 userposition;
    u32 userlength;
    u32 unknown2;
    u32 unknown3;

    // BI2 info 
    s32 debugMonSize;
    s32 simMemSize;
    u32 argOffset;
    u32 debugFlag;
    u32 trkLocation;
    s32 trkSize;
    u32 countryCode;
    u32 unknown4[3];
    u32 dolLimit;
    u8  unknown5[0x1FD4]; } GCM_HEADER;

typedef struct {
    char date[0x10];
    u32 entrypoint;
    u32 size;
    u32 trailersize;
    u32 unused; } APPLOADER_HEADER;

typedef struct {
    GCM_HEADER gcmheader;
    APPLOADER_HEADER apploaderheader;

    FST_DIR rootdir;

    HANDLE file; } GCM_IMAGE;

typedef struct {
    GCM_IMAGE* image;
    HANDLE file;

    char filename[256];
    u32 offset;
    u32 position;
    u32 size; } GCM_FILE;

GCM_IMAGE* gcm_init(char* name);
void gcm_exit(GCM_IMAGE* image);

GCM_FILE* gcm_open_file(GCM_IMAGE* image,char* name);
void gcm_seek_file(GCM_FILE* file,s32 value,u32 mode);
void gcm_read_file(GCM_FILE* file,void* data,u32 size);
void gcm_write_file(GCM_FILE* file,void* data,u32 size);
void gcm_close_file(GCM_FILE* file);

