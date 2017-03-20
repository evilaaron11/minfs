CC = gcc 
all: clean minget
	$(CC) verbose.c fs.c -o minls
	export PATH=$$PATH:$$PWD

minget:
	$(CC) minget.c -o minget
clean:
	rm -f minls minget
