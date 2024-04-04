/* Lama SM Bytecode interpreter */
#include "language.h"
#include "bytereader.h"
#include "disassemble.h"
#include "Set/Set.h"

typedef struct {
    const byte* code;
    int codeSize;

    char* string;
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
    for (int i = 0; i < n; ++i)
        printf("%2d\t%s\n", base[i].count, base[i].string);

    free(base);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ./analyser <bytecode file>\n");
        return 1;
    }
    dumpFile(argv[1]);

    Set counts;
    initSet(&counts, sizeof(Entry), 1001, entryHash, entryCmp, entryFree);

    setCode((const byte*)codeAt(0));

    Entry entry = {.count = 1, .code = getCode()};

    while ((entry.string = disassemble())) {
        entry.codeSize = (int)(getCode() - entry.code);
        Entry* found = searchSet(&counts, &entry);

        if (!found) addTo(&counts, &entry);
        else found->count++;

        entry.code = getCode();
    }

    printCounts(&counts);

    deinitSet(&counts);
    deleteFile();

    return 0;
}

/// https://stackoverflow.com/questions/7666509/hash-function-for-string
static int entryHash(const void* e, int buckets) {
    char* str = ((Entry*)e)->string;
    int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return abs(hash) % buckets;
}

static int entryCmp(const void* e1, const void* e2) {
    const Entry* a = (const Entry*)e1;
    const Entry* b = (const Entry*)e2;
    if (a->codeSize != b->codeSize) return a->codeSize - b->codeSize;
    return memcmp(a->code, b->code, a->codeSize);
}

static void entryFree(void* e) {
    free(((Entry*)e)->string);
}
