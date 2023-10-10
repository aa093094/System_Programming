#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#define BUFFERSIZE 4096
#define COPYMODE 0644

void oops(char *, char *);

int main(int ac, char *av[])
{
	int in_fd, out_fd, n_chars;
	char buf[BUFFERSIZE];
	char destination_file[PATH_MAX];
	const char *source_file = av[1];
	const char *destination = av[2];
	int result = strcmp(av[1], av[2]);

	if(result == 0)
	{
		fprintf(stderr,"cp: '%s' and '%s' are the same file\n", av[1], av[2]);
		exit(1);
	}
	
	

	if( ac != 3 ) 
	{
		fprintf( stderr, "usage: %s source destination\n", *av);
		exit(1);
	}
	
	struct stat dest_stat;
	if ( stat(destination, &dest_stat) == 0 && S_ISDIR(dest_stat.st_mode))
	{
		const char *source_filename = source_file;
		const char *last_slash = strrchr(source_file, '/');
		if (last_slash != NULL) {
			source_filename = last_slash + 1;
		}
		snprintf(destination_file, sizeof(destination_file), "%s/%s", destination, source_filename);
		destination = destination_file;
	}


	if( (in_fd = open(av[1], O_RDONLY)) == -1 )
		oops("Cannot open ", av[1]);

	if( (out_fd=creat( destination, COPYMODE)) == -1 )
		oops("Cannot creat", av[2]);
	
	while( (n_chars = read(in_fd, buf, BUFFERSIZE)) > 0 )
		if ( write( out_fd, buf, n_chars ) != n_chars )
			oops("Write error to ", av[2]);
	if ( n_chars == -1 )
		oops("Read error from ", av[1]);

	if ( close(in_fd) == -1 || close(out_fd) == -1 )
		oops("Error closing files", "");
	
	return 0;
}

void oops(char *s1, char *s2)
{
	fprintf(stderr, "Error: %s ", s1);
	perror(s2);
	exit(1);
}	
