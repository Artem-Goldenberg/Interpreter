#ifndef stack_critical_h
#define stack_critical_h

#include "runtime/runtime.h"

extern size_t __gc_stack_top, __gc_stack_bottom;
extern size_t* fp;
extern const size_t* stackEnd;

#define Up(p, n) p = (size_t)((size_t*)p + (n))
#define Down(p, n) p = (size_t)((size_t*)p - (n))

inline void push(size_t value) {
    if (__gc_stack_top == (size_t)stackEnd)
        failure("Push to a full stack\n");

    Down(__gc_stack_top, 1);
    *(size_t*)__gc_stack_top = value;
}

inline size_t* top(void) {
    return (size_t*)__gc_stack_top;
}

inline size_t* bot(void) {
    return (size_t*)((size_t)stackEnd + STACK_SIZE);
}

inline size_t pop(void) {
    if (__gc_stack_top == __gc_stack_bottom)
        failure("Pop from an empty stack\n");

    size_t result = *(size_t*)__gc_stack_top;
    Up(__gc_stack_top, 1);

    return result;
}

inline void reverse(int n) {
    assert(n >= 0);
    if (__gc_stack_top > __gc_stack_bottom - sizeof(size_t) * n)
        failure("Not enough elements on stack to reverse\n");

    size_t* stack = (size_t*)__gc_stack_top;
    for (int i = 0; i < n / 2; ++i) {
        size_t tmp = stack[i];
        stack[i] = stack[n - i - 1];
        stack[n - i - 1] = tmp;
    }
}

inline void discard(int n) {
    assert(n >= 0);
    if (__gc_stack_top > __gc_stack_bottom - sizeof(size_t) * n)
        failure("Discarding from an empty stack\n");

    Up(__gc_stack_top, n);
}

inline void resetToFrame(void) {
    __gc_stack_top = (size_t)fp;
}

inline void beginFrame(void) {
    fp = (size_t*)__gc_stack_top;
}

inline void setFrame(size_t* ptr) {
    fp = ptr;
}

inline size_t* getFrame(void) {
    return fp;
}

#endif /* stack_critical_h */
