CC=g++

GUI_FLAGS=-lSDL2 -lSDL2_ttf 
TUI_FLAGS=-lncurses

TECO_ENGINE_INPUT=./teco_engine.cpp
TECO_GUI_INPUT=./teco_gui.cpp
TECO_TUI_INPUT=./teco_tui.hpp

TECO_ENGINE_OUTPUT=./teco_engine.o
TECO_GUI_OUTPUT=./teco_gui.o
TECO_TUI_OUTPUT=./teco_tui.o

.PHONY: engine gui tui

engine:
	$(CC) -c -o $(TECO_ENGINE_OUTPUT) $(TECO_ENGINE_INPUT)

gui:
	$(CC) -c -o $(TECO_GUI_OUTPUT) $(TECO_GUI_INPUT) $(GUI_FLAGS)

tui:
	$(CC) -c -o $(TECO_TUI_OUTPUT) $(TECO_TUI_INPUT) $(TUI_FLAGS)

