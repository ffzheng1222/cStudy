CC = gcc
CFLAGS = -Wall -g -O0

OBJ = linklist.c test.c

APP = test

$(APP):$(OBJ) 
	    $(CC) $(CFLAGS) -o $@ $^

clean:
	    $(RM) $(APP) core .*.sw? -rf
