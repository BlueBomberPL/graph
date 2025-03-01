#
#	MAIN MAKEFILE FOR THIS PROJECT
#

SRC 	:= $(wildcard *.c)
FLAGS	:= -O2 -std=c11 -Wall
OUT		:= graph

# Default: Linux build 
main:
	gcc $(SRC) $(FLAGS) -o bin/$(OUT).out"

# Windows build:
win:
	gcc $(SRC) $(FLAGS) -o bin/$(OUT).exe"
