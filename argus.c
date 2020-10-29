#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include "argus.h"

void inthandler_cliente(){
	char clientPipe[50] = "";
	pid_t pid = getpid();
	sprintf(clientPipe,"%d",pid);
	remove(clientPipe);
	_exit(0);
}

void pai(){
	pid_t pid = getpid();
	int fd = open(serverPipe,O_WRONLY);
	if (fd > 0){
		puts("Entrou no servidor");
		while(1){
			char buffer[MAXSIZE] = {0};
			if(readln(0,buffer,MAXSIZE) > 0){
				if(strlen(buffer) > 0){
					Action act= (Action) malloc(sizeof(struct action));
					act->pid = pid;
					char** args = parser(buffer);
					strcpy(act->opcao,args[0]);
					if (args[1] != NULL){
						strcpy(act->linha,args[1]);
					}else{
						strcpy(act->linha,"");
					}
					write(fd,act,sizeof(*act));
				}
			}
		}
		close(fd);
	}else{
		puts("Servidor desligado");
	}
}

void filho(){
	Response resp = (Response) malloc(sizeof(struct response));
	char clientPipe[MAXSIZE] = "";
	pid_t pid = getppid();
	sprintf(clientPipe,"%d",pid);
	mkfifo(clientPipe,0666);
	int fd1 = open(clientPipe,O_RDONLY);
	while(1){
		if(fd1 < 0) break;
		int tam = read(fd1,resp,sizeof(struct response));
		if (tam > 0){
			write(1,resp->output,sizeof(resp->output));
			puts("\n");
		}
		
	}
	close(fd1);
	remove(clientPipe);
	_exit(0);
}

int main(int argc,char* argv[]){
	signal(SIGINT,inthandler_cliente);
	if (argc == 1){
		if (fork() != 0){
			pai();
		}else{
			filho();
		}
	}
	else if(argc > 1){
		if (fork() != 0){
			pid_t pid = getpid();
			int fd = open(serverPipe,O_WRONLY);
			if (fd > 0){
				Action act= (Action) malloc(sizeof(struct action));
				act->pid = pid;
				strcpy(act->opcao,argv[1]);
				if (argv[2] != NULL){
						strcpy(act->linha,argv[2]);
				}else{
						strcpy(act->linha,"");
				}
				write(fd,act,sizeof(*act));
			}else{
				puts("Servidor desligado");
			}
			close(fd);
			wait(NULL);
		}else{
			Response resp = (Response) malloc(sizeof(struct response));
			char clientPipe[MAXSIZE] = "";
			pid_t pid = getppid();
			sprintf(clientPipe,"%d",pid);
			mkfifo(clientPipe,0666);
			int fd1 = open(clientPipe,O_RDONLY);
			int tam = read(fd1,resp,sizeof(struct response));
			while ((tam = read(fd1,resp,sizeof(struct response))) > 0){
				write(1,resp->output,sizeof(resp->output));
			}
			close(fd1);
			remove(clientPipe);
			exit(0);
		}
	}
	return 0;
}