export LAMAC = lamac

.PHONY: build
build:
	$(MAKE) -C src

.PHONY: test
test: build
	$(MAKE) -C regression testI=$(abspath src/lamaI)

.PHONY: perfomance
perfomance: lamaI
	$(MAKE) -C perfomance testI=$(abspath src/lamaI)

clean:
	$(MAKE) clean -C src
	$(MAKE) clean -C regression
	$(MAKE) clean -C perfomance
