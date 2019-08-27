#include "gamecube.h"
#include "cs.h"

#include "gcars-testfile.bin.h"

#define GCARS_FILENAME "GCARS-CS-CONFIG"

char file_header_old[] = {
0x46,0x75,0x7A,0x7A,0x69,0x71,0x65,0x72,0x20,0x53,0x6F,0x66,0x74,0x77,0x61,0x72,
0x65,0x20,0x47,0x43,0x41,0x52,0x53,0x2D,0x43,0x53,0x00,0x00,0x00,0x00,0x00,0x00,
0x43,0x6F,0x64,0x65,0x20,0x4C,0x69,0x73,0x74,0x20,0x46,0x69,0x6C,0x65,0x20,0x28,
0x76,0x30,0x2E,0x39,0x29,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

unsigned char file_header[] = {
0x46,0x75,0x7A,0x7A,0x69,0x71,0x65,0x72,0x20,0x53,0x6F,0x66,0x74,0x77,0x61,0x72,
0x65,0x20,0x47,0x43,0x41,0x52,0x53,0x2D,0x43,0x53,0x00,0x00,0x00,0x00,0x00,0x00,
0x43,0x6F,0x64,0x65,0x20,0x4C,0x69,0x73,0x74,0x20,0x46,0x69,0x6C,0x65,0x20,0x28,
0x76,0x31,0x2E,0x30,0x29,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01, // last byte was 0x00 
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x00,0x01,
0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,
0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x03,0x03,
0x03,0x03,0x03,0x03,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,
0x01,0x01,0x01,0x01,0x01,0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x00,0x03,0x03,
0x03,0x03,0x03,0x02,0x01,0x00,0x00,0x01,0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x01,
0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x01,0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x01,
0x02,0x03,0x03,0x03,0x03,0xAE,0x03,0x03,0x03,0xB7,0xB8,0xB9,0xBA,0x8A,0x03,0x37,
0x36,0xB6,0x4D,0x6A,0x0B,0x5A,0x77,0x6A,0x1D,0xB1,0xB2,0x70,0xB3,0x2D,0x1D,0xA8,
0x2A,0x27,0xAF,0x57,0xB0,0x1D,0x8A,0xBB,0xA0,0xBC,0xBD,0x03,0x03,0x03,0x8B,0x28,
0x6A,0x4B,0x32,0x2E,0x46,0x03,0xB4,0xB5,0x7C,0x7E,0x62,0x1D,0x1C,0x64,0x1D,0x32,
0xA0,0xA5,0xB0,0x85,0x57,0x0A,0x03,0x03,0x03,0x03,0x03,0x02,0x01,0x00,0x03,0x03,
0x03,0x03,0x03,0x02,0x01,0x00,0x03,0x03,0x03,0x03,0x03,0x02,0x01,0x00,0x28,0x03,
0x03,0x03,0x03,0x02,0x01,0x00,0x00,0x01,0x02,0x03,0x03,0x03,0xA7,0xA8,0x00,0x01,
0x02,0x03,0x03,0xA3,0xA4,0x46,0x00,0x01,0x02,0x03,0x03,0x79,0x80,0x99,0x00,0x01,
0x02,0x03,0x3D,0x30,0x43,0x63,0xA9,0xA9,0xAA,0x70,0x85,0xA7,0x76,0xA9,0x70,0x23,
0x1D,0x1D,0x40,0x71,0xA5,0x6C,0x2D,0x9F,0x7E,0xA0,0x1D,0x1D,0x08,0x78,0x4F,0x27,
0x78,0x9C,0x98,0x1D,0x98,0x10,0x54,0x11,0x59,0x7C,0x6B,0x03,0xAB,0xAC,0x1E,0x93,
0x5D,0xA6,0x9F,0x03,0xA3,0x29,0xA1,0xA2,0x65,0x46,0x7C,0x03,0x5E,0x20,0x3F,0x1D,
0x9D,0x4E,0x19,0x05,0x3C,0x7F,0x9D,0xAD,0x03,0x03,0x03,0x02,0x01,0x00,0x37,0x9C,
0x6B,0x03,0x03,0x02,0x01,0x00,0x3C,0x30,0x19,0x03,0x03,0x02,0x01,0x00,0x96,0x95,
0x8E,0x9E,0x03,0x02,0x01,0x00,0x00,0x01,0x02,0x03,0x28,0x83,0x03,0x94,0x00,0x01,
0x02,0x03,0x8F,0x60,0x90,0x5F,0x00,0x01,0x02,0x03,0x86,0x5E,0x87,0x6C,0x00,0x01,
0x02,0x03,0x17,0x64,0x6F,0x42,0x90,0x5D,0x6B,0x97,0x26,0x1D,0x1D,0x1D,0x0D,0x03,
0x91,0x20,0x2C,0x1D,0x1D,0x1D,0x03,0x03,0x88,0x34,0x52,0x89,0x1D,0x1D,0x64,0x64,
0x58,0x2F,0x81,0x82,0x1D,0x1D,0x1D,0x98,0x99,0x9A,0x2E,0x66,0x4A,0x9B,0x1D,0x82,
0x92,0x03,0x28,0x93,0x03,0x03,0x8A,0x59,0x03,0x03,0x8B,0x7A,0x03,0x03,0x00,0x83,
0x84,0x81,0x63,0x53,0x64,0x64,0x7F,0x03,0x6E,0x58,0x03,0x02,0x01,0x00,0x94,0x46,
0x95,0x96,0x03,0x02,0x01,0x00,0x8C,0x8D,0x03,0x8E,0x03,0x02,0x01,0x00,0x2E,0x85,
0x84,0x82,0x03,0x02,0x01,0x00,0x00,0x01,0x02,0x03,0x09,0x61,0x6C,0x76,0x00,0x01,
0x02,0x03,0x6A,0x5E,0x6B,0x6C,0x00,0x01,0x02,0x03,0x33,0x66,0x67,0x68,0x00,0x01,
0x02,0x03,0x2B,0x13,0x03,0x5C,0x77,0x77,0x78,0x79,0x41,0x7A,0x01,0x7B,0x03,0x03,
0x6D,0x6E,0x03,0x03,0x03,0x14,0x0D,0x03,0x50,0x69,0x03,0x03,0x03,0x61,0x5D,0x5E,
0x46,0x5F,0x60,0x61,0x48,0x2A,0x10,0x7C,0x7D,0x47,0x4F,0x3B,0x7E,0x77,0x63,0x6F,
0x70,0x5B,0x71,0x72,0x03,0x03,0x22,0x5D,0x06,0x1D,0x1D,0x1D,0x16,0x03,0x38,0x5A,
0x32,0x1D,0x1D,0x1D,0x18,0x62,0x7F,0x65,0x61,0x80,0x03,0x02,0x01,0x00,0x73,0x74,
0x03,0x75,0x03,0x02,0x01,0x00,0x58,0x43,0x3A,0x1E,0x03,0x02,0x01,0x00,0x63,0x03,
0x64,0x65,0x03,0x02,0x01,0x00,0x00,0x01,0x02,0x03,0x56,0x4B,0x37,0x57,0x00,0x01,
0x02,0x03,0x03,0x0F,0x4D,0x4E,0x00,0x01,0x02,0x03,0x03,0x43,0x44,0x45,0x00,0x01,
0x02,0x03,0x03,0x03,0x34,0x0A,0x36,0x05,0x58,0x54,0x59,0x5A,0x48,0x2A,0x4F,0x4A,
0x0D,0x13,0x50,0x03,0x03,0x51,0x20,0x46,0x03,0x15,0x47,0x03,0x03,0x48,0x35,0x36,
0x03,0x37,0x38,0x39,0x3A,0x3B,0x36,0x48,0x23,0x1D,0x1D,0x1D,0x1D,0x1D,0x52,0x03,
0x4C,0x08,0x1D,0x1D,0x1D,0x1D,0x15,0x03,0x03,0x11,0x1D,0x1D,0x1D,0x49,0x3C,0x3D,
0x3E,0x3F,0x1D,0x25,0x40,0x41,0x5B,0x37,0x19,0x3E,0x03,0x02,0x01,0x00,0x53,0x54,
0x54,0x55,0x03,0x02,0x01,0x00,0x4A,0x4B,0x4C,0x03,0x03,0x02,0x01,0x00,0x42,0x13,
0x03,0x03,0x03,0x02,0x01,0x00,0x00,0x01,0x02,0x03,0x03,0x03,0x03,0x28,0x00,0x01,
0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x01,0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x01,
0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x29,0x2A,0x2B,0x2C,0x2D,0x16,0x2E,0x1B,0x1C,
0x1D,0x1E,0x1F,0x20,0x03,0x21,0x03,0x0E,0x0F,0x10,0x11,0x12,0x13,0x14,0x03,0x03,
0x03,0x04,0x05,0x06,0x07,0x08,0x2F,0x13,0x30,0x1D,0x31,0x32,0x2A,0x33,0x22,0x03,
0x23,0x24,0x0A,0x25,0x26,0x27,0x15,0x16,0x17,0x12,0x18,0x19,0x1A,0x03,0x09,0x07,
0x0A,0x0B,0x0C,0x0D,0x03,0x03,0x20,0x0D,0x03,0x03,0x03,0x02,0x01,0x00,0x03,0x03,
0x03,0x03,0x03,0x02,0x01,0x00,0x03,0x03,0x03,0x03,0x03,0x02,0x01,0x00,0x03,0x03,
0x03,0x03,0x03,0x02,0x01,0x00,0x00,0x01,0x02,0x03,0x03,0x03,0x03,0x03,0x00,0x01,
0x02,0x02,0x02,0x02,0x02,0x02,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x03,0x02,0x02,
0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x03,0x03,0x03,0x02,0x01,0x00,0x02,0x02,
0x02,0x02,0x02,0x02,0x01,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0xDE,0xF7,0xBD,0xEF,0xA1,0x08,0x80,0x00,0x98,0xC6,
0xC6,0x31,0xDE,0xF7,0xF3,0x9C,0xF7,0xBD,0xF7,0xBD,0xE3,0x18,0xCA,0x52,0x9C,0xE7,
0x80,0x00,0x98,0xC6,0xD6,0xB5,0xFF,0xFF,0xE3,0x18,0xE3,0x18,0xC2,0x10,0xB1,0x8C,
0xB1,0x8C,0xC2,0x10,0xF7,0xBD,0xFB,0xDE,0xDE,0xF7,0x9C,0xE7,0xB5,0xAD,0xF3,0x9C,
0xFF,0xFF,0xDE,0xF7,0xA1,0x08,0xC6,0x31,0xA5,0x29,0xAD,0x6B,0xE7,0x39,0xD6,0xB5,
0xFF,0xFF,0xF3,0x9C,0xBD,0xEF,0xB9,0xCE,0xCA,0x52,0xCA,0x52,0xBD,0xEF,0xEF,0x7B,
0xD6,0xB5,0xD6,0xB5,0xD6,0xB5,0xE7,0x39,0xFF,0xFF,0xFB,0xDE,0xDA,0xD6,0xB5,0xAD,
0xA9,0x4A,0xCE,0x73,0x8C,0x63,0xCA,0x52,0x88,0x42,0x90,0x84,0xC2,0x10,0xC6,0x31,
0x90,0x84,0xA1,0x08,0xFF,0xFF,0xF7,0xBD,0xA9,0x4A,0xDA,0xD6,0x94,0xA5,0xEB,0x5A,
0x8C,0x63,0x94,0xA5,0xAD,0x6B,0xA9,0x4A,0xDE,0xF7,0x84,0x21,0xE7,0x39,0x9C,0xE7,
0xEB,0x5A,0xCA,0x52,0xD2,0x94,0x90,0x84,0xA9,0x4A,0xB1,0x8C,0xCE,0x73,0xE3,0x18,
0x80,0x00,0x94,0xA5,0xD2,0x94,0xC6,0x31,0xB9,0xCE,0xAD,0x6B,0xEB,0x5A,0xCE,0x73,
0x84,0x21,0x80,0x00,0xCE,0x73,0xA1,0x08,0xA9,0x4A,0xD2,0x94,0xD2,0x94,0xB5,0xAD,
0xC6,0x31,0x9C,0xE7,0x88,0x42,0xD2,0x94,0xC2,0x10,0xEB,0x5A,0x98,0xC6,0xC2,0x10,
0x98,0xC6,0xB9,0xCE,0xCA,0x52,0xC6,0x31,0xF7,0xBD,0xC2,0x10,0xB9,0xCE,0xA5,0x29,
0xE3,0x18,0xD2,0x94,0xA9,0x4A,0xBD,0xEF,0xD2,0x94,0xA1,0x08,0xF7,0xBD,0xC2,0x10,
0xA5,0x29,0xA9,0x4A,0xCE,0x73,0xEF,0x7B,0xB1,0x8C,0xF3,0x9C,0xC2,0x10,0xB5,0xAD,
0xCE,0x73,0xEB,0x5A,0x94,0xA5,0x98,0xC6,0xFF,0xFF,0xFB,0xDE,0xB1,0x8C,0xB9,0xCE,
0xA1,0x08,0xE3,0x18,0xD6,0xB5,0x88,0x42,0x8C,0x63,0x84,0x21,0x9C,0xE7,0xCA,0x52,
0x94,0xA5,0xDA,0xD6,0xDA,0xD6,0xFF,0xFF,0xBD,0xEF,0xA5,0x29,0x84,0x21,0xE7,0x39,
0xE3,0x18,0x9C,0xE7,0xB5,0xAD,0xDA,0xD6,0xFB,0xDE,0xFB,0xDE,0x90,0x84,0xE7,0x39,
0xEF,0x7B,0xA5,0x29,0xAD,0x6B,0xEF,0x7B,0xEF,0x7B,0xBD,0xEF,0xCA,0x52,0xB1,0x8C,
0xB9,0xCE,0xAD,0x6B,0xEB,0x5A,0xC6,0x31,0xDE,0xF7,0xA5,0x29,0x80,0x00,0xCE,0x73,
0x8C,0x63,0xFB,0xDE,0x90,0x84,0xB9,0xCE,0xDA,0xD6,0xF3,0x9C,0xF3,0x9C,0xBD,0xEF,
0x94,0xA5,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,
0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

u32 slotID = 0xFFFFFFFF;

bool GCARSCardPrintError(s32 error)
{
    char* title = "Fuzziqer Software GCARS-CS";
    if (error < 0)
    {
        GCARSClearFrameBuffer(COLOR_BLACK);
        GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
        GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
        DEBUG_SetTextColor(COLOR_QRHYGREEN);
        DEBUG_Print(50,50,title);
        DEBUG_SetTextColor(COLOR_YELLOW);
        DEBUG_Print(50,100,"Memory Card Error:");
        switch (error)
        {
          case CARD_ERROR_NONE:
            DEBUG_Print(50,120,"No error.");
            DEBUG_Print(50,140,"Please yell at Fuzziqer, because");
            DEBUG_Print(50,160,"you should never see this error.");
            break;
          case CARD_ERROR_NOCARD:
            DEBUG_Print(50,120,"No memory card present.");
            break;
          case CARD_ERROR_CORRUPT:
            DEBUG_Print(50,120,"I/O error. Card may be corrupt.");
            break;
          case CARD_ERROR_FATAL:
            DEBUG_Print(50,120,"< FATAL ERROR >");
            break;
          case CARD_ERROR_INIT:
            DEBUG_Print(50,120,"Card system not inited.");
            DEBUG_Print(50,140,"Please yell at Fuzziqer, because");
            DEBUG_Print(50,160,"you should never see this error.");
            break;
          case CARD_ERROR_NOSPACE:
            DEBUG_Print(50,120,"No free space.");
            break;
          case CARD_ERROR_NOENTRY:
            DEBUG_Print(50,120,"File entry not found.");
            break;
          case CARD_ERROR_EXISTS:
            DEBUG_Print(50,120,"File already exists.");
            break;
          default:
            DEBUG_ShowValueU32(66,120,error);
        }
        debugDrawInfo();
        return 1;
    }
    return 0;
}

void GCARSSelectCardSlot()
{
    char* title = "GCARS-CS: Select Card Slot";

    char menu1[100] = "A: ";
    char menu2[100] = "B: ";
    char menu3[] = "Do Not Load or Save";

    u32 ids[2];
    ids[0] = exi_deviceid(0,0);
    ids[1] = exi_deviceid(1,0);
    strcpy(&menu1[3],exi_devicename(ids[0]));
    strcpy(&menu2[3],exi_devicename(ids[1]));

    if ((ids[0] | ids[1]) == 0)
    {
        GCARSError("No memory cards inserted.");
        slotID = 0xFFFFFFFF;
        return;
    }
    if (ids[0] == 0)
    {
        slotID = CARD_SLOTB;
        return;
    }
    if (ids[1] == 0)
    {
        slotID = CARD_SLOTA;
        return;
    }

    u32 x;
    u8 draw = 1;
    u8 selected = 1;
    PAD pad;

    for (;;)
    {
        if ((ids[0] != exi_deviceid(0,0)) || (ids[1] != exi_deviceid(1,0)))
        {
            ids[0] = exi_deviceid(0,0);
            ids[1] = exi_deviceid(1,0);
            strcpy(&menu1[3],exi_devicename(ids[0]));
            strcpy(&menu2[3],exi_devicename(ids[1]));
            draw = 1;
        }

        if (draw)
        {
            GCARSClearFrameBuffer(COLOR_BLACK);
            GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
            GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
            DEBUG_SetTextColor(COLOR_QRHYGREEN);
            DEBUG_Print(50,50,title);
            if (debugEnabled())
            {
                DEBUG_SetTextColor(COLOR_GRAY);
                DEBUG_ShowValueU32(50,150,ids[0]);
                DEBUG_ShowValueU32(50,170,ids[1]);
            }
            DEBUG_SetTextColor(COLOR_WHITE);
            DEBUG_Print(debugEnabled() ? 194 : 50,150,menu1);
            DEBUG_Print(debugEnabled() ? 194 : 50,170,menu2);
            DEBUG_Print(debugEnabled() ? 194 : 50,190,menu3);
            DEBUG_SetTextColor(COLOR_YELLOW);
            switch (selected)
            {
              case 0:
                DEBUG_Print(debugEnabled() ? 194 : 50,150,menu1);
                break;
              case 1:
                DEBUG_Print(debugEnabled() ? 194 : 50,170,menu2);
                break;
              case 2:
                DEBUG_Print(debugEnabled() ? 194 : 50,190,menu3);
                break;
            }
            debugDrawInfo();
        }

        while (draw)
        {
            FS_PAD_ReadAllState(&pad);
            if (FS_PAD_IsEmpty(&pad)) draw = 0;
            else FS_VIDEO_WaitVSync();
        }

        FS_PAD_ReadAllState(&pad);

        if (pad.Digital.Down || (pad.Analog.Y > 0x40) || (pad.AnalogC.Y > 0x40))
        {
            if (selected < 2) selected++;
            else selected = 0;
            draw = 1;
        }
        if (pad.Digital.Up || (pad.Analog.Y < -0x40) || (pad.AnalogC.Y < -0x40))
        {
            if (selected > 0) selected--;
            else selected = 2;
            draw = 1;
        }

        if (pad.Digital.A || pad.Digital.Start)
        {
            switch (selected)
            {
              case 0:
                slotID = CARD_SLOTA;
                break;
              case 1:
                slotID = CARD_SLOTB;
                break;
              case 2:
                slotID = 0xFFFFFFFF;
                break;
            }
            return;
        }

        if (pad.Digital.B)
        {
            slotID = 0xFFFFFFFF;
            return;
        }

        debug_hook();
        FS_VIDEO_WaitVSync();
    }
}

////////////////////////////////////////////////////////////////////////////////

void GCARSLoadCodes()
{
    u32* buffer = malloc(0x00100000);//(u32*)0x81600000;
    u32* buffer_orig = buffer;

    if (slotID != 0xFFFFFFFF)
    {
        GCARSClearFrameBuffer(COLOR_BLACK);
        GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
        GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
        DEBUG_SetTextColor(COLOR_QRHYGREEN);
        DEBUG_Print(50,50,"Fuzziqer Software GCARS-CS");
        DEBUG_SetTextColor(COLOR_YELLOW);
        if (slotID == CARD_SLOTA) DEBUG_Print(50,100,"Loading code list: Slot A");
        if (slotID == CARD_SLOTB) DEBUG_Print(50,100,"Loading code list: Slot B");
        DEBUG_SetTextColor(COLOR_WHITE);
        DEBUG_Print(50,150,"Reading from the Memory Card.");
        DEBUG_Print(50,170,"Do not touch the Memory Card");
        DEBUG_Print(50,190,"or the Power Button.");

        s32 errors;
        u32 size;

        int fixed;
        CARD_Begin(slotID,&fixed);

        CARD_File file;
        CARD_Offsets offsets;

        memset(&file,0,sizeof(file));
        strcpy(&file.filename[0],GCARS_FILENAME);
        strcpy(&file.gamecode[0],"ARCS");
        strcpy(&file.company[0],"FS");

        errors = CARD_OpenFile(&file,&offsets);
        if (errors != CARD_ERROR_NONE)
        {
            GCARSErrorNum("Could not open file",errors);
            memcpy(buffer,testfile,sizeof(testfile));
        } else {
            errors = CARD_ReadFile(&file,buffer);
            if (errors != CARD_ERROR_NONE)
            {
                GCARSErrorNum("Could not read file",errors);
                memcpy(buffer,testfile,sizeof(testfile));
            }
        }
    } else memcpy(buffer,testfile,sizeof(testfile));

    GCARSStatus("Parsing code list");
    if (*(u8*)((u32)buffer + 0x3F) == 0x01) buffer = (u32*)((u32)buffer + sizeof(file_header));
    else buffer = (u32*)((u32)buffer + sizeof(file_header_old));

    u32 x,y,z;
    u32 ver;
    GCARS_DATABLOCK* db = malloc(sizeof(GCARS_DATABLOCK));
    GCARS_GAME* g;
    GCARS_CODE* c;

    if (buffer[0] != GCARS_FILE_IDENTIFIER)
    {
        if (debugEnabled()) hexviewer("Code list file is corrupt.",(u32)buffer);
        else GCARSError("Code list file is corrupt.");
        memcpy(buffer,testfile,sizeof(testfile));
    }
    ver = buffer[1];
    if (ver > GCARS_FILE_VERSION)
    {
        if (debugEnabled()) hexviewer("Code list format unrecognized.",(u32)buffer);
        else GCARSError("Code list format unrecognized.");
        memcpy(buffer,testfile,sizeof(testfile));
    }
    buffer = (u32*)((u32)buffer + 8);

    memcpy(CS_DATA,buffer,sizeof(CSData));
    buffer = (u32*)((u32)buffer + sizeof(CSData));

    if (ver == 2)
    {
        memcpy(CS_ENTITY,buffer,0x24);
        buffer = (u32*)((u32)buffer + 0x24);
    } else {
        memcpy(CS_ENTITY,buffer,sizeof(CSEntity));
        buffer = (u32*)((u32)buffer + sizeof(CSEntity));
    }

    db->numgames = buffer[0];
    buffer = (u32*)((u32)buffer + sizeof(db->numgames));

    db->games = (GCARS_GAME**)malloc(sizeof(GCARS_GAME*) * db->numgames);

    for (x = 0; x < db->numgames; x++)
    {
        db->games[x] = (GCARS_GAME*)malloc(sizeof(GCARS_GAME));
        memcpy(db->games[x]->name,buffer,sizeof(db->games[x]->name));
        buffer = (u32*)((u32)buffer + sizeof(db->games[x]->name));
        db->games[x]->gameid = *buffer;
        buffer = (u32*)((u32)buffer + sizeof(db->games[x]->gameid));
        memcpy(db->games[x]->numaddress,buffer,16);
        buffer = (u32*)((u32)buffer + 16);
        db->games[x]->address[0] = malloc(db->games[x]->numaddress[0] * 8);
        memcpy(db->games[x]->address[0],buffer,db->games[x]->numaddress[0] * 8);
        buffer = (u32*)((u32)buffer + (db->games[x]->numaddress[0] * 8));
        db->games[x]->address[1] = malloc(db->games[x]->numaddress[1] * 8);
        memcpy(db->games[x]->address[1],buffer,db->games[x]->numaddress[1] * 8);
        buffer = (u32*)((u32)buffer + (db->games[x]->numaddress[1] * 8));
        db->games[x]->address[2] = malloc(db->games[x]->numaddress[2] * 8);
        memcpy(db->games[x]->address[2],buffer,db->games[x]->numaddress[2] * 8);
        buffer = (u32*)((u32)buffer + (db->games[x]->numaddress[2] * 8));
        db->games[x]->address[3] = malloc(db->games[x]->numaddress[3] * 8);
        memcpy(db->games[x]->address[3],buffer,db->games[x]->numaddress[3] * 8);
        buffer = (u32*)((u32)buffer + (db->games[x]->numaddress[3] * 8));
        db->games[x]->padaddress = *buffer;
        buffer = (u32*)((u32)buffer + sizeof(db->games[x]->padaddress));
        if (ver < 4)
        {
            db->games[x]->frames = 0;
            db->games[x]->flags  = 0;
            db->games[x]->filler = 0;
            buffer = (u32*)((u32)buffer + 12);
        }
        if (ver > 4)
        {
            db->games[x]->frames = *buffer;
            buffer = (u32*)((u32)buffer + 4);
            db->games[x]->flags  = *buffer;
            buffer = (u32*)((u32)buffer + 4);
            db->games[x]->filler = *buffer;
            buffer = (u32*)((u32)buffer + 4);
        }
        db->games[x]->numcodes = *buffer;
        buffer = (u32*)((u32)buffer + sizeof(db->games[x]->numcodes));
    }

    for (x = 0; x < db->numgames; x++)
    {
        db->games[x]->codes = (GCARS_CODE**)malloc(sizeof(GCARS_CODE**) * db->games[x]->numcodes);
        for (y = 0; y < db->games[x]->numcodes; y++)
        {
            db->games[x]->codes[y] = (GCARS_CODE*)malloc(sizeof(GCARS_CODE));
            db->games[x]->codes[y]->numlines = *buffer;
            buffer = (u32*)((u32)buffer + sizeof(db->games[x]->codes[y]->numlines));
            db->games[x]->codes[y]->enabled = *buffer;
            buffer = (u32*)((u32)buffer + sizeof(db->games[x]->codes[y]->enabled));
            memcpy(db->games[x]->codes[y]->name,buffer,sizeof(db->games[x]->codes[y]->name));
            buffer = (u32*)((u32)buffer + sizeof(db->games[x]->codes[y]->name));
            db->games[x]->codes[y]->lines = malloc(db->games[x]->codes[y]->numlines * 8);
            memcpy(db->games[x]->codes[y]->lines,buffer,(db->games[x]->codes[y]->numlines * 8));
            buffer = (u32*)((u32)buffer + (db->games[x]->codes[y]->numlines * 8));
        }
    }
    datablock = db;
    free(buffer);

    if (ver < GCARS_FILE_VERSION)
    {
        GCARSError("Your code list has been updated.");
        GCARSSaveCodes();
    }
}

void GCARSSaveCodes()
{
    if (slotID == 0xFFFFFFFF) return;

    GCARSStatus("Parsing code list");

    u32* buffer = malloc(0x00100000);//(u32*)0x81600000;
    u32* buffer_orig = buffer;
    u32 x,y,z;

    memcpy(buffer,file_header,sizeof(file_header));
    buffer = (u32*)((u32)buffer + sizeof(file_header));

    buffer[0] = GCARS_FILE_IDENTIFIER;
    buffer[1] = GCARS_FILE_VERSION;
    buffer = (u32*)((u32)buffer + 8);

    memcpy(buffer,CS_DATA,sizeof(CSData));
    buffer = (u32*)((u32)buffer + sizeof(CSData));
    memcpy(buffer,CS_ENTITY,sizeof(CSEntity));
    buffer = (u32*)((u32)buffer + sizeof(CSEntity));

    buffer[0] = datablock->numgames;
    buffer = (u32*)((u32)buffer + sizeof(datablock->numgames));
    for (x = 0; x < datablock->numgames; x++)
    {
        memcpy(buffer,datablock->games[x]->name,sizeof(datablock->games[x]->name));
        buffer = (u32*)((u32)buffer + sizeof(datablock->games[x]->name));
        memcpy(buffer,&datablock->games[x]->gameid,sizeof(datablock->games[x]->gameid));
        buffer = (u32*)((u32)buffer + sizeof(datablock->games[x]->gameid));
        memcpy(buffer,&datablock->games[x]->numaddress,sizeof(datablock->games[x]->numaddress));
        buffer = (u32*)((u32)buffer + sizeof(datablock->games[x]->numaddress));
        memcpy(buffer,datablock->games[x]->address[0],datablock->games[x]->numaddress[0] * 8);
        buffer = (u32*)((u32)buffer + (datablock->games[x]->numaddress[0] * 8));
        memcpy(buffer,datablock->games[x]->address[1],datablock->games[x]->numaddress[1] * 8);
        buffer = (u32*)((u32)buffer + (datablock->games[x]->numaddress[1] * 8));
        memcpy(buffer,datablock->games[x]->address[2],datablock->games[x]->numaddress[2] * 8);
        buffer = (u32*)((u32)buffer + (datablock->games[x]->numaddress[2] * 8));
        memcpy(buffer,datablock->games[x]->address[3],datablock->games[x]->numaddress[3] * 8);
        buffer = (u32*)((u32)buffer + (datablock->games[x]->numaddress[3] * 8));
        memcpy(buffer,&datablock->games[x]->padaddress,4);
        buffer = (u32*)((u32)buffer + 4);
        memcpy(buffer,&datablock->games[x]->frames,4);
        buffer = (u32*)((u32)buffer + 4);
        memcpy(buffer,&datablock->games[x]->flags,4);
        buffer = (u32*)((u32)buffer + 4);
        memcpy(buffer,&datablock->games[x]->filler,4);
        buffer = (u32*)((u32)buffer + 4);
        memcpy(buffer,&datablock->games[x]->numcodes,sizeof(datablock->games[x]->numcodes));
        buffer = (u32*)((u32)buffer + sizeof(datablock->games[x]->numcodes));
    }
    for (x = 0; x < datablock->numgames; x++)
    {
        for (y = 0; y < datablock->games[x]->numcodes; y++)
        {
            memcpy(buffer,&datablock->games[x]->codes[y]->numlines,sizeof(datablock->games[x]->codes[y]->numlines));
            buffer = (u32*)((u32)buffer + sizeof(datablock->games[x]->codes[y]->numlines));
            memcpy(buffer,&datablock->games[x]->codes[y]->enabled,sizeof(datablock->games[x]->codes[y]->enabled));
            buffer = (u32*)((u32)buffer + sizeof(datablock->games[x]->codes[y]->enabled));
            memcpy(buffer,datablock->games[x]->codes[y]->name,sizeof(datablock->games[x]->codes[y]->name));
            buffer = (u32*)((u32)buffer + sizeof(datablock->games[x]->codes[y]->name));
            memcpy(buffer,datablock->games[x]->codes[y]->lines,(datablock->games[x]->codes[y]->numlines * 8));
            buffer = (u32*)((u32)buffer + (datablock->games[x]->codes[y]->numlines * 8));
        }
    }

    u32 osize = ((u32)buffer - (u32)buffer_orig);
    u32 size = osize;
    size = (size + 8191) & ~8191;
    int fixed;
    s32 errors;

    GCARSClearFrameBuffer(COLOR_BLACK);
    GRAPH_Rectangle(0,40,320,2,COLOR_QRHYGREEN);
    GRAPH_Rectangle(0,74,320,2,COLOR_QRHYGREEN);
    DEBUG_SetTextColor(COLOR_QRHYGREEN);
    DEBUG_Print(50,50,"Fuzziqer Software GCARS-CS");
    DEBUG_SetTextColor(COLOR_YELLOW);
    if (slotID == CARD_SLOTA) DEBUG_Print(50,100,"Saving code list: Slot A");
    if (slotID == CARD_SLOTB) DEBUG_Print(50,100,"Saving code list: Slot B");
    if (debugEnabled())
    {
        DEBUG_Print(50,230,"Data size:");
        DEBUG_Print(50,250,"File size:");
        DEBUG_ShowValueU32(242,230,osize);
        DEBUG_ShowValueU32(242,250,size);
    }
    DEBUG_SetTextColor(COLOR_ORANGE);
    DEBUG_Print(50,150,"Writing to the Memory Card.");
    DEBUG_Print(50,170,"Do not touch the Memory Card");
    DEBUG_Print(50,190,"or the Power Button.");

    CARD_File file;
    CARD_Offsets offsets;
    errors = CARD_Begin(slotID,&fixed);
    if (errors != CARD_ERROR_NONE)
    {
        GCARSErrorNum("Card slot failed to init",errors);
        free(buffer);
        return;
    }

    memset(&file,0,sizeof(file));
    strcpy(&file.filename[0],GCARS_FILENAME);
    strcpy(&file.gamecode[0],"ARCS");
    strcpy(&file.company[0],"FS");

    errors = CARD_OpenFile(&file,&offsets);
    if (errors == CARD_ERROR_NONE)
    {
        errors = CARD_DeleteFile(&file);
        if (errors != CARD_ERROR_NONE)
        {
            GCARSErrorNum("Could not delete old file",errors);
            free(buffer);
            return;
        }
    }

    file.banner_fmt   = 0x04;
    file.time         = 0;
    file.icon_addr    = 0x00000046;
    file.icon_fmt     = 0x0001;
    file.icon_speed   = 0x0002;
    file.attrib       = CARD_ATTRIB_PUBLIC;
    file.comment_addr = 0;
    file.size         = size;
    CARD_GetOffsets(&file,&offsets);

    errors = CARD_CreateFile(&file,&offsets);
    if (errors != CARD_ERROR_NONE)
    {
        GCARSErrorNum("Could not create new file",errors);
        free(buffer);
        return;
    }
    errors = CARD_WriteFile(&file,buffer_orig);
    free(buffer_orig);
    if (errors != CARD_ERROR_NONE)
    {
        GCARSErrorNum("Could not write file",errors);
        return;
    }
}

/* GCARS Save File Format:
 * u32 numgames
 *   char name[0x40]
 *   u32 gameid
 *   u32 numaddress[4]
 *   u32 addresses[4][numaddress[...] * 2]
 *   u32 padaddress[4]
 *   u32 numcodes
 *     u32 numlines
 *     u32 enabled
 *     char name[0x40]
 *     u32 lines[numlines * 2]
 */