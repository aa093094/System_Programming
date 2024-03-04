#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "smsh5.h"
#include "varlib.h"

#define DFL_PROMPT	"> "

pid_t background_pid = 0;

int main()
{
	char	*cmdline, *prompt, **arglist;
	int	result, process(char **, int);
	void	setup();
	background = 0;

	prompt = DFL_PROMPT;
	setup();

	while ( (cmdline = next_cmd(prompt, stdin)) != NULL )
	{
		if (background_pid != 0)
		{
			int status;
			waitpid(background_pid, &status, 0);
			background_pid = 0;
		}

		if ( (arglist = splitline(cmdline)) != NULL )
		{
			result = process(arglist, background);
			if (background)
			{
				background_pid = fork();
				if ( background_pid == 0)
				{
					exit(result);
				}
			}
			freelist(arglist);
		}
		free(cmdline);
	}

	if(background_pid != 0)
	{
		int status;
		waitpid(background_pid, &status, 0);
	}

	return 0;
}

void setup()
{
	extern char **environ;

	VLenviron2table(environ);
	signal(SIGINT, handle_ctrl_c);
	signal(SIGQUIT, SIG_IGN);
}

void fatal(char *s1, char *s2, int n)
{
	fprintf(stderr, "Error: %s,%s\n", s1, s2);
	exit(n);
}

void handle_ctrl_c(int signum)
{
	if(background)
	{
		printf("\nTerminating background process.\n");
		background = 0;
		kill(background_pid, SIGTERM);
		waitpid(background_pid, NULL, 0);
		background_pid = 0;
	}
	else
	{
		printf("^C");
	}
}
