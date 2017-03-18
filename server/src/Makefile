CC=g++
CPPFLAGS=-c -Wall -Wextra -O3 -flto -std=c++14
DEPFLAGS=-M
LDFLAGS=-lboost_system -lpthread

BUILDDIR=../build
DEPDIR=$(BUILDDIR)/dep
OBJDIR=$(BUILDDIR)/obj
EXEDIR=$(BUILDDIR)

EXEC=server
SOURCES=$(wildcard *.cc)
DEPS=$(SOURCES:.cc=.d)
OBJS=$(SOURCES:.cc=.o)

$(shell mkdir -p $(DEPDIR) > /dev/null)
$(shell mkdir -p $(OBJDIR) > /dev/null)
$(shell mkdir -p $(EXEDIR) > /dev/null)

all: $(patsubst %, $(OBJDIR)/%, $(OBJS))
	$(CC) $^ -o $(EXEDIR)/$(EXEC) $(LDFLAGS)

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
