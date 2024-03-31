#include "Set.h"

#include <stdlib.h>
#include <assert.h>

#define BUCKET_SIZE 10

void initSet(Set *set, size_t width, size_t numBuckets,
             HashFunction hash, CompareFunction cmp, FreeFunction freefn) {
    assert(width > 0);
    assert(numBuckets > 0);
    assert(hash);
    assert(cmp);
    
    set->buckets = calloc(numBuckets, sizeof(array));
    set->numBuckets = (int)numBuckets;
    for (int i = 0; i < set->numBuckets; ++i)
        initArray(set->buckets + i, width, BUCKET_SIZE, freefn);
    
    set->hash = hash;
    set->cmp = cmp;
}

Set *newSet(size_t width, size_t numBuckets, HashFunction hash, CompareFunction cmp, FreeFunction freefn) {
    Set *set = malloc(sizeof(Set));
    initSet(set, width, numBuckets, hash, cmp, freefn);
    
    return set;
}

int countSet(Set *set) {
    int count = 0;
    for (int i = 0; i < set->numBuckets; ++i)
        count += countArray(set->buckets + i);
    return count;
}

static array *bucketWith(Set *set, const void *elem) {
    assert(elem);
    
    int hash = set->hash(elem, set->numBuckets);
    
    assert(hash >= 0);
    assert(hash < set->numBuckets);
    
    return set->buckets + hash;
}

void *addTo(Set *set, const void *elem) {
    array *bucket = bucketWith(set, elem);
    
    int found = searchArray(bucket, elem, set->cmp, true);
    if (found != ARRAY_NOT_FOUND) return NULL;

    int index = insertSorted(bucket, elem, set->cmp);
    // inserted element is the last
    return getElement(bucket, index);
}

bool removeFrom(Set *set, const void *elem) {
    array *bucket = bucketWith(set, elem);
    
    int found = searchArray(bucket, elem, set->cmp, true);
    if (found == ARRAY_NOT_FOUND) return false;
    
    pop(bucket, found);
    
    return true;
}

void mapSet(Set *set, MapFunction withMapping, void *usingAdditionalData) {
    for (int i = 0; i < set->numBuckets; ++i)
        mapArray(set->buckets + i, withMapping, usingAdditionalData);
}

void clearSet(Set *set) {
    for (int i = 0; i < set->numBuckets; ++i)
        clearArray(set->buckets + i);
}

void *searchSet(Set *set, const void *elem) {
    array *bucket = bucketWith(set, elem);
    
    int found = searchArray(bucket, elem, set->cmp, true);
    if (found == ARRAY_NOT_FOUND) return NULL;

    return getElement(bucket, found);
}

void deinitSet(Set *set) {
    for (int i = 0; i < set->numBuckets; ++i)
        deinitArray(set->buckets + i);
    free(set->buckets);
}

void deleteSet(Set *set) {
    deinitSet(set);
    free(set);
}
