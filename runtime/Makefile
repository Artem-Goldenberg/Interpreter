CC = gcc
CFLAGS = -m32 -g2 -fstack-protector-all

# this target is the most important one, its' artefacts should be used as a runtime of Lama
all: runtime.o gc_runtime.o
	ar rc runtime.a $^

gc_runtime.o: gc_runtime.s
	$(CC) $(CFLAGS) -c $< -o $@

runtime.o: runtime.c runtime.h
	$(CC) $(CFLAGS) -c runtime.c

clean:
	$(RM) *.a *.o *~
