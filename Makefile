.default: all

all: scrabble test

clean:
	rm -rf scrabble test *.o *.dSYM

scrabble: Action.o Dictionary.o Game.o PlayerState.o BoardState.o State.o Tile.o Node.o LinkedList.o scrabble.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

test: Action.o Dictionary.o Game.o PlayerState.o BoardState.o State.o Tile.o Node.o LinkedList.o test.o
	g++ -Wall -Werror -std=c++14 -g -O -o $@ $^

%.o: %.cpp
	g++ -Wall -Werror -std=c++14 -g -O -c $^
