#include <stdio.h>
#include <stdlib.h>
//#include "bytereader.h"
//#include "language.h"

int a[] = {
    [1] = 34,
    
};

int main(int argc, char* argv[]) {
//    if (argc != 2) {
//        fprintf(stderr, "Usage: ./main <bytefile>");
//        return 1;
//    }
    int b;
    scanf("%d", &b);

    int* a = malloc(100);

    //    bytefile* f = read_file(argv[1]);

    free(a);

    return 0;
}
