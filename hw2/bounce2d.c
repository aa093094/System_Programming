#include <stdio.h>
#include <curses.h>
#include <signal.h>
#include <string.h>

#define TOP_ROW	5
#define BOT_ROW	20
#define LEFT_EDGE	10
#define RIGHT_EDGE	70
#define MESSAGE "o"
#define BLANK " "

void move_xmsg(int);
void move_ymsg(int);
void move_xymsg(int);
void move_msg(int);
int set_ticker(int);

int row;
int col;
int x_dir;
int y_dir;
int x_delay;
int y_delay;
int x_ndelay;
int y_ndelay;

int main()
{
	int c;
	void move_xmsg(int);
	void move_ymsg(int);
	void move_xymsg(int);
	void move_msg(int);

	initscr();
	crmode();
	noecho();
	clear();

	row = 10;
	col = 40;
	x_dir = 1;
	y_dir = 1;
	x_delay = 50;
	y_delay = 50;

	move(row, col);
	addstr(MESSAGE);
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	set_ticker(x_delay);
	set_ticker(y_delay);
	signal(SIGALRM, move_msg);

	while(1)
	{
		x_ndelay = 50;
		y_ndelay = 50;
		c = getch();
		if (c == 'Q') break;
		if (c == ' ') 
		{
			x_dir = -x_dir;
			y_dir = -y_dir;
		}
		if (c == 'f' && x_delay > 2) x_ndelay = x_delay/2;
		if (c == 'F' && y_delay > 2) y_ndelay = y_delay/2;
		if (c == 's') x_ndelay = x_delay * 2;
		if (c == 'S') y_ndelay = y_delay * 2;
		if (y_ndelay > 0 && (x_ndelay > y_ndelay))
		{
			x_delay = x_ndelay;
			set_ticker(y_delay = y_ndelay);
		}
		else if (x_ndelay > 0 && (y_ndelay > x_ndelay))
		{
			y_delay = y_ndelay;
			set_ticker(x_delay = x_ndelay);
		}
		else if ((x_ndelay > 0 && y_ndelay > 0) && x_ndelay == y_ndelay)
		{
			x_delay = x_ndelay;
			set_ticker(y_delay = y_ndelay);
		}
	}
	endwin();
	return 0;
}

void move_xmsg(int signum)
{
	move(row, col);
	addstr(BLANK);
	col += x_dir;
	move(row, col);
	addstr(MESSAGE);
	refresh();

	if (x_dir == -1 && col <= LEFT_EDGE)
		x_dir = 1;
	else if (x_dir == 1 && col+strlen(MESSAGE) >= RIGHT_EDGE)
		x_dir = -1;
}

void move_ymsg(int signum)
{
	move(row, col);
	addstr(BLANK);
	row += y_dir;
	move(row, col);
	addstr(MESSAGE);
	refresh();

	if (y_dir == -1 && row <= TOP_ROW)
		y_dir = 1;
	else if ( y_dir == 1 && row + strlen(MESSAGE) >= BOT_ROW)
		y_dir = -1;
}

void move_xymsg(int signum)
{
	move(row, col);
	addstr(BLANK);
	col += x_dir;
	row += y_dir;
	move(row, col);
	addstr(MESSAGE);
	refresh();
		
	if (x_dir == -1 && col <= LEFT_EDGE)
		x_dir = 1;
	else if (x_dir == 1 && col + strlen(MESSAGE) >= RIGHT_EDGE)
		x_dir = -1;
	if (y_dir == -1 && row <= TOP_ROW)
		y_dir = 1;
	else if(y_dir == 1 && row + 1 >= BOT_ROW)
		y_dir = -1;
}

void move_msg(int signum)
{
	signal(SIGALRM, move_msg);
	int cnt;
	move(row, col);
	addstr(BLANK);
	if (x_delay > y_delay)
	{
		cnt = x_delay / y_delay;
		row += (y_dir * cnt);
		col += x_dir;
		if (row + 1 >= BOT_ROW)
		{
			row = BOT_ROW - 1;
			y_dir = -1;
		}
		else if(row <= TOP_ROW)
		{
			row = TOP_ROW;
			y_dir = 1;
		}
		move(row, col);
		addstr(MESSAGE);
		refresh();
		if (x_dir == -1 && col <= LEFT_EDGE)
			x_dir = 1;
		else if (x_dir == 1 && col + strlen(MESSAGE) >= RIGHT_EDGE)
			x_dir = -1;
		if (y_dir == -1 && row <= TOP_ROW)
			y_dir = 1;
		else if(y_dir == 1 && row + 1 >= BOT_ROW)
			y_dir = -1;

	}
	else if ( y_delay > x_delay)
	{
		cnt = y_delay / x_delay;
		row += y_dir;
		col += (x_dir * cnt);
		if (col <= LEFT_EDGE)
		{
			col = LEFT_EDGE;
			x_dir = 1;
		}
		else if (col + strlen(MESSAGE) >= RIGHT_EDGE)
		{
			col = RIGHT_EDGE - strlen(MESSAGE);
			x_dir = -1;
		}
		move(row, col);
		addstr(MESSAGE);
		refresh();
		if (x_dir == -1 && col <= LEFT_EDGE)
			x_dir = 1;
		else if (x_dir == 1 && col + strlen(MESSAGE) >= RIGHT_EDGE)
			x_dir = -1;
		if (y_dir == -1 && row <= TOP_ROW)
			y_dir = 1;
		else if(y_dir == 1 && row + 1 >= BOT_ROW)
			y_dir = -1;

	}
	else
	{
		row += y_dir;
		col += x_dir;
		move(row, col);
		addstr(MESSAGE);
		refresh();
		if (x_dir == -1 && col <= LEFT_EDGE)
			x_dir = 1;
		else if (x_dir == 1 && col + strlen(MESSAGE) >= RIGHT_EDGE)
			x_dir = -1;
		if (y_dir == -1 && row <= TOP_ROW)
			y_dir = 1;
		else if(y_dir == 1 && row + 1 >= BOT_ROW)
			y_dir = -1;

	}
}

