OBJ = main.c condition.c mutex.c condition.h mutex.h

main: $(OBJ)
	gcc $(OBJ) -o main -Wall -pthread -lm

5car: main
	@./main 50 -info
