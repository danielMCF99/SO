#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <string.h>
#include <sys/wait.h>
#include "argus.h"

int filho2 = -1;

ssize_t readln(int fd, char* buffer, ssize_t buffLength) {
    int size = 0;
	ssize_t res = 0;
	char c ;
    while((res = read(fd,&c,1)) > 0 && size < buffLength) {
		buffer[size] = c;
		if(c == '\n') return size;
        size++;
    }
	buffer[size] = '\0';
    return size;
}

void resposta(char* pid,Response resp){
	int fd2;
	char buffer[MAXSIZE];
	sprintf(buffer,"%s",pid);
	fd2 = open(buffer,O_CREAT | O_WRONLY, 0666);
	ssize_t res = write(fd2,resp,sizeof(*resp));
	if(res == -1) perror("MENSAGEM");
	close(fd2);
}	

char** parser(char* buffer){
    char** args = malloc(sizeof(char) * MAXSIZE);
    args[0] = strtok(buffer," ");
    if(strcmp(args[0],"-e") == 0 || strcmp(args[0],"executar") == 0){
        args[1] = strtok(NULL,"\'");
    }else{
        args[1] = strtok(NULL," ");
    }
    return args;
}

int parse_espaco(char* args, char** parsed){
    char* token = strtok(args," ");
    parsed[0] = token;
    int i = 1;
    while(token != NULL){
        token = strtok(NULL," ");
        parsed[i] = token;
        i++;
    }
    parsed[i] = NULL;
    i++;
    return i;
}

int count_pipe(char* args, char** ret){
	char* token = strtok(args,"|");
    ret[0] = token;
    int i = 1;
    while(token != NULL){
        token = strtok(NULL,"|");
        ret[i] = token;
        i++;
    }
    ret[i] = NULL;
    i++;
    return i;
}

void catch_alarm(){
	kill(filho2,SIGTERM);
}

void executar(int npipes,char* pid,int logFile,char* parsedpipe[MAXSIZE],int tmaxe){
	signal(SIGALRM,catch_alarm);
	int pant[2];
	int pseg[2];
	int N = npipes - 2;
	for(int i = 0; i < N; i++){
		if(i != N-1){
			pipe(pseg);
		}
		if((filho2 = fork()) == 0){
			alarm(tmaxe);
			if(i != 0){
				dup2(pant[0],0);
				close(pant[0]);
			}

			if (i != N- 1){
				close(pseg[0]);
				dup2(pseg[1],1);
				close(pseg[1]);
			}

			char* args[strlen(parsedpipe[i])];
			parse_espaco(parsedpipe[i],args);
			execvp(args[0],args);
			perror(args[0]);
			exit(0);
		}else{
			if (i!= 0){
				close(pant[0]);
			}

			if (i != N-1){
				close(pseg[1]);
				pant[0] = pseg[0];
			}
		}
	}
	for(int i = 0; i<N; i++){
		wait(&i);
	}
}

void ajuda(){
	char texto[MAXSIZE] = "";
	strcat(texto,"tempo-inatividade(-i) segs\n");
	strcat(texto,"tempo-execucao(-m) segs\n");
	strcat(texto,"executar(-e) 'p1 | p2 ... | pn'\n");
	strcat(texto,"lista(-l)\n");
	strcat(texto,"terminar(-t) n\n");
	strcat(texto,"historico(-r)\n");
	strcat(texto,"output(-o) n\n");
	write(1,texto,strlen(texto));
}
















