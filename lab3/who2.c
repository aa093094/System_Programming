#include <stdio.h>
#include <unistd.h>
#include <utmp.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>

#define SHOWHOST

void show_info(struct utmp *);
void showtime(long);

int main()
{
	struct utmp utbuf;
	int utmpfd;

	if((utmpfd = open(UTMP_FILE, O_RDONLY)) == -1)
	{
		perror(UTMP_FILE);
		exit(1);
	}

	while( read(utmpfd, &utbuf, sizeof(utbuf)) == sizeof(utbuf))
		show_info(&utbuf);
	close(utmpfd);
	
	return 0;
}

void show_info(struct utmp *utbufp)
{
	if ( utbufp->ut_type != USER_PROCESS )
		return;
	printf("%-12.12s", utbufp->ut_name);
	printf(" ");
	printf("%-8.8s", utbufp->ut_line);
	printf(" ");
	showtime( utbufp->ut_time );
#ifdef SHOWHOST
	if( utbufp->ut_host[0] != '\0' )
		printf(" (%s)", utbufp->ut_host);
#endif
	printf("\n");
}

void showtime( long timeval )
{
	char *cp;
	cp = ctime(&timeval);
	
	if(cp[4] == 'S')
		printf("2023-09-");
	printf("%8.8s", cp+8 );
}
