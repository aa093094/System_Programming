#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	int pid;
	if (argc != 2)
	{
		fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
		exit(1);
	}

	printf("About to run sort with input from %s\n", argv[1]);
	if ( (pid = fork() ) == -1)
	{
		perror("fork"); exit(1);
	}

	if (pid == 0)
	{
		execlp("sort", "sort", argv[1], (char *)NULL);

		perror("execlp");
		exit(1);
	}

	if (pid != 0)
	{
		wait(NULL);
		printf("Done running sort < %s\n", argv[1]);
	}

	return 0;
}

