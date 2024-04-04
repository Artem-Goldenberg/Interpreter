#include "bytereader.h"
#include <stdio.h>

static bytefile* current;

int globalsCount(void) {
    return current->global_area_size;
}

const char* codeEnd(void) {
    return current->code_ptr + current->code_size;
}

const char* codeAt(int pos) {
    if (pos >= current->code_size)
        failure("Invalid code offset: '%d'\n", pos);
    return current->code_ptr + pos;
}

void checkBounds(const char* code) {
    if (code < current->code_ptr)
        failure("Code pointer broke a lower bound\n");
    if (code - current->code_ptr >= current->code_size)
        failure("Code pointer broke an upper bound\n");
}

const char* getInt(const char* code) {
    code += sizeof(int);
    checkBounds(code);
    return code;
}

const char* getString(int pos) {
    if (pos >= current->stringtab_size)
        failure("Invalid string table offset: '%d'\n", pos);
    return current->string_ptr + pos;
}

const char* publicName(int i) {
    return getString(current->public_ptr[i*2]);
}

int publicOffset(int i) {
    return current->public_ptr[i*2+1];
}

void deleteFile(void) {
    free(current);
}

static void badFile(const char* msg) {
    failure("Incorrect file format: %s\n", msg);
}

void dumpFile(const char *fname) {
    FILE *f = fopen (fname, "rb");
    long size;
    bytefile *file;

    if (f == 0) {
        failure ("%s\n", strerror (errno));
    }

    if (fseek (f, 0, SEEK_END) == -1) {
        failure ("%s\n", strerror (errno));
    }

    file = (bytefile*) malloc (sizeof(int)*4 + (size = ftell (f)));

    if (file == 0) {
        failure ("*** FAILURE: unable to allocate memory.\n");
    }

    rewind (f);

    if (size != fread (&file->stringtab_size, 1, size, f)) {
        failure ("%s\n", strerror (errno));
    }
    if (file->stringtab_size < 0) badFile("stringtab size is negative");
    if (file->public_symbols_number < 0) badFile("public symbols number is negative");
    if (file->global_area_size < 0) badFile("global area size is negative");

    const long words = size / sizeof(int);
    // they can't be more than a file size
    if (file->stringtab_size > size) badFile("stringtab size is too big");
    if (file->public_symbols_number > words) badFile("public symbols number is too big");
    if (file->global_area_size > words) badFile("global area size is too big");

    fclose (f);

    file->string_ptr  = &file->buffer [file->public_symbols_number * 2 * sizeof(int)];
    file->public_ptr  = (int*) file->buffer;
    file->code_ptr    = &file->string_ptr [file->stringtab_size];

    size_t codeOffset = file->public_symbols_number * 2 * sizeof(int) + file->stringtab_size;
    assert(size >= 3 * sizeof(int) + codeOffset);

    file->code_size = (int)(size - 3 * sizeof(int) - codeOffset);
    // for 3 ints before buffer    ^^^^^^^^^^^    and offset afterwards;

    current = file;
}
