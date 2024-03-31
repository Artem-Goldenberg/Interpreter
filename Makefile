sources = main.c language.c bytereader.c stack.c
stdlib = runtime/runtime.a

CC = gcc
CFLAGS = -m32 -g
export LAMAC = lamac

test: lamaI | regression
	$(MAKE) -C regression check testI=$(abspath ./lamaI)

perfomance: lamaI | regression
	$(MAKE) -C regression

lamaI: $(sources) $(stdlib)
	$(LINK.c) $^ -o $@

$(stdlib): runtime/gc_runtime.s runtime/runtime.c | runtime
	$(MAKE) -C runtime

clean:
	$(RM) $(stdlib) lamaI
	$(MAKE) clean -C runtime
	$(MAKE) clean -C regression
