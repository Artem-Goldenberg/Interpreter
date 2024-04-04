#include "language.h"
#include "stack.h"
#include "stack-critical.h"
#include "bytereader.h"

#include "runtime/runtime.h"

#define LowBits(x) ((x) & 0x0F)
#define HighBits(x) (((x) & 0xF0) >> 4)

static const byte* binop(const byte*);
static const byte* other(const byte*);
static const byte* boxed(const byte*);
static const byte* control(const byte*);
static const byte* pattern(const byte*);
static const byte* call(const byte*);

const byte* interpret(const byte* code) {
    checkBounds((char*)code);
    byte h = HighBits(*code);

    if (h == 15) return NULL; // termination code
    if (h > 7) {
        fprintf(stderr, "Anomaly detected\n");
        return NULL;
    }

     switch (h) {
         case 0: return binop(code);
         case 1: return other(code);
         case 2: case 3: case 4: return boxed(code);
         case 5: return control(code);
         case 6: return pattern(code);
         case 7: return call(code);
         default: failure("Unknown bytecode: %d", *code);
     }
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

static const byte* binop(const byte* code) {
    int b = UNBOX(pop());
    int a = UNBOX(pop());

    push(BOX(compute(LowBits(*code++), a, b)));
    return code;
}

#define GetInt (code = (byte*)getInt((char*)code), *((int*)code - 1))
#define GetString getString(GetInt)

static const byte* other(const byte* code) {
    switch (LowBits(*code++)) {
        case Const:
            push(BOX(GetInt));
            break;
        case String:
            push((size_t)Bstring((char*)GetString));
            break;
        case Sexp: {
            const char* name = GetString;
            int nargs = GetInt;

            reverse(nargs);
            void* sexp = Bsexp(BOX(nargs + 1), LtagHash((char*)name), (int*)top());

            discard(nargs);
            push((size_t)sexp);
        } break;
        case Sti: failure("Unused sti code: '%d'", code[-1]);
        case Sta: {
            void* value = (void*)pop();
            size_t index = pop();
            size_t p = UNBOXED(index) ? pop() : index;

            push((size_t)Bsta(value, (int)index, (void*)p));
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
    return code;
}

static size_t* locationAddress(byte code, int i) {
    switch (code) {
        case Global: return bot() - i;
        case Local: return getFrame() - i - 1;
        case Argument: return getFrame() + i + 3;
        case Closure: {
            int nargs = (int)getFrame()[1];
            return BelemClosure((void*)getFrame()[nargs + 2], BOX(i));
        }
        default: failure("Invalid location code: '%d'", code);
    }
}

static const byte* boxed(const byte* code) {
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
    return code;
}

static const byte* control(const byte* code) {
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
            push((size_t)Bclosure(BOX(nbinds), (char*)codeAt(section), binds));
            free(binds);
        } break;
        case CallC: {
            int nargs = GetInt;
            reverse(nargs);
            byte* loc = Belem((void*)top()[nargs], BOX(0));
            push((size_t)code);
            push(nargs + 1);
            code = loc;
        } break;
        case Call: {
            int section = GetInt;
            int nargs = GetInt;
            reverse(nargs);
            push((size_t)code);
            push(nargs);
            code = (byte*)codeAt(section);
        } break;
        case Tag: {
            int hash = LtagHash((char*)GetString);
            push(Btag((void*)pop(), hash, BOX(GetInt)));
        } break;
        case Array: {
            push(Barray_patt((void*)pop(), BOX(GetInt)));
        } break;
        case Fail: {
            int a = GetInt, b = GetInt;
            failure("Something went wrong: %d, %d", a, b);
        } break;
        case Line: GetInt; 
            break;
        default: failure("Unknown low code: %d", code[-1]);
    }
    return code;
}

static int (*const patternValidators[])(void*) = {
    [PString] = Bstring_tag_patt,
    [PArray] = Barray_tag_patt,
    [PSexp] = Bsexp_tag_patt,
    [PBoxed] = Bboxed_patt,
    [PUnboxed] = Bunboxed_patt,
    [PClosure] =  Bclosure_tag_patt
};

static const byte* pattern(const byte* code) {
    size_t* x = (size_t*)pop();
    byte l = LowBits(*code++);
    if (l < 0 || l > PClosure)
        failure("Invalid pattern for: '%d'", l);

    int result;
    // one corner case
    if (l == 0) result = Bstring_patt(x, (void*)pop());
    else result = patternValidators[l](x);

    push(result);
    return code;
}

static const byte* call(const byte* code) {
    size_t r;
    switch (LowBits(*code++)) {
        case LRead: r = Lread(); break;
        case LWrite: r = Lwrite((int)pop()); break;
        case LLenght: r = Llength((void*)pop()); break;
        case LString: r = (size_t)Lstring((void*)pop()); break;
        case BArray: {
            int n = GetInt;
            reverse(n);
            void* result = Barray(BOX(n), (void*)top());
            discard(n);
            r = (size_t)result;
            break;
        }
        default: failure("Unknown lower bits for call code '%d'", code[-1]);
    }
    push(r);
    return code;
}
