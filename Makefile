export LAMAC = lamac

.PHONY: build
build:
	$(MAKE) -C src

.PHONY: analyse
analyse:
	$(MAKE) -C src analyser
	$(MAKE) -C perfomance check-analyser testI=$(abspath src/analyser)

.PHONY: test
test: build
	$(MAKE) -C regression testI=$(abspath src/lamaI)

.PHONY: perfomance
perfomance: build
	$(MAKE) -C perfomance testI=$(abspath src/lamaI)

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C regression
	$(MAKE) clean -C perfomance
