sources = main.c language.c bytereader.c stack.c
stdlib = runtime/runtime.a

analyserSources = analyser.c disassemle.c bytereader.c Set/Array.c Set/Set.c

CC = gcc
CFLAGS = -m32 -g

lamaI: $(sources) $(stdlib)
	$(LINK.c) $^ -o $@

analyser: $(analyserSources)
	$(LINK.c) $^ -o $@

$(stdlib): runtime/runtime.c runtime/gc_runtime.s
	$(MAKE) -C runtime

clean:
	$(RM) lamaI analyser
	$(MAKE) clean -C runtime
