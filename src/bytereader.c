#include "bytereader.h"
#include <stdio.h>

bytefile* currentFile;

void deleteFile(void) {
    free(currentFile);
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

    currentFile = file;
}
