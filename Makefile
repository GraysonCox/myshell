CC = gcc
CXX = g++
ECHO = echo
RM = rm -f

TERM = "\"S2019\""

CFLAGS = -Wall -Werror -ggdb3 -funroll-loops -DTERM=$(TERM)
CXXFLAGS = -std=c++11 -Wall -Werror -ggdb3 -funroll-loops -DTERM=$(TERM)

LDFLAGS =

BIN = myshell
OBJS = io.o model.o decoder.o main.o

all: $(BIN) etags

$(BIN): $(OBJS)
	@$(ECHO) Linking $@
	@$(CXX) $^ -o $@ $(LDFLAGS)

-include $(OBJS:.o=.d)

%.o: %.c
	@$(ECHO) Compiling $<
	@$(CC) $(CFLAGS) -MMD -MF $*.d -c $<

%.o: %.cpp
	@$(ECHO) Compiling $<
	@$(CXX) $(CXXFLAGS) -MMD -MF $*.d -c $<

.PHONY: all clean clobber etags

clean:
	@$(ECHO) Removing all generated files
	@$(RM) *.o $(BIN) *.d TAGS core vgcore.* gmon.out

clobber: clean
	@$(ECHO) Removing backup files
	@$(RM) *~ \#* *pgm

etags:
	@$(ECHO) Updating TAGS
	@etags *.[ch] *.cpp