build:
	gcc -g -Wextra -pedantic -O2 -o main main.c logic.c -lSDL3 -fopenmp -lm
