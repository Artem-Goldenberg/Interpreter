#include <stdio.h>
#include "stack.h"
#include "language.h"
#include "bytereader.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./main <bytefile>");
        return 1;
    }

    dumpFile(argv[1]);
    initStack(globalsCount());

    byte* code = (byte*)codeAt(0);
    while (code) code = interpret(code);

    destroyStack();
    deleteFile();

    return 0;
}
