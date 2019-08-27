#ifndef __exception_h
#define __exception_h

#include "context.h"

void exception_init();
void exception_set_handler(int exception, void (*handler)(int, struct context_s*));
void exception_close();

#endif
