BIN = main
SRC = \
	network.c	\
	main.c

CC	     = cc
CFLAGS   = -g -Wall

CXX	     = g++
CXXFLAGS = -g

GDB    = gdb
RM     = rm -f

ALL_FILES = $(SRC:%.c=%.o)

all: $(BIN)

$(BIN): $(ALL_FILES)
	$(CXX) $(ALL_FILES) -o $@ 


# pull in dependencies for .o files
-include $(ALL_FILES:.o=.d)

%.o: %.c
	$(CC) $(CFLAGS) -c $<
	$(CC) -MM $(CFLAGS) $*.c > $*.d

debug: $(BIN)
	$(GDB) ./$(BIN)

run: $(BIN)
	./$(BIN)

clean:
	$(RM) $(BIN) $(ALL_FILES) *.d

.PHONY: clean debug run 

