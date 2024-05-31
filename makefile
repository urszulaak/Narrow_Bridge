OBJ = main.c condition.c mutex.c condition.h mutex.h

main: $(OBJ)
	gcc $(OBJ) -o main -Wall -pthread

5car: main
	@./main 5 -info