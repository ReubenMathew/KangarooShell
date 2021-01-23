EXECUTABLE_NAME=KShell

all: build

build: ksh.c
		gcc -g -o $(EXECUTABLE_NAME) ksh.c -W -Wall
		./$(EXECUTABLE_NAME)

clean: 
		rm $(EXECUTABLE_NAME)



