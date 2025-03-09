#
#	MAIN MAKEFILE FOR THIS PROJECT
#

SRC 	:= $(wildcard src/*.c)
WINDOWS_FLAGS	:= -O2 -std=c11 -Wall
LINUX_FLAGS 	:= -lm
DEBUG_FLAGS 	:= -ggdb
OUT				:= graph
CC 				:= gcc
# Default: Linux build
main:
	$(CC) $(SRC) $(LINUX_FLAGS) -o bin/$(OUT).out

# Windows build:
win:
	$(CC) $(SRC) $(WINDOWS_FLAGS) -o bin/$(OUT).exe

# Debug build (with debugger flags)
debug:
	$(CC) $(SRC) $(LINUX_FLAGS) $(DEBUG_FLAGS) -o bin/$(OUT).out

clean:
	rm -f bin/graph.out
	rm -f bin/graph.exe
