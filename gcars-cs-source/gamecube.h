#define FUZZIQER_TYPES
#include "types.h"

#include "video.h"
#include "color.h"
#include "memory.h"
#include "text.h"
#include "graphics.h"
#include "debugmenus.h"
//#include "filesys.h"

#include "time.h"
#include "cache.h"
#include "exi.h"

#include "bba.h"
//#include "fscard.h"
#include "card.h"
#include "sram.h"
#include "dvd.h"
#include "anaconda.h"
#include "pad.h"

#include "context.h"
#include "exception.h"
#include "irq.h"

//#include "fscardgraph.h"

// exi problems?
//normal: (control) = ((control) & 0x0405) | (0x80 << dev) | (freq << 4)
//sd:     (control) = ((control) & 0x0405) | (freq << 4)

