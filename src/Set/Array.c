#include "Array.h"

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#define DEFAULT_CAPACITY 10

#define arrElem(v, index) (((char*)(v->data)) + (index) * (v->width))

void initArray(array *arr, size_t width, size_t cap, FreeFunction freefn) {
    assert(width > 0);
    if (cap == 0) cap = DEFAULT_CAPACITY;
    
    arr->data = calloc(cap, width);
    arr->count = 0;
    arr->capacity = (int)cap;
    arr->width = (int)width;
    arr->freefn = freefn;
}

void initArrayWithBuffer(array* arr, size_t width, void *buffer, size_t count) {
    assert(width > 0);
    assert(buffer);
    
    arr->data = buffer;
    arr->count = (int)count;
    arr->capacity = (int)count;
    arr->width = (int)width;
    arr->freefn = NULL;
}

array *newArray(size_t width, size_t cap, FreeFunction freefn) {
    array *arr = malloc(sizeof(array));
    initArray(arr, width, cap, freefn);
    return arr;
}

array *newArrayWithBuffer(size_t width, void *buffer, size_t count) {
    array *arr = malloc(sizeof(array));
    initArrayWithBuffer(arr, width, buffer, count);
    return arr;
}

int countArray(array *arr) { return arr->count; }

void *getElement(array *arr, int i) { return arrElem(arr, i); }

static void grow(array* arr) {
    void *reallocated = realloc(arr->data, arr->width * arr->capacity * 2);
    assert(reallocated);
    arr->capacity *= 2;
    arr->data = reallocated;
}

void append(array *arr, const void *elem) {
    if (arr->count >= arr->capacity) grow(arr);
    memcpy(arrElem(arr, arr->count), elem, arr->width);
    arr->count++;
}

void insert(array *arr, const void *elem, int i) {
    assert(i >= 0);
    assert(i <= arr->count);
    if (arr->count >= arr->capacity) grow(arr);
    memmove(arrElem(arr, i + 1), arrElem(arr, i), (arr->count - i) * arr->width);
    memcpy(arrElem(arr, i), elem, arr->width);
    arr->count++;
}

/// https://stackoverflow.com/a/39100135
static int lowerBound(array* arr, const void* x, CompareFunction cmp) {
    int l = 0, h = arr->count;
    while (l < h) {
        int mid =  l + (h - l) / 2;
        if (cmp(x, arrElem(arr, mid)) <= 0) h = mid;
        else l = mid + 1;
    }
    return l;
}

int insertSorted(array *arr, const void *elem, CompareFunction cmp) {
    int i = lowerBound(arr, elem, cmp);
    insert(arr, elem, i);
    return i;
}

void pop(array *arr, int i) {
    if (arr->freefn) arr->freefn(arrElem(arr, i));
    memmove(arrElem(arr, i), arrElem(arr, i + 1), (arr->count - i - 1) * arr->width);
    arr->count--;
}

void mapArray(array *arr, MapFunction map, void *auxData) {
    assert(map);
    for (int i = 0; i < arr->count; ++i)
        map(arrElem(arr, i), auxData);
}

void clearArray(array *arr) {
    if (arr->freefn)
        for (int i = 0; i < arr->count; ++i)
            arr->freefn(arrElem(arr, i));
    arr->count = 0;
}

static void *lsearch(const void *key, void *base, int count, int width, CompareFunction searchfn) {
    for (int i = 0; i < count; ++i)
        if (searchfn((char*)base + i * width, key) == 0)
            return base + i * width;
    return NULL;
}

int searchArray(array *arr, const void *key, CompareFunction cmp, bool isSorted) {
    assert(key);
    assert(cmp);
    char *found;
    if (isSorted)
        found = bsearch(key, arr->data, arr->count, arr->width, cmp);
    else
        found = lsearch(key, arr->data, arr->count, arr->width, cmp);
    if (found)
        return (int)(found - (char*)arr->data) / arr->width;
    return ARRAY_NOT_FOUND;
}

void sortArray(array *arr, CompareFunction cmp) {
    assert(cmp);
    qsort(arr->data, arr->count, arr->width, cmp);
}

void deinitArray(array *arr) {
    if (arr->freefn)
        for (int i = 0; i < arr->count; ++i)
            arr->freefn(arrElem(arr, i));
    free(arr->data);
}

void deleteArray(array *arr) {
    deinitArray(arr);
    free(arr);
}
