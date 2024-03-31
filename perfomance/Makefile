TESTS=$(sort $(basename $(wildcard *.lama)))

.PHONY: check $(TESTS)

check: $(TESTS)

#time = $(shell which time)

$(TESTS): %: %.lama
	@echo "Measurements for $@:"
	@$(LAMAC) -b $<
	@command time -f "lamaI     (custom) : $@\t%Us" $(testI) $@.bc
	@command time -f "lamac -i (standard): $@\t%U" $(LAMAC) -i $<

clean:
	$(RM) test*.log *.s *~ $(TESTS) *.i *.bc
