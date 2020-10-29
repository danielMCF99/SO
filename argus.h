#ifndef ARGUS_H
#define ARGUS_H

#include <fcntl.h>
#include <unistd.h>

//#define PATH "/bin/"
#define serverPipe "serverPipe"
#define MAXSIZE 4096
#define TMAX 500

typedef struct action* Action;
typedef struct response* Response;

struct action{
    pid_t pid;
    char opcao[35]; //-e // -i 
    char linha[300]; //ls -la | wc // 10
};

struct response{
    char output[MAXSIZE]; //output calculado no servidor
};

ssize_t readln(int fd, char* buffer, ssize_t buffLength);

void resposta(char* pid,Response resp);//envia a resposta para o fifo especifico de cada cliente

char** parser(char* buffer);//separa o que cliente escreve num array com 2 posiçoes

int parse_espaco(char* args, char** parsed);//separa os argumentos a executar por espaços

int count_pipe(char *args, char** ret); //conta o numero de pipes

void catch_alarm();

void executar(int npipes,char* pid,int logFile,char** parsedpipe,int tmaxe);//executar um comado "-e"

void ajuda(); //mostra o menu das ajudas

#endif 

