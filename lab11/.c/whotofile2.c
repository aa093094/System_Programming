#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

int main(int ac, char **av)
{
	int pid;
	int fd;
	
	printf("About to run who into a file\n");

	if( (pid = fork() ) == -1 )
	{
		perror("fork"); exit(1);
	}

	if ( pid == 0 )
	{
		close(1);
		fd = open( av[1], O_WRONLY | O_APPEND);
		if (fd == -1)
		{
			perror("File not exists");
			exit(1);
		}
		execlp( "who", "who", NULL );
		perror("execlp");
		exit(1);
	}

	if ( pid != 0 )
	{
		wait(NULL);
	}

	return 0;
}

