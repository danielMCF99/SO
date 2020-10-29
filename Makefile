all: argus argusd

argus: argus.c
	gcc -o argus argus.c comandos.c argus.h

argusd: argusd.c
	gcc -o argusd argusd.c comandos.c argus.h

clean:
	rm argusd argus log.bin historico.idx
