#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>

#define INPUT_STRING 80
#define SIMPLE 		0
#define PIPELINE 	1
#define BACKGROUND	2


void parse(char *input, char *cmdArgv[], char **supplePtr, int *modePtr)
{
	int  end = 0;
	char *src = input;
	
	while(*src != '\0' && end == 0)
	{
		*cmdArgv = src;
	
	
		while(*src != ' ' && *src != '\t' && *src != '\0' && *src != '\n' && end == 0)
		{
			switch(*src)
			{
				case '&':
					*modePtr = BACKGROUND;
					break;
				case '|':
					*modePtr = PIPELINE;
					*cmdArgv = '\0';
					src++;
					while(*src == ' ' || *src == '\t')
						src++;
					*supplePtr = src;
			
					end = 1;
					break;
			}
			src++;
		}
		while((*src == ' ' || *src == '\t' || *src == '\n') && end == 0)
		{
			*src = '\0';
			src++;
		}
		cmdArgv++;
	}
	
	*cmdArgv = '\0';
	
}



void execute(char **cmdArgv, int mode, char **supplePtr)
{
	pid_t pid, pid2;
	
	int mode2 = SIMPLE, cmdArgc, status1, status2;
	char *cmdArgv2[INPUT_STRING], *supple2 = NULL;
	int myPipe[2];
	if(mode == PIPELINE)
	{
		if(pipe(myPipe))
		{
			fprintf(stderr, "Pipe failed!");
			exit(-1);
		}
		parse(*supplePtr, cmdArgv2, &supple2, &mode2);
	}
	pid = fork();
	if( pid < 0)
	{
		printf("Error occured");
		exit(-1);
	}
	else if(pid == 0)
	{
		switch(mode)
		{
			case PIPELINE:
				close(myPipe[0]);		
				dup2(myPipe[1], fileno(stdout));
				close(myPipe[1]);
				break;
		}
		execvp(*cmdArgv, cmdArgv);
	}
	else
	{
		if(mode == BACKGROUND)
			;
		else if(mode == PIPELINE)
		{
			waitpid(pid, &status1, 0);	
			pid2 = fork();
			if(pid2 < 0)
			{
				printf("error in forking");
				exit(-1);
			}
			else if(pid2 == 0)
			{
				close(myPipe[1]);		
				dup2(myPipe[0], fileno(stdin));
				close(myPipe[0]);
				execvp(*cmdArgv2, cmdArgv2);
			}
			else
			{
	
				close(myPipe[0]);
				close(myPipe[1]);
			}
		}
		else
			waitpid(pid, &status1, 0);
	
	}
}




void main()
{
	int mode = SIMPLE;
	size_t len = INPUT_STRING;
	char  *input, *cmdArgv[INPUT_STRING], *supple = NULL;
	input = (char*)malloc(sizeof(char)*INPUT_STRING);
	
	char curDir[100];
	
	while(1)
	{
		mode = SIMPLE;
		getcwd(curDir, 100);
		printf("%s@%s->SDREAM->", getlogin(),curDir);
		getline( &input, &len, stdin);
		
		 parse(input, cmdArgv, &supple, &mode);
		 if(strcmp(*cmdArgv, "exit") == 0)
			exit(0);
		if(strcmp(*cmdArgv, "cd") == 0)
		{
			chdir(cmdArgv[1]);
		}
		else 
			execute(cmdArgv, mode, &supple);
	}
	
}



