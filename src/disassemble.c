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

/// Dynamic print with format
static char* dpf(const char* format, ...) { //va_list args) {
    va_list args, args2;
    va_start(args, format);
    va_copy(args2, args);
    int size = vsnprintf(NULL, 0, format, args);
    char* result = malloc(size + 1);
    if (!result) failure("Cannot allocate space for string\n");
    vsprintf(result, format, args2);
    va_end(args);
    va_end(args2);
    return result;
}

static const byte* code;

//typedef struct {
//    const byte* next;
//    char* str;
//} pair;
//
//pair dpf(const char* string, ...) {
//    va_list args;
//    va_start(args, string);
//    pair result = {code, dynamicPrintf(string, args)};
//    va_end(args);
//    return result;
//}

static char* printClosure(void);

void setCode(const byte* newCode) {
    code = newCode;
}

const byte* getCode(void) {
    return code;
}

#define CheckInBounds(i, array) \
if ((i) >= sizeof(array) / sizeof(array[0])) unknownCode(c)

char* disassemble(void) {
    if (!code) return NULL;

    // reached end of the file
    if (code == (const byte*)codeEnd()) return NULL;

    checkBounds((const char*)code);
    byte c = *code++;
    byte h = HighBits(c), l = LowBits(c);

    switch (h) {
        case 0: 
            CheckInBounds(l, operationString);
            return dpf("BINOP\t%s", operationString[l - 1]); // binop
        case 1: // other
            switch (l) {
                case Const: return dpf("CONST\t%d", GetInt);
                case String: return dpf("STRING\t%s", GetString);
                case Sexp: {
                    const char* str = GetString;
                    return dpf("SEXP\t%s %d", str, GetInt);
                }
                case Sti: return dpf("STI");
                case Sta: return dpf("STA");
                case Jmp: return dpf("JMP\t0x%.8x", GetInt);
                case End: return dpf("END");
                case Ret: return dpf("RET");
                case Drop: return dpf("DROP");
                case Dup: return dpf("DUP");
                case Swap: return dpf("SWAP");
                case Elem: return dpf("ELEM");
                default: unknownCode(c);
            }
        case 2: case 3: case 4: { // boxed
            if (l > Closure) unknownCode(c);
            int i = GetInt;
            CheckInBounds(h - 2, loadOperationString);
            CheckInBounds(l, locationSymbol);
            return dpf("%s\t%c(%d)", loadOperationString[h - 2], locationSymbol[l], i);
        }
        case 5: // control
            switch (l) {
                case CJmpZero: return dpf("CJMPz\t0x%.8x", GetInt);
                case CJmpNotZero: return dpf("CJMPnz\t0x%.8x", GetInt);
                case Begin: {
                    int first = GetInt;
                    return dpf("BEGIN\t%d %d", first, GetInt);
                }
                case CBegin: {
                    int first = GetInt;
                    return dpf("CBEGIN\t%d %d", first, GetInt);
                }
                case BClosure: return printClosure();
                case CallC: return dpf("CALLC\t%d", GetInt);
                case Call: {
                    int addr = GetInt;
                    return dpf("CALL\t0x%.8x %d", addr, GetInt);
                }
                case Tag: {
                    const char* name = GetString;
                    return dpf("TAG\t%s %d", name, GetInt);
                }
                case Array: return dpf("ARRAY\t%d", GetInt);
                case Fail: {
                    int first = GetInt;
                    return dpf("FAIL\t%d %d", first, GetInt);
                }
                case Line: return dpf("LINE\t%d", GetInt);
                default: unknownCode(c);
            }
        case 6: 
            CheckInBounds(l, patterString);
            return dpf("PATT\t%s", patterString[l]); // pattern
        case 7: // call
            switch (l) {
                case LRead: return dpf("CALL\tLread");
                case LWrite: return dpf("CALL\tLwrite");
                case LLenght: return dpf("CALL\tLlength");
                case LString: return dpf("CALL\tLstring");
                case BArray: return dpf("CALL\tBarray\t%d", GetInt);
                default: unknownCode(c);
            }
        case 15: return dpf("STOP");
        default: unknownCode(c);
    }
    assert(0);
}

static int numDigits(int x) {
    int result = 1;
    while (x /= 10) result++;
    return result;
}

static char* printClosure(void) {
    int addr = GetInt;
    int n = GetInt;
    const byte* rewind = code;

    int digitSize = 0;
    for (int i = 0; i < n; ++i) {
        code++;
        digitSize += numDigits(GetInt);
    }

    int strSize = snprintf(NULL, 0, "CLOSURE\t0x%.8x", addr);
    int totalSize = strSize + digitSize + n * 3 + 1;
    char* result = malloc(totalSize); // for S() and \0
    if (!result) failure("Not enough memory to allocate, need %d bytes", totalSize);

    int j = sprintf(result, "CLOSURE\t0x%.8x", addr);
    code = rewind;
    for (int i = 0; i < n; ++i) {
        checkBounds((const char*)code);
        byte c = *code++;

        CheckInBounds(c, locationSymbol);

        j = sprintf(result + j, "%c(%d)", locationSymbol[c], GetInt);
    }

    assert(j == strSize + digitSize + n * 3 + 1);
    return result;
}
