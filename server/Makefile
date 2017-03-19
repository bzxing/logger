.PHONY: target_bin
.PHONY: target_tests

SRCDIR=src
TSTDIR=tests


.PHONY: all
all: bin tests

.PHONY: bin
bin: bin_obj
	$(MAKE) -C $(SRCDIR)

.PHONY: tests
tests: bin_obj
	$(MAKE) -C $(TSTDIR)

.PHONY: bin_obj
bin_obj:
	$(MAKE) -C $(SRCDIR) obj

.PHONY: clean
clean:
	$(MAKE) -C $(SRCDIR) clean
	$(MAKE) -C $(TSTDIR) clean
