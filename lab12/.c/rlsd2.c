#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <ctype.h>
#include <strings.h>
#include <sys/wait.h>

#define PORTNUM 13899
#define HOSTLEN 256
#define oops(msg)	{ perror(msg); exit(1); }

void sanitize(char * );

int main(int ac, char *av[])
{
	struct sockaddr_in saddr;
	struct hostent *hp;
	char hostname[HOSTLEN];
	int sock_id, sock_fd;
	FILE *sock_fpi, *sock_fpo;
	char dirname[BUFSIZ];
	char command[BUFSIZ];
	int dirlen, c;

	sock_id = socket( PF_INET, SOCK_STREAM, 0 );
	if ( sock_id == -1 )
		oops( "socket" );

	bzero( (void *)&saddr, sizeof(saddr) );
	gethostname( hostname, HOSTLEN );
	hp = gethostbyname( hostname );
	bcopy( (void *)hp->h_addr, (void *)&saddr.sin_addr, hp->h_length);
	saddr.sin_port = htons(PORTNUM);
	saddr.sin_family = AF_INET;
	if ( bind(sock_id, (struct sockaddr *)&saddr, sizeof(saddr)) != 0 )
		oops( "bind" );

	if ( listen(sock_id, 1) != 0 )
		oops( "listen" );

	while(1)
	{
		sock_fd = accept(sock_id, NULL, NULL);
		if ( sock_fd == -1 )
			oops("accept");

		if( (sock_fpi = fdopen(sock_fd, "r")) == NULL )
			oops("fdopen reading");

		if ( fgets(dirname, BUFSIZ-5, sock_fpi) == NULL )
			oops("reading dirname");
		sanitize(dirname);

		if ( (sock_fpo = fdopen(sock_fd, "w")) == NULL )
			oops("fdopen writing");

		int pipe_fds[2];
		if (pipe(pipe_fds) == -1)
			oops("pipe");

		pid_t child_pid = fork();

		if (child_pid == -1)
		{
			oops("fork");
		}
		else if (child_pid == 0)
		{
			close(pipe_fds[0]);
			dup2(pipe_fds[1], STDOUT_FILENO);

			execlp("ls", "ls", dirname, NULL);
			oops("execlp");
		}
		else
		{
			close(pipe_fds[1]);
			FILE *pipe_fp = fdopen(pipe_fds[0], "r");
	
			while( (c = getc(pipe_fp)) != EOF )
				putc(c, sock_fpo);

			fclose(pipe_fp);
			fclose(sock_fpo);

			int status;
			waitpid(child_pid, &status, 0);
		}

		fclose(sock_fpi);
	}

	return 0;
}

void sanitize(char *str)
{
	char *src, *dest;

	for ( src = dest = str ; *src ; src++ )
		if ( *src == '/' || isalnum(*src) )
			*dest++ = *src;
	*dest = '\0';
}

