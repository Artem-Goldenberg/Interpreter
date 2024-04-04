#include "stack.h"
#include "stack-critical.h"
#include "runtime/runtime.h"

#include <assert.h>

#define Up(p, n) p = (size_t)((size_t*)p + (n))
#define Down(p, n) p = (size_t)((size_t*)p - (n))

const size_t __start_custom_data, __stop_custom_data;
extern size_t __gc_stack_top, __gc_stack_bottom;

size_t* fp;
const size_t* stackEnd;

extern void __gc_init(void);

void initStack(int globalsCount) {
    stackEnd = (size_t*)malloc(STACK_SIZE);
    if (!stackEnd)
        failure("Unable to allocate stack memory\n");

    __gc_stack_top = __gc_stack_bottom = (size_t)stackEnd + STACK_SIZE;
    __gc_init();

    fp = (size_t*)__gc_stack_top;

    // allocate for global variables
    Down(__gc_stack_top, globalsCount);

    push(0); // 0 arg
    push(0); // stop interpreting
    push(2); // 2 args
}

void destroyStack(void) {
    free((size_t*)stackEnd);
}
