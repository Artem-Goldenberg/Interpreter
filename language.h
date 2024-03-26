#ifndef language_h
#define language_h

#include <stdint.h>

typedef uint8_t byte;
typedef uint32_t word;

typedef void (*Handler)(byte l);

void binop(byte l);
void value(byte l);
void boxed(byte l);
void control(byte l);
void pattern(byte l);
void call(byte l);

Handler handlers[] = {
    [0] = binop,
    [1] = value,
    [2] = boxed,
    [3] = boxed,
    [4] = boxed,
    [5] = control,
    [6] = pattern,
    [7] = call
};

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
} Whatever;

typedef enum {
    PStringCmp, PString,
    PArray,
    PSexp,
    PBoxed, PUnboxed,
    PColure
} Pattern;

//typedef enum {
//    Binop = 0x0,
//    Value = 0x1,
//    Movin =
//}

//typedef enum {
//    Binop = 0x00,
//    Const = 0x10,
//    String = 0x11,
//    Sexp = 0x12,
//    Sti = 0x13, Sta = 0x14,
//    Ld = , Lda,
//    St, Sti, Sta,
//    Elem,
//    Label, FLabel, SLabel,
//    Jmp, CJmp,
//    Begin, End,
//    Closure, Proto, Pproto,
//    Pcallc, Callc, Call, 
//    Ret,
//    Drop, Dup, Swap,
//    Tag, Array, Patt,
//    Fail,
//    Extern, Public,
//    Import,
//    Line
//} Instruction;

#endif /* language_h */
