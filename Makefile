SHELL=/bin/sh

# Program for compiling C programs.
CC=gcc

# What allocation strategy to use, and number of quick fit lists.
STRATEGY=5
NRQUICKLISTS=6

# Extra flags to give to the C preprocessor and programs that use it (the C and Fortran compilers).
CFLAGS=-DSTRATEGY=$(STRATEGY) -DNRQUICKLISTS=$(NRQUICKLISTS)

# Default plus extra flags for C preprocessor and compiler.
all_cflags=$(CFLAGS) -Wall -Wextra -ansi -O4 -pedantic -lpthread

# Malloc source file to use. Set to empty (with `make MALLOC=`) for system default.
MALLOC=openmalloc.c


##################################################
## Setup files variables
## =====================

# Source files to compile and link together
srcs=$(MALLOC) tstalgorithms.c tstcrash.c tstcrash_complex.c tstcrash_simple.c \
	 tstextreme.c tstmalloc.c tstmemory.c tstmerge.c tstrealloc.c \
	 tstbestcase.c tstworstcase.c

# Executables
execs=$(patsubst tst%.c, tst%, $(filter tst%.c, $(srcs)))


##################################################
## Ordinary targets
## ================

# http://www.gnu.org/software/make/manual/make.html#Phony-Targets
# These are not the name of files that will be created by their recipes.
.PHONY: all clean

all: $(execs)

tst%: tst%.o $(MALLOC:.c=.o)
	$(CC) $(all_cflags) -o $@ $^

# These programs can not be compiled as ANSI-standard C.
tst%.o: tst%.c
	$(CC) -c $(CFLAGS) $< -o $@

# But the rest should be ANSI-standard C.
%.o: %.c
	$(CC) -c $(all_cflags) $< -o $@

clean:
	-rm -f *.o core $(execs)
