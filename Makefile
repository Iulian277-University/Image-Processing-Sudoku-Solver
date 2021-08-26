all: build run clean

build:
	@gcc -Wall src/sudoku.c -o sudoku -lm src/cJson.c

run:
	@./sudoku

clean:
	@rm sudoku
