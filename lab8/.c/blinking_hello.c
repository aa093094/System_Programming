#include <stdio.h>
#include <curses.h>
#include <unistd.h>

int main()
{
	char message[] = "Hello, world";
	char blank[] = "            ";

	initscr();

	clear();
	
	while(1)
	{
		refresh();
		sleep(1);
		move(10, 20);
		addstr(message);
		move(LINES-1, COLS-1);
		refresh();
		sleep(1);
		move(10, 20);
		addstr(blank);
		move(LINES-1, COLS-1);
	}

	return 0;
}
