#include "language.h"
#include "stack.h"
#include "bytereader.h"

//#include "runtime/runtime_common.h"
#include "runtime/runtime.h"
//#ifdef __linux__
//#include "runtime/runtime.h"
//#else
//#include "runtime-macOS/runtime_common.h"
//#include "runtime-macOS/runtime.h"
//#endif

#define LowBits(x) ((x) & 0x0F)
#define HighBits(x) (((x) & 0xF0) >> 4)

extern byte* code;

const Handler handlers[] = {
    [0] = binop,
    [1] = other,
    [2] = boxed,
    [3] = boxed,
    [4] = boxed,
    [5] = control,
    [6] = pattern,
    [7] = call
};

void interpret(void) {
    byte h = HighBits(*code);
    if (h > 7) {
        fprintf(stderr, "Anomaly detected\n");
        return;
    }
    handlers[HighBits(*code)]();
}

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
    default: failure("Unknown operation: '%d'", code);
    }
}

void binop(void) {
    int b = UNBOX(pop());
    int a = UNBOX(pop());

    push(BOX(compute(LowBits(*code++), a, b)));
}

#define GetInt (code += sizeof(int), *((int*)code - 1))
#define GetString getString(GetInt)

void other(void) {
    switch (LowBits(*code++)) {
        case Const:
            push(BOX(GetInt));
            break;
        case String:
            push((size_t)Bstring(GetString));
            fprintf(stderr, "string done\n");
            break;
        case Sexp: {
            char* name = GetString;
            int nargs = GetInt;

            reverse(nargs);
            void* sexp = Bsexp(BOX(nargs + 1), LtagHash(name), top());

            discard(nargs);
            push((size_t)sexp);
        } break;
        case Sti: failure("Unused sti code: '%d'", code[-1]);
        case Sta: {
            void* value = (void*)pop();
            int index = (int)pop();

            if (UNBOXED(index)) 
                push((size_t)Bsta(value, index, (void*)pop()));
            else
                push((size_t)Bsta(value, index, NULL));
        } break;
        case Jmp:
            code = (byte*)codeAt(GetInt);
            break;
        case End: {
            size_t ret = pop();

            resetToFrame();
            setFrame((size_t*)pop()); // old fp is saved on the stack

            int nargs = (int)pop();
            code = (byte*)pop(); // old code ptr is also here

            discard(nargs);
            push(ret);
            fprintf(stderr, "End done\n");
        } break;
        case Ret:
            failure("Instruction not supported: '%d'", code[-1]);
        case Drop:
            pop();
            break;
        case Dup: {
            size_t value = pop();
            push(value);
            push(value);
        } break;
        case Swap:
            reverse(2);
            break;
        case Elem: {
            int i = (int)pop();
            void* a = (void*)pop();
            push((size_t)Belem(a, i));
        } break;
        default: failure("Unknown low code: %d", code[-1]);
    }
}

static size_t* locationAddress(byte code, int i) {
    switch (code) {
        case Global: return (size_t*)globalAt(i);
        case Local: return getFrame() - i - 1;
        case Argument: return getFrame() + i + 3;
        case Closure: {
            int nargs = (int)getFrame()[1];
            // TODO: incompatible size_t and int
            return BelemClosure((void*)getFrame()[nargs + 2], BOX(i));
        }
        default: failure("Invalid location code: '%d'", code);
    }
}

void boxed(void) {
    byte c = *code++;
    size_t* loc = locationAddress(LowBits(c), GetInt);
    switch (HighBits(c)) {
        case LD:
            push(*loc);
            break;
        case LDA:
            push((size_t)loc);
            break;
        case ST:
            *loc = pop();
            push(*loc);
            break;
        default: failure("Unknown higher bits in: '%d'", c);
    }
}

void control(void) {
    switch (LowBits(*code++)) {
        case CJmpZero: {
            byte* dest = (byte*)codeAt(GetInt);
            if (UNBOX(pop()) == 0)
                code = dest;
        } break;
        case CJmpNotZero: {
            byte* dest = (byte*)codeAt(GetInt);
            if (UNBOX(pop()) != 0)
                code = dest;
        } break;
        case Begin: {
            fprintf(stderr, "Begin\n");
            push((size_t)getFrame());
            beginFrame();
            /* int nargs = */ GetInt;
            int nlocals = GetInt;
            for (int i = 0; i < nlocals; ++i)
                push(BOX(0));
        } break;
        case CBegin: {
            push((size_t)getFrame());
            beginFrame();
            /* int nargs = */ GetInt;
            int nlocals = GetInt;
            for (int i = 0; i < nlocals; ++i)
                push(BOX(0));
        } break;
        case BClosure: {
            int section = GetInt;
            int nbinds = GetInt;
            int* binds = calloc(nbinds, sizeof(int));
            for (int i = 0; i < nbinds; ++i) {
                byte loc = LowBits(*code++);
                binds[i] = (int)*locationAddress(loc, GetInt);
            }
            push((size_t)Bclosure(BOX(nbinds), codeAt(section), binds));
            free(binds);
        } break;
        case CallC: {
            fprintf(stderr, "CallC\n");
            int nargs = GetInt;
            reverse(nargs);
            byte* loc = Belem((void*)*top(), BOX(0));
            push((size_t)code);
            push(nargs + 1);
            code = loc;
        } break;
        case Call: {
            fprintf(stderr, "Call\n");
            int section = GetInt;
            int nargs = GetInt;
            reverse(nargs);
            fprintf(stderr, "reverse done, %d, %d\n", section, nargs);
            push((size_t)code);
            push(nargs);
            fprintf(stderr, "push done\n");
            code = (byte*)codeAt(section);
        } break;
        case Tag: {
            int hash = LtagHash(GetString);
            push(Btag((void*)pop(), hash, GetInt));
        } break;
        case Array: {
            int n = GetInt;
            push(Barray_patt((void*)pop(), BOX(n)));
        } break;
        case Fail: {
            int a = GetInt, b = GetInt;
            failure("Something went wrong: %d, %d", a, b);
        } break;
        case Line: GetInt; 
            break;
        default: failure("Unknown low code: %d", code[-1]);
    }
}

static int (*const patternValidators[])(void*) = {
    [PString] = Bstring_tag_patt,
    [PArray] = Barray_tag_patt,
    [PSexp] = Bsexp_tag_patt,
    [PBoxed] = Bboxed_patt,
    [PUnboxed] = Bunboxed_patt,
    [PClosure] =  Bclosure_tag_patt
};

void pattern(void) {
    size_t* x = (size_t*)pop();
    byte l = LowBits(*code++);
    if (l < 0 || l > PClosure)
        failure("Invalid pattern for: '%d'", l);

    int result;
    // one corner case
    if (l == 0) result = Bstring_patt(x, (void*)pop());
    else result = patternValidators[l](x);

    push(result);
}

static size_t callResult(byte l) {
    switch (l) {
        case LRead: return Lread();
        case LWrite: return Lwrite((int)pop());
        case LLenght: return Llength((void*)pop());
        case LString: return (size_t)Lstring((void*)pop());
        case BArray: {
            int n = GetInt;
            reverse(n);
            void* result = Barray(BOX(n), (void*)top());
            discard(n);
            return (size_t)result;
        }
        default: failure("Unknown lower bits: '%d'", l);
    }
}

void call(void) {
    size_t result = callResult(LowBits(*code++));
    push(result);
}
