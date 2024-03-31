#ifndef Set_h
#define Set_h

#include "Array.h"

typedef struct {
    array *buckets;
    int numBuckets;
    HashFunction hash;
    CompareFunction cmp;
} Set;

void initSet(Set *atPointer, size_t withElementSize, size_t numberOfBuckets,
             HashFunction usingHash, CompareFunction andOrder, FreeFunction freeFunction);

Set *newSet(size_t withElementSize, size_t numberOfBuckets,
            HashFunction usingHashing, CompareFunction andOrder, FreeFunction freeFunction);

int countSet(Set *set);

void *addTo(Set *theSet, const void *anElementWithAddress);
bool removeFrom(Set *theSet, const void *anElementWithAddress);
void *searchSet(Set *toSearch, const void *forElementAtAddress);

void mapSet(Set *set, MapFunction withMapping, void *usingAdditionalData);
void clearSet(Set *set);

void deinitSet(Set *set);
void deleteSet(Set *set);

#endif /* Set_h */
