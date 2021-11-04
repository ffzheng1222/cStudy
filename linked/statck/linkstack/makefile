
CC = gcc
CFLAGS = -Wall -g -O0

OBG = linkstack.c test.c

APP = test

$(APP):$(OBG)
	    $(CC) $(CFLAGS) -o $@ $^

clean:
	    $(RM) $(APP) .*.sw? -rf core

