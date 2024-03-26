#ifndef stack_h
#define stack_h

#define STACK_SIZE (1024 * 1024)

#include <stdint.h>

void initStack(void);

void push(size_t value);
size_t pop(void);

void destroyStack(void);

#endif /* stack_h */
