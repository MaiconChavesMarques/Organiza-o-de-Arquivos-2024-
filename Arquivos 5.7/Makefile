all: help.o leitura_escrita.o leitura_impressao.o ordenacao_binaria.o main.o
	gcc help.o leitura_escrita.o busca_impressao.o ordenacao_binaria.o main.o -o final -std=c99 -Wall

help.o:
	gcc -c help.c -o help.o

leitura_escrita.o:
	gcc -c leitura_escrita.c -o leitura_escrita.o

leitura_impressao.o:
	gcc -c busca_impressao.c -o busca_impressao.o

ordenacao_binaria.o:
	gcc -c ordenacao_binaria.c -o ordenacao_binaria.o

main.o:
	gcc -c main.c -o main.o
	 
clean:
	rm *.o final

run:
	./final
