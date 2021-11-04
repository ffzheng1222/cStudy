
CC = gcc
CFLAGS = -Wall -g -O0

OBJ = kernellist.c test.c

APP = test

$(APP):$(OBJ) 
	    $(CC) $(CFLAGS) -o $@ $^

clean:
	    $(RM) $(APP) core .*.sw? -rf 
