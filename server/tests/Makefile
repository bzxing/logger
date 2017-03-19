
CC=g++
CPPFLAGS=-c -Wall -Wextra -O3 -flto -std=c++14
DEPFLAGS=-M
LDFLAGS=-lboost_system -lpthread

BUILDDIR=../build/tests
DEPDIR=$(BUILDDIR)/dep
OBJDIR=$(BUILDDIR)/obj
EXEDIR=$(BUILDDIR)

EXEC=server_test
LIB=server.a
SOURCES=$(wildcard *.cc)
DEPS=$(SOURCES:.cc=.d)
OBJS=$(SOURCES:.cc=.o)
DEPSFP=$(patsubst %, $(DEPDIR)/%, $(DEPS))
OBJSFP=$(patsubst %, $(OBJDIR)/%, $(OBJS))

MAIN_OBJ=$(OBJDIR)/main.o
OBJSFP_NOMAIN=$(filter-out $(MAIN_OBJ), $(OBJSFP))

BIN_SRC_DIR=../src
BIN_BUILD_DIR=../build/bin
BIN_LIB=$(BIN_BUILD_DIR)/server.a

$(shell mkdir -p $(DEPDIR) > /dev/null)
$(shell mkdir -p $(OBJDIR) > /dev/null)
$(shell mkdir -p $(EXEDIR) > /dev/null)


$(EXEDIR)/$(EXEC): $(OBJSFP) $(BIN_LIB)
	$(CC) $^ -o $(EXEDIR)/$(EXEC) $(LDFLAGS)

$(BIN_LIB):
	$(MAKE) -C $(BIN_SRC_DIR) lib

$(DEPDIR)/%.d: %.cc
	@set -e; rm -f $@; \
	$(CC) $(DEPFLAGS) $(CPPFLAGS) $< > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*, $(OBJDIR)/$(@F:.d=.o) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
.PRECIOUS: $(DEPDIR)/%.d

-include $(DEPSFP)
$(OBJDIR)/%.o: %.cc $(DEPDIR)/%.d
	$(CC) $(CPPFLAGS) $< -o $@
.PRECIOUS: $(OBJDIR)/%.o


.PHONY: clean
clean:
	rm -rf ./$(DEPDIR)/*.d \
	rm -rf ./$(OBJDIR)/*.o \
	rm -rf ./$(EXEDIR)/$(EXEC) \
	rm -rf $(BIN_LIB)

