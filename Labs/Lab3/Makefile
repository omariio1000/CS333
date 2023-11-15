# Omar Nassar
# October 11, 2023
# Portland State University CS201
# Makefile for thread_crypt program

CC = gcc
DEBUG = -g
DEFINES = 

CFLAGS = ${DEBUG} ${DEFINES} -Wall -Wshadow -Wunreachable-code -Wredundant-decls \
		-Wmissing-declarations -Wold-style-definition -Wmissing-prototypes \
		-Wdeclaration-after-statement -Wextra -Werror -Wpedantic -Wno-return-local-addr \
		-Wunsafe-loop-optimizations -Wuninitialized

PROG1 = thread_crypt
INCLUDES = thread_crypt.h

PROGS = ${PROG1} 

# $@ is the name of the target
# $^ is the names of all the prerequisites
# $< is the name of the first prerequisite

all: ${PROGS}

${PROG1}: ${PROG1}.o
	${CC} ${CFLAGS} -o $@ $^

${PROG1}.o: ${PROG1}.c $(INCLUDES)
	${CC} ${CFLAGS} -c $<

clean cls:
	-rm -f ${PROGS} *.o *~ \#*

tar:
	tar cvfz $(shell basename $(CURDIR)).tar.gz *.[ch] [Mm]akefile
	@echo "\nVERIFICATION:\n"
	tar tvf $(shell basename $(CURDIR)).tar.gz
