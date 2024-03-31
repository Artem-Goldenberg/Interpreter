#ifndef stack_h
#define stack_h

#define STACK_SIZE (1024 * 1024 * 1024)

#include <stddef.h>

void initStack(void);

void push(size_t value);
size_t pop(void);
size_t* top(void);

void reverse(int n);
void discard(int n);

void resetToFrame(void);
void beginFrame(void);
size_t* getFrame(void);
void setFrame(size_t* fp);

void destroyStack(void);

#endif /* stack_h */
