#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAXARGS	20
#define ARGLEN	100

void execute(char**);
char* makestring(char*);

int main()
{
	char *arglist[MAXARGS + 1];
	int numargs;
	char argbuf[ARGLEN];
	char *makestring();

	numargs = 0;
	while(numargs < MAXARGS)
	{
		char *cp;
		int bp = 0;
		printf("Arg[%d]? ", numargs);
		if(fgets(argbuf, ARGLEN, stdin) && *argbuf != '\n')
		{
			cp = makestring(argbuf);
			if(strcmp(cp, "exit") == 0)
				bp = 1;
			else	
				arglist[numargs++] = cp;
		}
		else if(feof(stdin))
			break;
		else
		{
			if(numargs > 0)
			{
				arglist[numargs] = NULL;
				execute(arglist);
				numargs = 0;
			}
		}
		if(bp == 1)
			break;
	}
	return 0;
}

void execute( char *arglist[] )
{
	int pid, exitstatus;

	pid = fork();
	switch(pid)
	{
		case -1:
			perror("fork failed");
			exit(1);
		case 0:
			execvp(arglist[0], arglist);
			perror("execvp failed");
			exit(1);
		default:
			while(wait(&exitstatus) != pid)
				;
			printf("child exited with status %d,%d\n",
					exitstatus>>8, exitstatus&0377);
	}
}

char *makestring( char *buf )
{
	char *cp;

	buf[strlen(buf)-1] = '\0';
	cp = malloc(strlen(buf)+1);
	if(cp == NULL)
	{
		fprintf(stderr, "no memory\n");
		exit(1);
	}
	strcpy(cp, buf);
	return cp;
}

