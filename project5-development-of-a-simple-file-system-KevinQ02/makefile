CC = gcc
CFLAGS = -Wall -Werror -g

all: demo

demo: fs_core.o fs_file_ops.o disk.o demo.o
	$(CC) $(CFLAGS) -o demo fs_core.o fs_file_ops.o disk.o demo.o

fs_core.o: fs_core.c fs_core.h disk.h
	$(CC) $(CFLAGS) -c fs_core.c

fs_file_ops.o: fs_file_ops.c fs_file_ops.h fs_core.h disk.h
	$(CC) $(CFLAGS) -c fs_file_ops.c

demo.o: demo.c fs_core.h fs_file_ops.h disk.h
	$(CC) $(CFLAGS) -c demo.c

disk.o: disk.c disk.h
	$(CC) $(CFLAGS) -c disk.c

clean:
	rm -f *.o demo mydisk
