C_SRC = $(wildcard *.c)
C_OBJ = $(patsubst %c, %o, $(C_SRC))
TARGETLIST = $(patsubst %.c, %, $(C_SRC))

.PHONY:all clean

all:${TARGETLIST}

clean:  
	rm -f ${TARGETLIST} *.o 
