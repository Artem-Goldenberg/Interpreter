#include "language.h"
#include "stack.h"
#include "bytereader.h"
#include "runtime/runtime_common.h"
#include "runtime/runtime.h"

#define ASSERT_UNBOXED(memo, x) \
if (!UNBOXED(x)) failure("unboxed value expected in %s\n", memo);

static int compute(byte code, int a, int b) {
    switch (code) {
    case Plus: return a + b;
    case Minus: return a - b;
    case Multiply: return a * b;
    case Divide: return a / b;
    case Mod: return a % b;
    case Less: return a < b;
    case LessEqual: return a <= b;
    case More: return a > b;
    case MoreEqual: return a >= b;
    case Equal: return a == b;
    case NotEqual: return a != b;
    case And: return a && b;
    case Or: return a || b;
    default:
        failure("Unknown operation: '%d'", code);
    }
}

void binop(byte code) {
    int a = UNBOX(pop());
    int b = UNBOX(pop());

    push(BOX(compute(code, a, b)));
}

#define GetInt code += sizeof(int), *((int*)code - 1)

void value1(byte* code) {
    switch (*code++) {
        case Const:
            push(BOX(GetInt));
            break;
        case String:
            push()
        case Sexp:

        default:
            break;
    }
}
