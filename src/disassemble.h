#ifndef disassemle_h
#define disassemle_h

#include "language.h"

/// To avoid linking with runtime
void failure(char* message, ...);
void setCode(byte* code);
char* disassemble(void);

#endif /* disassemle_h */
