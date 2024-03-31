sources = main.c language.c bytereader.c stack.c
stdlib = runtime/runtime.a

CC = gcc
CFLAGS = -m32 -g

lamaI: $(sources) $(stdlib)
	$(LINK.c) $^ -o $@

$(stdlib): runtime/runtime.c runtime/gc_runtime.s
	$(MAKE) -C runtime

clean:
	$(RM) lamaI
	$(MAKE) clean -C runtime
