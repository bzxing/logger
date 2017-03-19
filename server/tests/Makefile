#all:
#	g++ *.cc ../src/reqs.cc ../src/msgs.cc -Wall -Wextra -O3 -flto -std=c++14 -lboost_system -lpthread -o server_tests

CC=g++
CPPFLAGS=-c -Wall -Wextra -O3 -flto -std=c++14
DEPFLAGS=-M
LDFLAGS=-lboost_system -lpthread

BUILDDIR=../build/tests
DEPDIR=$(BUILDDIR)/dep
OBJDIR=$(BUILDDIR)/obj
EXEDIR=$(BUILDDIR)

EXEC=server_test
SOURCES=$(wildcard *.cc)
DEPS=$(SOURCES:.cc=.d)
OBJS=$(SOURCES:.cc=.o)
OBJFULL=$(patsubst %, $(OBJDIR)/%, $(OBJS))

# Link to all BIN objects, minus main
BINSRCDIR=../src
BINOBJDIR=../build/bin/obj
BINSRCFILTER=main.cc
BINSRC=$(filter-out $(BINSRCFILTER), $(notdir $(wildcard $(BINSRCDIR)/*.cc)))
BINOBJS=$(BINSRC:.cc=.o)

BINOBJSFULL=$(patsubst %, $(BINOBJDIR)/%, $(BINOBJS))


$(shell mkdir -p $(DEPDIR) > /dev/null)
$(shell mkdir -p $(OBJDIR) > /dev/null)
$(shell mkdir -p $(EXEDIR) > /dev/null)

# Will build actual server binaries if not exist or outdated

.PHONY: bin

all: $(OBJFULL) bin
	$(CC) $(OBJFULL) $(BINOBJSFULL) -o $(EXEDIR)/$(EXEC) $(LDFLAGS)

bin:
	$(MAKE) -C $(BINSRCDIR)

$(DEPDIR)/%.d: %.cc
	@set -e; rm -f $@; \
	$(CC) $(DEPFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*, $(OBJDIR)/$(@F:.d=.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
.PRECIOUS: $(DEPDIR)/%.d

-include $(patsubst %, $(DEPDIR)/%, $(DEPS))
$(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d
	$(CC) $(CPPFLAGS) $< -o $@
.PRECIOUS: $(OBJDIR)/%.o

clean:
	rm -rf ./$(DEPDIR)/*.d \
	rm -rf ./$(OBJDIR)/*.o \
	rm -rf ./$(EXEDIR)/$(EXEC)
