CC = gcc
CFLAGS = -I.
PROGRAM = lss 
N = 

all: lss.c
	$(CC) -o $(PROGRAM) lss.c $(CFLAGS)

run: $(PROGRAM)
	$(PROGRAM) $(N)

sample:
	/bin/ls -Lla | grep -v '^total' | sort -k +5nr

sample1:
	/bin/ls -la | grep -v '^total' | sort -k +5nr	
clean:
	rm $(PROGRAM)

