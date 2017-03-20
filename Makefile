CC = gcc 
<<<<<<< HEAD
all: clean minget
	$(CC) fs.c -o minls
	export PATH=$$PATH:$$PWD
=======
all:
	$(CC) verbose.c fs.c -o minls
>>>>>>> 7e121bfeeccec627ba471393c6422e7675975658

minget:
	$(CC) minget.c -o minget
clean:
	rm -f minls minget
