#ifndef FS_PAD_H
#define FS_PAD_H
#define MEM_FS_PAD_CHANNEL_BASE   (0xCC006400)               ///< Start address of pad-channels
#define MEM_FS_PAD_CHANNEL(Number)(MEM_FS_PAD_CHANNEL_BASE+(Number*12)) ///< Memory address of pad-channel "Number". "Number" can be 0, 1, 2 or 3. \n You can also specify one of the FS_PAD_CHANNEL_[0/1/2/3] defines
#define MEM_FS_PAD_CHANNEL_0     (MEM_FS_PAD_CHANNEL(0))       ///< Memory address of pad-channel 0
#define MEM_FS_PAD_CHANNEL_1     (MEM_FS_PAD_CHANNEL(1))       ///< Memory address of pad-channel 1
#define MEM_FS_PAD_CHANNEL_2     (MEM_FS_PAD_CHANNEL(2))       ///< Memory address of pad-channel 2
#define MEM_FS_PAD_CHANNEL_3     (MEM_FS_PAD_CHANNEL(3))       ///< Memory address of pad-channel 3
#define MEM_FS_PAD_CHANNEL_0_PTR (u32*)MEM_FS_PAD_CHANNEL_0   ///< Pointer to pad-channel 0
#define MEM_FS_PAD_CHANNEL_1_PTR (u32*)MEM_FS_PAD_CHANNEL_1   ///< Pointer to pad-channel 1
#define MEM_FS_PAD_CHANNEL_2_PTR (u32*)MEM_FS_PAD_CHANNEL_2   ///< Pointer to pad-channel 2
#define MEM_FS_PAD_CHANNEL_3_PTR (u32*)MEM_FS_PAD_CHANNEL_3   ///< Pointer to pad-channel 3
#define FS_PAD_CHANNEL_0         (0) ///< Helper define for the function FS_PAD_ReadState(). \n Can be used as PadChannel parameter.
#define FS_PAD_CHANNEL_1         (1) ///< Helper define for the function FS_PAD_ReadState(). \n Can be used as PadChannel parameter.
#define FS_PAD_CHANNEL_2         (2) ///< Helper define for the function FS_PAD_ReadState(). \n Can be used as PadChannel parameter.
#define FS_PAD_CHANNEL_3         (3) ///< Helper define for the function FS_PAD_ReadState(). \n Can be used as PadChannel parameter.
#define FS_PAD_CHANNEL_ALL       (5) ///< Helper define for the function FS_PAD_ReadState(). \n Can be used as PadChannel parameter.

typedef struct FS_PAD_BUTTONS_DIGITAL {
   u8 Up;         ///< Digital button UP. (true or false)
   u8 Down;       ///< Digital button DOWN. (true or false)
   u8 Left;       ///< Digital button LEFT. (true or false)
   u8 Right;      ///< Digital button RIGHT. (true or false)
   u8 L;          ///< Digital button L. (true or false)
   u8 R;          ///< Digital button R. (true or false)
   u8 A;          ///< Digital button A. (true or false)
   u8 B;          ///< Digital button B. (true or false)
   u8 X;          ///< Digital button X. (true or false)
   u8 Y;          ///< Digital button Y. (true or false)
   u8 Z;          ///< Digital button Z. (true or false)
   u8 Start;      ///< Digital button START. (true or false)
} FS_PAD_BUTTONS_DIGITAL;

typedef struct FS_PAD_BUTTONS_ANALOG {
   s8 X;          ///< In range between -127 .. 127 ???
   s8 Y;          ///< In range between -127 .. 127 ???
} FS_PAD_BUTTONS_ANALOG;

typedef struct FS_PAD_TRIGGER {
   u8 L;       ///< 0x18 .. 0xe3 ??? Is u8 OK?
   u8 R;       ///< 0x18 .. 0xe3 ??? Is u8 OK?
} FS_PAD_TRIGGER;

typedef struct {
   FS_PAD_BUTTONS_DIGITAL  Digital;    ///< Digital buttons
   FS_PAD_BUTTONS_ANALOG   Analog;     ///< Analog stick
   FS_PAD_BUTTONS_ANALOG   AnalogC;    ///< Analog stick C
   FS_PAD_TRIGGER          Trigger;    ///< Trigger (L + R)
} PAD;

void FS_PAD_ReadAllState(PAD* pPad);
void FS_PAD_ReadState(PAD *pPad, u8 PadChannel);
void FS_PAD_WaitEmpty(u8);
void FS_PAD_WaitEmptyAll();
bool FS_PAD_IsEmpty(PAD*);
void FS_PAD_Init();

#endif /* FS_PAD_H */

