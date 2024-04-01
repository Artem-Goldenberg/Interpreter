#include "bytereader.h"
#include <stdio.h>

static bytefile* current;

int globalsCount(void) {
    return current->global_area_size;
}

char* codeAt(int pos) {
    return current->code_ptr + pos;
}

char* getString(int pos) {
    return current->string_ptr + pos;
}

char* publicName(int i) {
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

void dumpFile(char *fname) {
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

    current = file;
}
