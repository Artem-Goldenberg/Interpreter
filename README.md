#  Lama Interpreter

To just build the interpreter run:
```
make build
```

To run tests use:
```
make test [LAMAC=<path to lama compiler>]
```
*They take a long time! You can probably view the result in CI here*

`LAMAC` is optional.  
Default value for `LAMAC` is... `lamac`   
So if `lamac` is available in PATH, don't specify it

To measure timings run:
```
make perfomance [LAMAC=...]
```

Timings on my machine:
```
lamaI     (custom) : Sort	3.49s
lamac -i (standard): Sort   ♾️s (lamac -i hangs completely on my machine, I don't know why)
```

## Bytecode counter

To run frequency analysis:
```
make analyse
```

Output on my machine:
```
Count	Bytecode
------------------
31	DROP
28	DUP
21	ELEM
16	CONST	1
12	CONST	0
 8	LD	A(0)
 6	END
 5	JMP	0x00000308
 4	SEXP	cons 2
 3	LD	L(3)
 3	ST	L(0)
 3	LD	L(0)
 3	JMP	0x0000015e
 3	CALL	0x0000015f 1
 3	ARRAY	2
 3	CALL	Barray	2
 2	JMP	0x00000074
 2	LD	L(1)
 2	CALL	0x00000097 1
 2	BINOP	==
 2	BINOP	-
 2	TAG	cons 2
 2	BEGIN	1 0
 2	BEGIN	2 0
 2	CALL	0x0000002b 1
 1	CJMPnz	0x000000c5
 1	CJMPnz	0x00000118
 1	ST	L(5)
 1	BINOP	>
 1	ST	L(4)
 1	CJMPz	0x00000266
 1	ST	L(3)
 1	ST	L(2)
 1	ST	L(1)
 1	FAIL	18 9
 1	JMP	0x00000182
 1	CONST	1000
 1	LD	L(2)
 1	JMP	0x00000150
 1	LD	A(1)
 1	LD	L(4)
 1	CJMPnz	0x000001ac
 1	CJMPz	0x000000bf
 1	LD	L(5)
 1	BEGIN	1 6
 1	BEGIN	1 1
 1	LINE	10
 1	JMP	0x00000106
 1	FAIL	11 17
 1	CALL	0x00000075 1
 1	JMP	0x000002ec
 1	CJMPnz	0x0000028b
 1	JMP	0x000002d9
 1	LINE	9
 1	LINE	7
 1	LINE	2
 1	LINE	1
 1	CJMPnz	0x00000188
 1	CJMPz	0x00000112
 1	CALL	0x00000309 2
 1	LINE	31
 1	LINE	29
 1	LINE	28
 1	LINE	24
 1	LINE	22
 1	LINE	20
 1	CJMPz	0x0000006a
 1	LINE	19
 1	LINE	18
 1	LINE	13
 1	LINE	11
```

To clean everything:
```
make clean
```
