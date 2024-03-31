#ifndef Array_h
#define Array_h

#include "GenericFunctions.h"
#include <stddef.h>
#include <stdbool.h>

#define ARRAY_NOT_FOUND -1

typedef struct {
    void *data;
    int width;
    int count;
    int capacity;
    FreeFunction freefn;
} array;

void initArray(array *atPointer, size_t withElementsOfSize, size_t capacity, FreeFunction usingFreeFunction);
void initArrayWithBuffer(array *atPointer, size_t withElementsOfSize, void *bytesBuffer, size_t elementsAmount);

array *newArray(size_t withElementsOfSize, size_t capacity, FreeFunction usingFreeFunction);
array *newArrayWithBuffer(size_t withElementsOfSize, void *bytesBuffer, size_t elementsAmount);

int countArray(array *thearray);
void *getElement(array *ofarray, int atIndex);

void append(array *array, const void *withElementFromAddress);
void insert(array *intoarray, const void *fromAddress, int atIndex);
int insertSorted(array *intoarray, const void *fromAddress, CompareFunction usingOrder);
void pop(array *array, int atIndex);

void mapArray(array *array, MapFunction withMapping, void *usingAdditionalData);
void clearArray(array *array);

int searchArray(array *array, const void *forElementAtAddress, CompareFunction usingOrderFunction, bool isSorted);
void sortArray(array *array, CompareFunction usingOrderFunction);

void deinitArray(array *array);
void deleteArray(array *array);

#endif /* Array_h */
