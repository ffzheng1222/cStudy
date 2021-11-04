##############################
# servfox Makefile
##############################

#INSTALLROOT=$(PWD)
#CROSS_COMPILE = arm-linux-gnu-


CC = $(CROSS_COMPILE)gcc
STRIP=$(CROSS_COMPILE)strip
ifdef CROSS_COMPILE
TARGET_FS_PATH = /opt/filesystem
endif

BINARY = sem_c_w

BIN_PATH =$(TARGET_FS_PATH)/usr/bin

INSTALL=install

#WARNINGS = -Wall \
#           -Wundef -Wpointer-arith -Wbad-function-cast \
#           -Wcast-align -Wwrite-strings -Wstrict-prototypes \
#           -Wmissing-prototypes -Wmissing-declarations \
#           -Wnested-externs -Winline -Wcast-qual -W \
#           -Wno-unused
#           -Wunused

ifdef DEBUG
 	CFLAGS = -g -O0 -DDEBUG
else
	CFLAGS = -O2
endif

CFLAGS += -DLINUX $(WARNINGS)

#LIBS= -lpthread


OBJ= ${patsubst %.c, %.o, ${wildcard *.c}}
		
all:$(BINARY)
	
clean:
	@echo "Cleaning up directory."
	rm -f *.a *.o $(BINARY)   core *~ log errlog

$(BINARY):$(OBJ)
	$(CC) $(CFLAGS) -o $(BINARY) $(OBJ) $(LIBS)
	
%.o : %.c	
	$(CC) $(CFLAGS) -c -o $@ $<
	 
install: $(BINARY)
	$(INSTALL) -s -m 755 -g root -o root $(BINARY) $(BIN_PATH) 

dist: clean
	tar czf ../farsight_daemon_demo_for_16071.tar.gz ../daemon
	
