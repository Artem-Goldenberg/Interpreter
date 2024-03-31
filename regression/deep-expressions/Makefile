TESTS=$(sort $(basename $(wildcard generated*.lama)))

.PHONY: check $(TESTS)

check: $(TESTS)

$(TESTS): %: %.lama
	@echo "regression/deep-expressions/$@"
	@$(LAMAC) -b $<
	@cat $@.input | $(testI) $@.bc > $@.log && diff $@.log orig/$@.log

clean:
	rm -f *.log *.s *.bc *.i *~
	find . -maxdepth 1 -type f -not -name '*.*' -not -name 'Makefile' -delete
