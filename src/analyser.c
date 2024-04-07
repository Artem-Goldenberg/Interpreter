/* Lama SM Bytecode interpreter */
#include "language.h"
#include "bytereader.h"
#include "disassemble.h"
#include "Set/Set.h"

typedef struct {
    const byte* code;
    int codeSize;
    int count;
} Entry;

static int entryHash(const void* e, int buckets);
static int entryCmp(const void* e1, const void* e2);
static void entryFree(void* e);

static void appendCount(void* entry, void* dump) {
    Entry** loc = dump;
    *(*loc)++ = *(Entry*)entry;
}

static int frequencyCmp(const void* e1, const void* e2) {
    return ((Entry*)e2)->count - ((Entry*)e1)->count;
}

static void printCounts(Set* counts) {
    int n = countSet(counts);
    Entry* dump = calloc(n, sizeof(Entry));
    Entry* base = dump;

    mapSet(counts, appendCount, &dump);

    qsort(base, n, sizeof(Entry), frequencyCmp);

    printf("Count\tBytecode\n");
    printf("------------------\n");
    for (int i = 0; i < n; ++i) {
        printf("%2d\t", base[i].count);
        disassemble(base[i].code, stdout);
        printf("\n");
    }

    free(base);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./analyser <bytecode file>\n");
        return 1;
    }
    dumpFile(argv[1]);

    Set counts;
    initSet(&counts, sizeof(Entry), 1001, entryHash, entryCmp, NULL);

    const byte* code = (const byte*)codeAt(0);

    Entry entry = {.count = 1, .code = code};

    // just skip to the next code without printing
    while ((code = disassemble(code, NULL))) {
        entry.codeSize = (int)(code - entry.code);
        Entry* found = searchSet(&counts, &entry);

        if (!found) addTo(&counts, &entry);
        else found->count++;

        entry.code = code;
    }

    printCounts(&counts);

    deinitSet(&counts);
    deleteFile();

    return 0;
}

/// https://stackoverflow.com/questions/7666509/hash-function-for-string
static int entryHash(const void* e, int buckets) {
    Entry* x = (Entry*)e;
    int hash = 5381;

    for (int i = 0; i < x->codeSize; ++i)
        hash = ((hash << 5) + hash) + x->code[i];

    return abs(hash) % buckets;
}

static int entryCmp(const void* e1, const void* e2) {
    const Entry* a = (const Entry*)e1;
    const Entry* b = (const Entry*)e2;
    if (a->codeSize != b->codeSize) return a->codeSize - b->codeSize;
    return memcmp(a->code, b->code, a->codeSize);
}
