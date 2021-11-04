
CC = gcc
CFLAGS = -Wall -g -O0

OBJ = doublelist.c test.c

APP = test

$(APP):$(OBJ) 
	    $(CC) $(CFLAGS) -o $@ $^

clean:
	    $(RM) $(APP) core .*.sw? -rf 
