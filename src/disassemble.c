#include "disassemble.h"
#include "bytereader.h"
#include <stdarg.h>

#define LowBits(x) ((x) & 0x0F)
#define HighBits(x) (((x) & 0xF0) >> 4)

#define GetInt (code = (const byte*)getInt((const char*)code), *((int*)code - 1))
#define GetString getString(GetInt)

static char* const operationString[] = {
    "+", "-", "*", "/", "%", "<", "<=", ">", ">=", "==", "!=", "&&", "!!"
};
static char* const patterString[] = {
    "=str", "#string", "#array", "#sexp", "#ref", "#val", "#fun"
};
static char* const loadOperationString[] = {"LD", "LDA", "ST"};
static const char locationSymbol[] = {
    [Global] = 'G',
    [Local] = 'L',
    [Argument] = 'A',
    [Closure] = 'C'
};

/// To avoid linking with runtime
void failure(char* message, ...) {
    va_list args;
    va_start(args, message);
    vfprintf(stderr, message, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

static void unknownCode(byte c) {
    failure("Unknown code: %d-%d", HighBits(c), LowBits(c));
}

/// Just print into file if not null, with format
static void dpf(FILE* out, const char* format, ...) { //va_list args) {
    if (!out) return;

    va_list args;
    va_start(args, format);

    vfprintf(out, format, args);

    va_end(args);
}

static const byte* printClosure(const byte* code, FILE* out);

#define CheckInBounds(i, array) \
if ((i) >= sizeof(array) / sizeof(array[0])) unknownCode(c)

const byte* disassemble(const byte* code, FILE* out) {
    if (!code) return NULL;

    // reached end of the file
    if (code == (const byte*)codeEnd()) return NULL;

    checkBounds((const char*)code);
    byte c = *code++;
    byte h = HighBits(c), l = LowBits(c);

    switch (h) {
        case 0: 
            CheckInBounds(l, operationString);
            dpf(out, "BINOP\t%s", operationString[l - 1]); // binop
            break;
        case 1: // other
            switch (l) {
                case Const: dpf(out, "CONST\t%d", GetInt); break;
                case String: dpf(out, "STRING\t%s", GetString); break;
                case Sexp: {
                    const char* str = GetString;
                    dpf(out, "SEXP\t%s %d", str, GetInt);
                } break;
                case Sti: dpf(out, "STI"); break;
                case Sta: dpf(out, "STA"); break;
                case Jmp: dpf(out, "JMP\t0x%.8x", GetInt); break;
                case End: dpf(out, "END"); break;
                case Ret: dpf(out, "RET"); break;
                case Drop: dpf(out, "DROP"); break;
                case Dup: dpf(out, "DUP"); break;
                case Swap: dpf(out, "SWAP"); break;
                case Elem: dpf(out, "ELEM"); break;
                default: unknownCode(c);
            } break;
        case 2: case 3: case 4: { // boxed
            if (l > Closure) unknownCode(c);
            int i = GetInt;
            CheckInBounds(h - 2, loadOperationString);
            CheckInBounds(l, locationSymbol);
            dpf(out, "%s\t%c(%d)", loadOperationString[h - 2], locationSymbol[l], i);
        } break;
        case 5: // control
            switch (l) {
                case CJmpZero: dpf(out, "CJMPz\t0x%.8x", GetInt); break;
                case CJmpNotZero: dpf(out, "CJMPnz\t0x%.8x", GetInt); break;
                case Begin: {
                    int first = GetInt;
                    dpf(out, "BEGIN\t%d %d", first, GetInt); 
                    break;
                }
                case CBegin: {
                    int first = GetInt;
                    dpf(out, "CBEGIN\t%d %d", first, GetInt); 
                    break;
                }
                case BClosure: code = printClosure(code, out); break;
                case CallC: dpf(out, "CALLC\t%d", GetInt); break;
                case Call: {
                    int addr = GetInt;
                    dpf(out, "CALL\t0x%.8x %d", addr, GetInt); 
                    break;
                }
                case Tag: {
                    const char* name = GetString;
                    dpf(out, "TAG\t%s %d", name, GetInt); 
                    break;
                }
                case Array: dpf(out, "ARRAY\t%d", GetInt); break;
                case Fail: {
                    int first = GetInt;
                    dpf(out, "FAIL\t%d %d", first, GetInt); break;
                }
                case Line: dpf(out, "LINE\t%d", GetInt); break;
                default: unknownCode(c);
            } break;
        case 6:
            CheckInBounds(l, patterString);
            dpf(out, "PATT\t%s", patterString[l]); // pattern
            break;
        case 7: // call
            switch (l) {
                case LRead: dpf(out, "CALL\tLread"); break;
                case LWrite: dpf(out, "CALL\tLwrite"); break;
                case LLenght: dpf(out, "CALL\tLlength"); break;
                case LString: dpf(out, "CALL\tLstring"); break;
                case BArray: dpf(out, "CALL\tBarray\t%d", GetInt); break;
                default: unknownCode(c);
            } break;
        case 15: dpf(out, "STOP"); break;
        default: unknownCode(c);
    }
    return code;
}

static const byte* printClosure(const byte* code, FILE* out) {
    int addr = GetInt;
    int n = GetInt;

    if (n < 0)
        failure("Invalid number of closure arguments: '%d'\n", n);

    if (!out) {
        for (int i = 0; i < n; ++i) {
            code++; GetInt;
        }
        return code;
    }

    fprintf(out, "CLOSURE\t0x%.8x", addr);
    for (int i = 0; i < n; ++i) {
        checkBounds((const char*)code);
        byte c = *code++;
        CheckInBounds(c, locationSymbol);
        fprintf(out, "%c(%d)", locationSymbol[c], GetInt);
    }

    return code;
}
