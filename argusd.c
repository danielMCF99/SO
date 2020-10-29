#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include "argus.h"

/*
	função para remover o fifo "serverPipe" quando houver uma interrupçao
	(control c)
*/
void inthandler_servidor(){
	remove(serverPipe);
	_exit(0);
}

/*
	@param pid processo cliente que enviou o comando
	@param logFile fd do ficheiro em que vai escrever o output
	@param historico fd do ficheiro historico.idx(para escrita)
	@param lerhistorico fd do ficheiro historico.idx(para leitura)
	@param op que o cliente escreve
	@param line argumentos da op
	@param tmaxe tempo maximo de execuçao para tarefa
*/
int fun(char* pid,int logFile,int historico,int lerhistorico,char* op,char* line,int* tmaxe){
	char* aux[MAXSIZE];
	char* comando = strdup(op);
	char* linha = strdup(line);

	if (strcmp(op,"-i") == 0 || strcmp(op,"tempo-inactividade") == 0){
		
	}
	else if (strcmp(op,"-m") == 0 || strcmp(op,"tempo-execucao") == 0){
		*tmaxe = atoi(line);

	}
	else if (strcmp(op,"-e") == 0 || strcmp(op,"executar") == 0){
		int x = count_pipe(line,aux);
		int arr[2];
		pipe(arr);
		if (fork() == 0){
			close(arr[0]);
			dup2(arr[1],1);
			close(arr[1]);
			executar(x,pid,logFile,aux,*tmaxe);
			_exit(0);
		}else{
			close(arr[1]);
			int n;
			Response resp = (Response) malloc(sizeof(struct response));
			while((n = read(arr[0],resp->output,MAXSIZE)) > 0){
				write(logFile,resp->output,n);
			}
			close(arr[0]);
			resposta(pid,resp);
			wait(NULL);
		}
		
	}
	else if (strcmp(op,"-l") == 0 || strcmp(op,"listar") == 0){
		
	}
	else if (strcmp(op,"-t") == 0 || strcmp(op,"terminar") == 0){

	}
	else if (strcmp(op,"-h") == 0 || strcmp(op,"ajuda") == 0){
		int arr[2];
		pipe(arr);
		if(fork() == 0){
			close(arr[0]);
			dup2(arr[1],1);
			close(arr[1]);
			ajuda();
			_exit(0);
		}else{
			close(arr[1]);
			int n;
			Response resp = (Response) malloc(sizeof(struct response));
			while((n = read(arr[0],resp->output,MAXSIZE)) > 0){
				write(logFile,resp->output,n);
			}
			close(arr[0]);
			resposta(pid,resp);
			wait(NULL);
		}
		
	}
	else if (strcmp(op,"-r") == 0 || strcmp(op,"historico") == 0){
		Response resp = (Response) malloc(sizeof(struct response));
		int lidos = 0;
		int n;
		do{
			n = read(lerhistorico,resp->output,MAXSIZE);
			lidos += n;
		}while (n != 0);
		lseek(lerhistorico,-lidos,SEEK_CUR);
		resposta(pid,resp);
	}
	else if (strcmp(op,"-o") == 0 || strcmp(op,"output") == 0){

		
	}
	strcat(comando," ");
	strcat(comando,linha);
	comando = strtok(comando,"\n");
	strcat(comando,"\n");
	write(historico,comando,strlen(comando));
	return 1;
}

//MAIN SERVIDOR
void sv(){
	signal(SIGINT,inthandler_servidor);
	int servidor,ficheiro,historico,lerhistorico,lerdados,fd1;
	char pid[10]; 
	int tmaxe = TMAX;
	servidor = mkfifo(serverPipe,0666);
	if(servidor<0){
		perror("FIFO");
	}else{
		puts("Servidor esta aberto");
	}
	fd1 = open(serverPipe,O_RDONLY);
	ficheiro = open("log.bin",O_CREAT | O_WRONLY | O_APPEND, 0666);
	historico = open("historico.idx",O_CREAT | O_WRONLY | O_APPEND, 0666);
	lerhistorico = open("historico.idx",O_RDONLY);
	while(1){ 
		Action dados = (Action) malloc(sizeof(struct action));
		lerdados = read(fd1,dados,sizeof(struct action));
		char* op; 
		char* line;
		if(lerdados > 0){
			sprintf(pid,"%d",dados->pid);
			op = dados->opcao;
			line = dados->linha;
			if(strcmp(line,"") == 0){
				op = strtok(op,"\n");
			}
			fun(pid,ficheiro,historico,lerhistorico,op,line,&tmaxe);
		}
	}
	close(fd1);
	close(historico);
	close(ficheiro);
}

//MAIN
int main() {
	sv();
	return 0;
}