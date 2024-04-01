#ifndef bytereader_h
#define bytereader_h

/// from https://github.com/PLTools/Lama/blob/1.10/byterun/byterun.c

#include "runtime/runtime.h"

/* The unpacked representation of bytecode file */
typedef struct {
    char *string_ptr;              /* A pointer to the beginning of the string table */
    int  *public_ptr;              /* A pointer to the beginning of publics table    */
    char *code_ptr;                /* A pointer to the bytecode itself               */
    int   stringtab_size;          /* The size (in bytes) of the string table        */
    int   global_area_size;        /* The size (in words) of global area             */
    int   public_symbols_number;   /* The number of public symbols                   */
    char  buffer[0];
} bytefile;

int globalsCount(void);

char* codeAt(int pos);

/* Gets a string from a string table by an index */
char* getString(int pos);

/* Gets a name for a public symbol */
char* publicName(int i);

/* Gets an offset for a publie symbol */
int publicOffset(int i);

/* Reads a binary bytecode file by name and unpacks it */
void dumpFile(char *fname);

/* Call when you are done */
void deleteFile(void);

#endif /* bytereader_h */
