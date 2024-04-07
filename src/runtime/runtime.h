#ifndef __LAMA_RUNTIME__
#define __LAMA_RUNTIME__

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <regex.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#define WORD_SIZE (CHAR_BIT * sizeof(int))

void failure (char *s, ...);

extern void* Bstring(void*);
extern int LtagHash(char*);
extern void* Bsexp(int bn, int tag, int* content);
extern void* Bsta(void *v, int i, void *x);
extern void* Belem(void *p, int i);
extern int* BelemClosure(void* closure, int i);
extern void* Bclosure(int bn, void *entry, int* binds);
extern int Btag(void *d, int t, int n);
extern int Barray_patt(void *d, int n);
extern int Bstring_patt(void *x, void *y);
extern int Bclosure_tag_patt(void *x);
extern int Bstring_tag_patt(void *x);
extern int Barray_tag_patt(void *x);
extern int Bsexp_tag_patt(void *x);
extern int Bboxed_patt(void *x);
extern int Bunboxed_patt(void *x);
extern int Bclosure_tag_patt(void *x);

extern int Lread(void);
extern int Lwrite(int n);
extern int Llength(void *p);
extern void* Lstring(void *p);
extern void* Barray(int bn, void* p);

# define UNBOXED(x)  (((int) (x)) &  0x0001)
# define UNBOX(x)    (((int) (x)) >> 1)
# define BOX(x)      ((((int) (x)) << 1) | 0x0001)

#endif
