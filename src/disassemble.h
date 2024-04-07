#ifndef disassemle_h
#define disassemle_h

#include "language.h"
#include <stdio.h>

/// To avoid linking with runtime
void failure(char* message, ...);

const byte* disassemble(const byte* code, FILE* out);

#endif /* disassemle_h */
