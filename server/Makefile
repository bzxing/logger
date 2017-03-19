.PHONY: target_bin
.PHONY: target_tests

SRCDIR=src
TSTDIR=tests



all: target_tests target_bin



target_bin:
	$(MAKE) -C $(SRCDIR)

target_tests: target_bin
	$(MAKE) -C $(TSTDIR)

clean:
	$(MAKE) -C $(SRCDIR) clean
	$(MAKE) -C $(TSTDIR) clean
