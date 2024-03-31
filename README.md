#  Lama Interpreter

To just build the interpreter run:
```
make build LAMAC=<path to lama compiler>
```
`LAMAC` is optional.  
Default value for `LAMAC` is... `lamac`   
So if `lamac` is available in PATH, don't specify it

To run tests use:
```
make test [LAMAC=...]
```
*They take a long time!*


To measure timings run:
```
make perfomance [LAMAC=...]
```

Timings on my machine:
```
$ make perfomance        
make -C perfomance testI=/home/Interpreter/lamaI
make[1]: Entering directory '/home/Interpreter/perfomance'
Measurements for Sort:
lamaI     (custom) : Sort	3.49s
lamac -i (standard): Sort   ♾️s (lamac -i hangs completely on my machine, I don't know why)
make[1]: Leaving directory '/home/Interpreter/perfomance'
```
