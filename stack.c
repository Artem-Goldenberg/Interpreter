#include "stack.h"
#include "runtime/runtime.h"
#include "runtime/gc.h"

#define Up(p) p = (size_t)((size_t*)p + 1)
#define Down(p) p = (size_t)((size_t*)p - 1)

extern size_t __gc_stack_top, __gc_stack_bottom;
static size_t* fp;

void initStack(void) {
    __gc_init();

    __gc_stack_bottom = (size_t)malloc(STACK_SIZE);
    __gc_stack_top = __gc_stack_bottom + STACK_SIZE;
    fp = (size_t*)__gc_stack_top;
}

void push(size_t value) {
    if (__gc_stack_top == __gc_stack_bottom)
        failure("Push to a full stack");

    Down(__gc_stack_top);
    *(size_t*)__gc_stack_top = value;
}

size_t pop(void) {
    if (__gc_stack_top == (size_t)fp)
        failure("Pop from an empty stack");

    size_t result = *(size_t*)__gc_stack_top;
    Up(__gc_stack_top);

    return result;
}

void destroyStack(void) {
    __shutdown();
}
