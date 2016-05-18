all: obj md5.o hash.o integ.o main.o
	gcc -Wall -o integrctrl obj/md5.o obj/hash.o obj/integ.o obj/main.o
	
obj:
	mkdir obj/

main.o: src/main.c
	gcc -o obj/main.o -c src/main.c
	
integ.o: src/integ.c
	gcc -o obj/integ.o -c src/integ.c

hash.o: src/main.c
	gcc -o obj/hash.o -c src/hash.c
	
md5.o: src/md5.c
	gcc -o obj/md5.o -c src/md5.c
	
.PHONY: clean

clean:
	rm obj/*
