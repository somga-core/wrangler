CC=g++
FLAGS=-lncurses -lSDL2 -lSDL2_ttf
INPUT=./main.cpp
OUTPUT=./exc

RMCOMMAND=rm -f

.PHONY: clean build

all: clean build

build:
	$(CC) -o $(OUTPUT) $(INPUT) $(FLAGS)

run: all
	$(OUTPUT)

clean:
	$(RMCOMMAND) $(OUTPUT)
