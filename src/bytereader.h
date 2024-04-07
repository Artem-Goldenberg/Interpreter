#ifndef bytereader_h
#define bytereader_h

/// from https://github.com/PLTools/Lama/blob/1.10/byterun/byterun.c

#include "runtime/runtime.h"

/* The unpacked representation of bytecode file */
typedef struct {
    int   code_size;               /* Size of code section in bytes */
    char *string_ptr;              /* A pointer to the beginning of the string table */
    int  *public_ptr;              /* A pointer to the beginning of publics table    */
    char *code_ptr;                /* A pointer to the bytecode itself               */
    int   stringtab_size;          /* The size (in bytes) of the string table        */
    int   global_area_size;        /* The size (in words) of global area             */
    int   public_symbols_number;   /* The number of public symbols                   */
    char  buffer[0];
} bytefile;

extern bytefile* currentFile;

static inline int globalsCount(void) {
    return currentFile->global_area_size;
}

static inline const char* codeEnd(void) {
    return currentFile->code_ptr + currentFile->code_size;
}

static inline const char* codeAt(int pos) {
    if (pos >= currentFile->code_size)
        failure("Invalid code offset: '%d'\n", pos);
    return currentFile->code_ptr + pos;
}

static inline void checkBounds(const char* code) {
    if (code < currentFile->code_ptr)
        failure("Code pointer broke a lower bound\n");
    if (code - currentFile->code_ptr >= currentFile->code_size)
        failure("Code pointer broke an upper bound\n");
}

static inline const char* getInt(const char* code) {
    code += sizeof(int);
    checkBounds(code);
    return code;
}

static inline const char* getString(int pos) {
    if (pos >= currentFile->stringtab_size)
        failure("Invalid string table offset: '%d'\n", pos);
    return currentFile->string_ptr + pos;
}

static inline const char* publicName(int i) {
    return getString(currentFile->public_ptr[i*2]);
}

static inline int publicOffset(int i) {
    return currentFile->public_ptr[i*2+1];
}

/* Reads a binary bytecode file by name and unpacks it */
void dumpFile(const char *fname);

/* Call when you are done */
void deleteFile(void);

#endif /* bytereader_h */
