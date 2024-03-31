#ifndef GenericFunctions_h
#define GenericFunctions_h

typedef void (*MapFunction)(void *elementAddress, void *additionalData);

typedef int (*CompareFunction)(const void *firstElementAddress, const void *secondElementAddress);

typedef int (*HashFunction)(const void *elementAddress, int numBuckets);

typedef void (*FreeFunction)(void *elementAddress);

#endif /* GenericFunctions_h */
