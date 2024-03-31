#include "stack.h"
#include "runtime/runtime.h"

#include <assert.h>

#define Up(p, n) p = (size_t)((size_t*)p + (n))
#define Down(p, n) p = (size_t)((size_t*)p - (n))

const size_t __start_custom_data, __stop_custom_data;
extern size_t __gc_stack_top, __gc_stack_bottom;
static size_t* fp;
static size_t* stackEnd;

extern void __gc_init(void);

void initStack(void) {
    stackEnd = (size_t*)malloc(STACK_SIZE);
    if (!stackEnd)
        failure("Unable to allocate stack memory\n");

    __gc_stack_top = __gc_stack_bottom = (size_t)stackEnd + STACK_SIZE;
    __gc_init();

    fp = (size_t*)__gc_stack_top;

    push(0); // 0 arg
    push(0); // stop interpreting
    push(2); // 2 args
}

void push(size_t value) {
    if (__gc_stack_top == (size_t)stackEnd)
        failure("Push to a full stack\n");

//    printf("Pushing '%zu'\n", value);
    Down(__gc_stack_top, 1);
    *(size_t*)__gc_stack_top = value;
}

size_t* top(void) {
    return (size_t*)__gc_stack_top;
}

size_t pop(void) {
    if (__gc_stack_top == __gc_stack_bottom)
        failure("Pop from an empty stack\n");

    size_t result = *(size_t*)__gc_stack_top;
//    printf("Popping '%zu'\n", result);
    Up(__gc_stack_top, 1);

    return result;
}

void reverse(int n) {
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

void discard(int n) {
    assert(n >= 0);
    if (__gc_stack_top > __gc_stack_bottom - sizeof(size_t) * n)
        failure("Discarding from an empty stack\n");

    Up(__gc_stack_top, n);
}

void resetToFrame(void) {
    __gc_stack_top = (size_t)fp;
}

void beginFrame(void) {
    fp = (size_t*)__gc_stack_top;
}

void setFrame(size_t* ptr) {
    fp = ptr;
}

size_t* getFrame(void) {
    return fp;
}

void destroyStack(void) {
    free(stackEnd);
//    __shutdown();
}
