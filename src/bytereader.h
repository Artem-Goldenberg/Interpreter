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

int globalsCount(void);

const char* codeEnd(void);
const char* codeAt(int pos);

/* Check if `code` is safe to dereference */
void checkBounds(const char* code);

/* Extracts int from code, signals error if outside bounds, returns new code position */
const char* getInt(const char* code);

/* Gets a string from a string table by an index */
const char* getString(int pos);

/* Gets a name for a public symbol */
const char* publicName(int i);

/* Gets an offset for a publie symbol */
int publicOffset(int i);

/* Reads a binary bytecode file by name and unpacks it */
void dumpFile(const char *fname);

/* Call when you are done */
void deleteFile(void);

#endif /* bytereader_h */
