
CC = gcc
CFLAGS = -Wall -g -O0

OBG = btree.c  linkqueue.c seqstack.c test.c

APP = test

$(APP):$(OBG)
	    $(CC) $(CFLAGS) -o $@ $^

clean:
	    $(RM) $(APP) .*.sw? -rf core

