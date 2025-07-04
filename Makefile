FLAGS = @g++ -Wall -Ofast

.PHONY: all alll main perfttest bestmovetest gui clean cleano

all: main perfttest bestmovetest gui

alll:
	$(FLAGS) -o main main.cpp game.cpp board.cpp movegen.cpp ai.cpp `pkg-config --cflags --libs gtk4`
	$(FLAGS) -o perfttest perfttest.cpp game.cpp board.cpp movegen.cpp ai.cpp `pkg-config --cflags --libs gtk4`
	$(FLAGS) -o bestmovetest bestmovetest.cpp game.cpp board.cpp movegen.cpp ai.cpp `pkg-config --cflags --libs gtk4`
	$(FLAGS) -o gui gui.cpp game.cpp board.cpp movegen.cpp ai.cpp `pkg-config --cflags --libs gtk4`

main: main.o game.o board.o movegen.o ai.o
	$(FLAGS) -o main main.o game.o board.o movegen.o ai.o `pkg-config --cflags --libs gtk4`
perfttest: perfttest.o board.o movegen.o 
	$(FLAGS) -o perfttest perfttest.o board.o movegen.o 
bestmovetest: bestmovetest.o board.o movegen.o ai.o
	$(FLAGS) -o bestmovetest bestmovetest.o board.o movegen.o ai.o `pkg-config --cflags --libs gtk4`
gui: gui.o board.o movegen.o game.o ai.o
	$(FLAGS) -o gui gui.o board.o movegen.o game.o ai.o `pkg-config --cflags --libs gtk4`
clean: cleano
	rm main perfttest bestmovetest gui
cleano:
	rm *.o

main.o: main.cpp
	$(FLAGS) -c main.cpp `pkg-config --cflags --libs gtk4`
game.o: game.cpp
	$(FLAGS) -c game.cpp `pkg-config --cflags --libs gtk4`
board.o: board.cpp
	$(FLAGS) -c board.cpp
movegen.o: movegen.cpp
	$(FLAGS) -c movegen.cpp
perfttest.o: perfttest.cpp
	$(FLAGS) -c perfttest.cpp
bestmovetest.o: bestmovetest.cpp
	$(FLAGS) -c bestmovetest.cpp `pkg-config --cflags --libs gtk4`
ai.o: ai.cpp
	$(FLAGS) -c ai.cpp `pkg-config --cflags --libs gtk4`
gui.o: gui.cpp
	$(FLAGS) -c gui.cpp `pkg-config --cflags --libs gtk4`