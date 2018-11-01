#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
#define MAX_LINE 80
char signals[50][80];
int left=0, right=0;

void setup(char inputBuffer[], char *args[], int *background){
	int length, start=-1, ans=0;
	length=read(STDIN_FILENO, inputBuffer, MAX_LINE);
	
	if(length==0) exit(0);
	if(length<0){
		perror("error reading the command");
		exit(-1);
	}
	inputBuffer[length]='\0';

	strcpy(signals[right++], inputBuffer);

	if(inputBuffer[0]=='r' && (inputBuffer[1]==' ' || inputBuffer[1]=='\t')){
		for(int i=right-2; i>=left; i--){
			if(signals[i][0]==inputBuffer[2]){
				strcpy(inputBuffer, signals[i]);
				length=strlen(inputBuffer);
				break;
			}
		}
	}
	if(right-left>10) left++;
	for(int i=0;i<length;i++){
		switch(inputBuffer[i]){
			case ' ':
			case '\t':
				if(start!=-1)
					args[ans++]=&inputBuffer[start];
				inputBuffer[i]='\0';
				start=-1;
				break;
			case '\n':
				if(start!=-1)
					args[ans++]=&inputBuffer[start];
				inputBuffer[i]='\0';
				args[ans]=NULL;
				break;
			default:
				if(start==-1)
					start=i;
				if(inputBuffer[i]=='&'){
					*background=1;
					inputBuffer[i]='\0';
				}
		}
	}
	args[ans]=NULL;
}

void handle_SIGINT(){
	printf("\nThe history signals that you have entered: \n");
	for(int i=right-1; i>=left; i--){
		write(STDOUT_FILENO, signals[i], strlen(signals[i]));
	}
	printf("$CMD->");
	fflush(stdout);
}

int main(){
	char inputBuffer[MAX_LINE];
	int background;
	char *args[MAX_LINE/2+1];

	// struct sigaction handler;
	// handler.sa_handler = handle_SIGINT;
	// sigaction(SIGINT, &handler, NULL);
	signal(SIGINT, &handle_SIGINT);

	while(1){
		background=0;

		printf("$CMD->");
		fflush(stdout);
	
		setup(inputBuffer, args, &background);
		
		pid_t pid=fork();
		if(pid==0){
			execvp(args[0], args);
			kill(getpid(), SIGKILL);
		}
		else{
			if(background==0){
				waitpid(pid, NULL, 0);
			}
		}
	}
	return 0;
}