#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

ino_t get_inode(char *);
void printpathto(ino_t, char *);
void inum_to_name(ino_t, char *, int );

int main()
{
	char *username;
	username = getenv("USER");
	if( username == NULL )
	{
		username = getenv("LOGNAME");
	}
	printpathto( get_inode( "." ), username );
	putchar('\n');
	return 0;
}

void printpathto( ino_t this_inode, char *username )
{
	ino_t my_inode;
	char its_name[BUFSIZ];

	if ( get_inode("..") != this_inode )
	{
		chdir( ".." );

		inum_to_name(this_inode, its_name, BUFSIZ);
		if( strcmp(username, its_name) == 0 )
		{
			printf("~");
		}
		else
		{
			my_inode = get_inode( "." );
			printpathto( my_inode, username );
			printf("/%s", its_name );
		}
		
	}
}

void inum_to_name(ino_t inode_to_find, char *namebuf, int buflen)
{
	DIR *dir_ptr;
	struct dirent *direntp;

	dir_ptr = opendir( "." );
	if ( dir_ptr == NULL )
	{
		perror( "." );
			exit(1);
	}

	while ( ( direntp = readdir ( dir_ptr ) ) != NULL )
		if ( direntp->d_ino == inode_to_find )
		{
			strncpy( namebuf, direntp->d_name, buflen);
			namebuf[buflen-1] = '\0';
			return;
		}
	fprintf(stderr, "error looking for inum %ld\n", inode_to_find);
	exit(1);
}

ino_t get_inode( char *fname )
{
	struct stat info;

	if ( stat( fname, &info ) == -1 )
	{
		fprintf(stderr, "Cannot stat ");
		perror(fname);
		exit(1);
	}
	return info.st_ino;
}

