#ifndef language_h
#define language_h

#include <stdint.h>

typedef uint8_t byte;
typedef uint32_t word;

typedef void (*Handler)(void);

void interpret(void);

void binop(void);
void other(void);
void boxed(void);
void control(void);
void pattern(void);
void call(void);

extern const Handler handlers[];

typedef enum {
    Plus = 1, Minus, Multiply, Divide, Mod,
    Less, LessEqual, More, MoreEqual,
    Equal, NotEqual,
    And, Or
} BinaryOperation;

typedef enum {
    Const, String, Sexp,
    Sti, Sta,
    Jmp, End, Ret,
    Drop, Dup,
    Swap, Elem
} Valueish;

typedef enum {
    CJmpZero, CJmpNotZero,
    Begin, CBegin, BClosure,
    CallC, Call, Tag,
    Array,
    Fail,
    Line
} ControlFlow;

typedef enum {
    PStringCmp, PString,
    PArray,
    PSexp,
    PBoxed, PUnboxed,
    PClosure
} Pattern;

typedef enum {
    Global, Local, Argument, Closure
} Location;

typedef enum {
    LD = 2, LDA, ST
} LoadStoreOperation;

typedef enum {
    LRead, LWrite, LLenght, LString, BArray
} StandardFunction;

#endif /* language_h */
