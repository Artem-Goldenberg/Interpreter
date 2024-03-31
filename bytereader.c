#include "bytereader.h"
#include <stdio.h>

static bytefile* current;

int* globalAt(int pos) {
    return current->global_ptr + pos;
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

    fclose (f);

    file->string_ptr  = &file->buffer [file->public_symbols_number * 2 * sizeof(int)];
    file->public_ptr  = (int*) file->buffer;
    file->code_ptr    = &file->string_ptr [file->stringtab_size];
    file->global_ptr  = (int*) malloc (file->global_area_size * sizeof (int));

    current = file;
}
