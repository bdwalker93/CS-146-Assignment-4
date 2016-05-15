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

sample2:
	/bin/ls -Lla tstDir1 | grep -v '^total' | sort -k +5nr

sample3:
	/bin/ls -Lla file1 | grep -v '^total' | sort -k +5nr

sample4:
	/bin/ls -Lla tstDir1 file1 | grep -v '^total' | sort -k +5nr
clean:
	rm $(PROGRAM)

