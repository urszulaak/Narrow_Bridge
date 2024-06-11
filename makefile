main:
	gcc main.c -o main -Wall -pthread -lm

1cari: main
	@./main 1 -info

1car: main
	@./main 1

5cari: main
	@./main 5 -info

5car: main
	@./main 5

50cari: main
	@./main 50 -info

50car: main
	@./main 5

.PHONY: clean

clean:
	@rm -r main