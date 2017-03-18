CC = gcc 
all:
	$(CC) verbose.c fs.c -o minls

clean:
	rm minls
