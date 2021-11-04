CC = gcc 
CFLAGS = -Wall -g -O0

OBJ = linkqueue.c test.c

OBJS = test

$(OBJS):$(OBJ)
	    $(CC) $(CFLAGS) -o $@ $^

clean:
	    $(RM) $(OBJS) .*.sw? -rf core
