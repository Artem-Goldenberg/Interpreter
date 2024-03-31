DEBUG_FILES=stack-dump-before data-dump-before extra-roots-dump-before heap-dump-before stack-dump-after data-dump-after extra-roots-dump-after heap-dump-after
TESTS=$(sort $(basename $(wildcard test*.lama)))

# LAMAC=../src/lamac
# testI = ../lamaI

.PHONY: check $(TESTS)

check: $(TESTS) | expressions deep-expressions
	@echo "Base tests OK"
	$(MAKE) check -C expressions
	@echo "Expression tests OK"
	$(MAKE) check -C deep-expressions
	@echo "Deep-Expressions test OK"


$(TESTS): %: %.lama
	@echo "regression/$@"
	@$(LAMAC) -b $<
	@cat $@.input | $(testI) $@.bc > $@.log && diff $@.log orig/$@.log	

clean:
	$(RM) test*.log *.s *.sm *.bc *~ $(TESTS) *.i $(DEBUG_FILES)
	$(MAKE) clean -C expressions
	$(MAKE) clean -C deep-expressions
