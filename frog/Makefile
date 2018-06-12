# Simple Makefile
 
# Compilers vars
CC=gcc
CPPFLAGS= -MMD -g #-fsanitize=address
CFLAGS= -O4 -Wall -Wextra -std=c99
LDFLAGS=
LDLIBS=-lreadline -lpthread -lm -lSDL2 -lSDL2_image
 
SRC= objects/*.c parser/*.c exec/*.c modules/*.c common/strutils.c common/hashmap.c error.c main.c
OBJ= ${SRC:.c=.o}
DEP= ${SRC:.c=.d}
 
all: frog

-include ${DEP}
 
frog:
	${CC} ${CPPFLAGS} ${CFLAGS} ${SRC} -o frog ${LDLIBS}
 
clean:
	rm -f ${DEP} ${OBJ} frog frog.d
 
# END Makefile
