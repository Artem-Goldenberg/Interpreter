#ifndef stack_h
#define stack_h

#define STACK_SIZE (1024 * 1024 * 1024)

#include <stddef.h>

void initStack(int globalsCount);
void destroyStack(void);

#endif /* stack_h */
