all: md5.o hash.o main.o mv
	gcc -Wall -o integrctrl obj/md5.o obj/hash.o obj/main.o
	
main.o: src/main.c
	gcc -c src/main.c
	
hash.o: src/main.c
	gcc -c src/hash.c
	
md5.o: src/md5.c
	gcc -c src/md5.c
	
mv:
	mv *.o obj/
	
.PHONY: clean

clean:
	rm obj/*
