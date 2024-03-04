#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include "smsh5.h"
#include "varlib.h"

void handle_ctrl_c(int);
void set_signal_handler();

int execute(char *argv[], int background)
{
	extern char **environ;

	int pid;
	int child_info = -1;

	if ( argv[0] == NULL )
		return 0;

	if ( (pid = fork()) == -1 )
		perror("fork");
	else if ( pid == 0 )
	{
		environ = VLtable2environ();
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execvp(argv[0], argv);
		perror("cannot execute command");
		exit(1);
	}
	else
	{
		if (!background)
			waitpid(pid, NULL, 0);
		else
			printf("%s (PID: %d)\n", argv[0], pid);
	}
	return child_info;
}

