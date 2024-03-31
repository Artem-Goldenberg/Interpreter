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

To clean everything:
```
make clean
```
