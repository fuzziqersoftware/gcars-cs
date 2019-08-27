#define LEVINE_VERIFIER 0x47682889

typedef struct {
    u32 verifier;
    u32 mode;
    u32 size;
    u8 data[1]; } LevinePacket;

void LevineInit();
void LevineSendData(u32 mode,void* data,u32 size);
u32 LevineReceiveData(u32 mode,void* data);
void LevineExit();
u8 LevineEnabled();

