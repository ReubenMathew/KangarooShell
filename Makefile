EXECUTABLE_NAME=KShell

all: run 

run: ksh.c
		gcc -g -o $(EXECUTABLE_NAME) ksh.c -W -Wall
		./$(EXECUTABLE_NAME)

build: ksh.c
		gcc -g -o $(EXECUTABLE_NAME) ksh.c -W -Wall

clean: 
		rm $(EXECUTABLE_NAME)

.PHONY: valgrind
valgrind: build
	valgrind --leak-check=full --show-leak-kinds=all ./$(EXECUTABLE_NAME)

