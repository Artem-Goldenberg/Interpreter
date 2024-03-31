/* Lama SM Bytecode interpreter */
#include "language.h"
#include "bytereader.h"
#include "disassemle.h"
#include "Set/Set.h"

extern byte* code;

typedef struct {
    char* bytecode;
    int count;
} Entry;

int entryHash(const void* e, int buckets);
int entryCmp(const void* e1, const void* e2);
void entryFree(void* e);

void appendCount(void* entry, void* dump) {
    Entry** loc = dump;
    *(*loc)++ = *(Entry*)entry;
}

int frequencyCmp(const void* e1, const void* e2) {
    return ((Entry*)e2)->count - ((Entry*)e1)->count;
}

void printCounts(Set* counts) {
    int n = countSet(counts);
    Entry* dump = calloc(n, sizeof(Entry));
    Entry* base = dump;

    mapSet(counts, appendCount, &dump);

    qsort(base, n, sizeof(Entry), frequencyCmp);

    printf("Count\tBytecode\n");
    printf("------------------\n");
    for (int i = 0; i < n; ++i)
        printf("%2d\t%s\n", base[i].count, base[i].bytecode);

    free(base);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./analyser <bytecode file>");
        return 1;
    }
    dumpFile(argv[1]);

    Set counts;
    initSet(&counts, sizeof(Entry), 1001, entryHash, entryCmp, entryFree);

    code = (byte*)codeAt(0);

    Entry entry = {.count = 1};

    while (code) {
        entry.bytecode = disassemle();
        if (!entry.bytecode) break;

        Entry* found = searchSet(&counts, &entry);
        if (!found) addTo(&counts, &entry);
        else found->count++;
    }

    printCounts(&counts);

    deinitSet(&counts);
    deleteFile();

    return 0;
}

/// https://stackoverflow.com/questions/7666509/hash-function-for-string
int entryHash(const void* e, int buckets) {
    char* str = ((Entry*)e)->bytecode;
    int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return abs(hash) % buckets;
}
int entryCmp(const void* e1, const void* e2) {
    return strcmp(((Entry*)e1)->bytecode, ((Entry*)e2)->bytecode);
}
void entryFree(void* e) {
    free(((Entry*)e)->bytecode);
}
