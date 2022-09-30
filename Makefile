# You can compile with either gcc or g++
# CC = g++
CC = gcc

# Compile/link options

# Use -lm (math library) if you wish, but leave it out if not in use.
#CFLAGS = -I. -Wall -lm $(HEAP_SIZE) -DNDEBUG
#Note: NDEBUG flag turns off asserts and other debugging keyed by this flag.

CFLAGS = -I. 

# Optimization
# Add this flag if you want to optimize for speed-testing.
OPTFLAG = -O2

#Debugging: to build for debugging, add this.  (-g3 might be better)
DEBUGFLAG = -g
CFLAGS += $(DEBUGFLAG)

EXECUTABLES = test_basicread test_basicwrite test_prioritywrite test_basicrw test_priorityrw

all: ${EXECUTABLES}

test: ${EXECUTABLES}
	for exec in ${EXECUTABLES}; do \
		./$$exec ; \
	done

debug: CFLAGS += $(DEBUGFLAG)
debug: ${EXECUTABLES}

test_basicread: test_basicread.c rwlock.o 
	$(CC) $(CFLAGS)  -o test_basicread test_basicread.c rwlock.o

test_basicwrite: test_basicwrite.c rwlock.o
	$(CC) $(CFLAGS)  -o test_basicwrite test_basicwrite.c rwlock.o

test_prioritywrite: test_prioritywrite.c rwlock.o
	$(CC) $(CFLAGS)  -o test_prioritywrite test_prioritywrite.c rwlock.o

test_basicrw: test_basicrw.c rwlock.o 
	$(CC) $(CFLAGS)  -o test_basicrw test_basicrw.c rwlock.o

test_priorityrw: test_priorityrw.c rwlock.o
	$(CC) $(CFLAGS)  -o test_priorityrw test_priorityrw.c rwlock.o

rwlock.o: rwlock.c rwlock.h
	$(CC) $(CFLAGS) -c rwlock.c

gradescope:
	zip submission.zip rwlock.c rwlock.h

clean:
	rm -rf *.o ${EXECUTABLES} *.dSYM a.out 
