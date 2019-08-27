struct FST_DISC_ENTRY {
    u32 nameoffset;
    u32 offset;
    u32 size; };

struct FST_FILE {
    u32 type;
    char* name;
    u32 offset;
    u32 size;
    u32 unused1;
    u32 unused2; };

struct FST_DIR {
    u32 type;
    char* name;
    u32 numFiles;
    u32 numDirs;
    FST_FILE* files;
    FST_DIR* dirs; };

FST_FILE* fst_add_file(FST_DIR* dir,FST_FILE* file);
FST_DIR* fst_add_dir(FST_DIR* dir,FST_DIR* add);
void fst_remove_file(FST_DIR* dir,FST_FILE* file);
void fst_remove_dir(FST_DIR* dir,FST_DIR* add);

u32 fst_count_objects(FST_DIR* root);
u32 fst_total_strlen(FST_DIR* root);

void fst_parse_dir(FST_DIR* root,void* data,u32 size);
u32 fst_unparse_dir(FST_DIR* root,void* data,u32 parent);
void fst_delete_dir(FST_DIR* del);
FST_DIR* fst_up(FST_DIR* root,FST_DIR* src);

void fst_print_dir(FST_DIR* dir);

