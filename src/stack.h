#ifndef stack_h
#define stack_h

#define STACK_SIZE (1024 * 1024 * 1024)

#include <stddef.h>

void initStack(int globalsCount);

void push(size_t value);
size_t pop(void);
size_t* top(void);
size_t* bot(void);

void reverse(int n);
void discard(int n);

void resetToFrame(void);
void beginFrame(void);
size_t* getFrame(void);
void setFrame(size_t* fp);

void destroyStack(void);

#endif /* stack_h */
