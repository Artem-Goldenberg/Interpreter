#include <stdio.h>
#include "stack.h"
#include "language.h"
#include "bytereader.h"

byte* code;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./main <bytefile>");
        return 1;
    }

    dumpFile(argv[1]);
    initStack();

    code = (byte*)codeAt(0);
    while (code) 
        interpret();

    destroyStack();
    deleteFile();

    return 0;
}
