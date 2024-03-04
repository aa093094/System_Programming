#include <stdio.h>
#include <stdlib.h>
#include <curses.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define TOP_ROW 1
#define BOT_ROW 35
#define LEFT_EDGE	2
#define RIGHT_EDGE	130
#define BLANK " "

struct card{
	char shape[10];
	char num[2];
	int score;
} card;

int n_cardnum, u_cardnum;
struct card p_deck[12];
struct card d_deck[12];
int p_setnum, d_setnum;
int p_sum, d_sum;
int coin = 100;

void init_screen();
void display_man();
void clear_screen();
void start_game();
void make_hcard(int col);
void make_vcard();
void game_process(struct card card_set[52]);
int* shuffle_deck();
void swap(int* a, int* b);
void initialize_game(struct card card_set[52], int* c_order);
void card_to_p(struct card card_set[52], int* c_order, int who);
void show_sum();
void show_ncard();
int check_win(int mode);
void show_coin();
int check_draw(int mode);

int main()
{
	char c_shape[4][10] = { "Spade", "Diamond", "Heart", "Clober" };
	char c_num[13][2] = { "A", "2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K" };	
	struct card card_set[52];
	n_cardnum = 52;
	u_cardnum = 0;

	int rows, cols;
	struct winsize size;
	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &size) == -1)
	{
		perror("ioctl");
	}
	else
	{
		rows = size.ws_row;
		cols = size.ws_col;
	}
	if (rows < 40 || cols < 135)
	{
		fprintf(stderr, "Error: Window size must be greater than 40 * 135\n");
		exit(EXIT_FAILURE);
	}


	for (int i = 0; i < 4; i++)
	{
		char n_shape[10];
		strcpy(n_shape, c_shape[i]);
		for (int j = 0; j < 13; j++)
		{
			char n_num[3];
			strcpy(n_num, c_num[j]);
			strcpy(card_set[i * 13 + j].shape, n_shape);
			strcpy(card_set[i * 13 + j].num, n_num);
			if (j >= 10)
				card_set[i * 13 + j].score = 10;
			else
				card_set[i * 13 + j].score = j + 1;
		}
	}
	
	initscr();
	crmode();
	noecho();
	clear();

	init_screen();
	display_man();
	clear_screen();
	start_game();
		
	game_process(card_set);

	endwin();
	return 0;
}

void init_screen()
{
	int row, col;

	row = TOP_ROW;
	col = LEFT_EDGE;
	for (col; col <= RIGHT_EDGE; col++)
	{
		row = TOP_ROW;
		move(row, col);
		addstr("-");
		row = BOT_ROW;
		move(row, col);
		addstr("-");
	}
	refresh();
	row = TOP_ROW;
	for (row; row <= BOT_ROW; row++)
	{
		col = LEFT_EDGE;
		move(row, col);
		addstr("|");
		col = RIGHT_EDGE;
		move(row, col);
		addstr("|");
	}
	refresh();
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void display_man()
{
	int row, col;
	int c;

	row = TOP_ROW + 10;
	col = LEFT_EDGE + 10;
	move(row, col);
	addstr("1. This game is blackjack. Blackjack is the game that the people who makes the sum of numbers to 21");
	row += 1;
	move(row, col);
	addstr("or close to 21 win the game.");
	
	row += 2;
	move(row, col);
	addstr("2. You will play with cpu dealer whose action is predefined. Dealer always hits when his sum is less than 17.");
	row += 1;
	move(row, col);
	addstr("So dealer's sum is equal or better than 17, he stays.");
	row += 1;
	move(row, col);
	addstr("\\hit means adding a card");
	
	row += 2;
	move(row, col);
	addstr("3. You can decide hit or stay. If you decided to hit and your sum exceed 21, you burst. If dealer and you burst");
	row += 1;
	move(row, col);
	addstr("at the same time, you draw. But only you have burst, you lose. Only dealer have burst, you win.");
	
	row += 2;
	move(row, col);
	addstr("4. Anyone who makes sum to 21 is winner. But you and dealer both decided to stay, anyone who makes sum close to 21");
	row += 1;
	move(row, col);	
	addstr("is winner. Basic bet is 10, and you received 20 when you win. You can surrender if you want.");
	row += 1;
	move(row, col);
	addstr("If you surrender, you only lose 5 coin. You will have 100 coins at the start of the game.");
	
	row += 2;
	move(row, col);
	addstr("5. Aces' score are treated as 1. J, Q, K's score are treated as 10. Number 10 represents as T.");
	
	row += 2;
	move(row, col);
	addstr("6. The number of cards in deck is 52. If the number of left cards go under 15, reshuffle the deck.");

	row += 2;
	move(row, col);
	addstr("If you lose all coin, game over. If you want to play the game, press enter.");

	refresh();

	while(1)
	{
		c = getch();
		if (c == '\n')
			break;
	}
}

void clear_screen()
{
	int row, col;

	row = TOP_ROW + 1;

	for (row; row < BOT_ROW; row++)
	{
		col = LEFT_EDGE + 1;
		for (col; col < RIGHT_EDGE; col++)
		{
			move(row, col);
			addstr(BLANK);
		}
	}
	
	refresh();
}

void start_game()
{
	int row, col;

	col = 66;
	for (row = TOP_ROW; row <= 23; row++)
	{
		move(row, col);
		addstr("|");
	}

	for(col = 4; col < 56; col += 10)
	{
		make_hcard(col);
	}
	for(col = 68; col < 126; col += 10)
	{
		make_hcard(col);
	}
	make_vcard();

	row = 24;
	for (col = LEFT_EDGE + 1; col < RIGHT_EDGE; col++)
	{
		move(row, col);
		addstr("-");
	}
	
	row = TOP_ROW + 1;
	col = 31;
	move(row, col);
	addstr("Player");
	col = 95;
	move(row, col);
	addstr("Dealer");

	row = 26;
	col = LEFT_EDGE + 2;
	move(row, col);
	addstr("1. Hit(add a card)");
	row += 1;
	move(row, col);
	addstr("2. Stay");
	row += 1;
	move(row, col);
	addstr("3. Surrender");
	row += 1;
	move(row, col);
	addstr("Press corresponding number you want to act. Press Q to quit the game");

	refresh();
}

void make_hcard(int col)
{
	int row;
	int i = col + 9;
	for (col; col < i; col++)
	{
		row = 3;
		move(row, col);
		addstr("-");
		row = 10;
		move(row, col);
		addstr("-");
		row = 13;
		move(row, col);
		addstr("-");
		row = 20;
		move(row, col);
		addstr("-");
	}
	refresh();
}

void make_vcard()
{
	int row, col1, col2, col3, col4;
	for (row = 3; row <= 10; row ++)
	{
		for (col1 = 4; col1 < 56; col1 += 10)
		{
			move(row, col1);
			addstr("|");
		}
		for (col2 = 13; col2 < 66; col2 += 10)
		{
			move(row, col2);
			addstr("|");
		}
		for (col3 = 68; col3 < 126; col3 += 10)
		{
			move(row, col3);
			addstr("|");
		}
		for(col4 = 77; col4 < 134; col4 += 10)
		{
			move(row, col4);
			addstr("|");
		}
	}

	for (row = 13; row <= 20; row++)
	{
		for (col1 = 4; col1 < 56; col1 += 10)
		{
			move(row, col1);
			addstr("|");
		}
		for (col2 = 13; col2 < 66; col2 += 10)
		{
			move(row, col2);
			addstr("|");
		}
		for (col3 = 68; col3 < 126; col3 += 10)
		{
			move(row, col3);
			addstr("|");
		}
		for(col4 = 77; col4 < 134; col4 += 10)
		{
			move(row, col4);
			addstr("|");
		}
	}
}

void game_process(struct card card_set[52])
{
	int* c_order = shuffle_deck();
	int row, col;
	initialize_game(card_set, c_order);
	int c;
	int win = 0;
	int draw = -1;
	int bp = 0;

	while(1)
	{
		while(1)
		{
			c = getch();
			if (c == 'Q')
			{
				bp = 1;
				break;
			}
			if (c == '1')
			{
				if (d_sum < 17)
				{
					card_to_p(card_set, c_order, 0);
					card_to_p(card_set, c_order, 1);
				}
				else
				{
					card_to_p(card_set, c_order, 0);
				}
				show_sum();
				show_ncard();
				win = check_win(0);
				draw = check_draw(0);
			}
			if (c == '2')
			{
				if (d_sum < 17)
				{
					card_to_p(card_set, c_order, 1);
					show_sum();
					show_ncard();
					win = check_win(1);
				}
				else
				{
					show_sum();
					show_ncard();
					win = check_win(2);
					draw = check_draw(2);
				}
			}
			if (c == '3')
			{
				coin += 5;
				win = -1;
				draw = -1;
				break;
			}

			if(win != 0 || draw != -1)
			{
				if (win == 1)
				{
					coin += 20;
					break;
				}
				else if (win == -1)
				{
					break;
				}
				if (draw == 1)
				{
					coin += 10;
					break;
				}
			}
		}
		if (bp == 1)
			break;
		if (n_cardnum < 15)
		{
			c_order = shuffle_deck(card_set);
			n_cardnum = 52;
			u_cardnum = 0;
		}
	
		if (coin < 10)
		{
			clear_screen();
			row = 15;
			col = 45;
			move(row, col);
			addstr("Game Over! After 5seconds game will turn off.");
			refresh();
			sleep(5);
			break;
		}

		while(1)
		{
			row = 31;
			col = 40;
			if (win == 1)
			{
				move(row, col);
				addstr("You win! If you want to continue press enter");
				int d = getch();
				if (d == '\n')
					break;
				if (d == 'Q')
				{
					bp = 1;
					break;
				}
			}
			else if (win == -1)
			{
				move(row, col);
				addstr("You lose! If you want to continue press enter");
				int d = getch();
				if (d == '\n')
					break;
				if (d == 'Q')
				{
					bp = 1;
					break;
				}
			}
			else if (draw == 1)
			{
				move(row, col);
				addstr("You draw. If you want to continue press enter");
				int d = getch();
				if (d == '\n')
					break;
				if (d == 'Q')
				{
					bp = 1;
					break;
				}
			}

		}
		if (bp == 1)
			break;
			
		clear_screen();
		start_game();
		initialize_game(card_set, c_order);
		
		win = 0;
		draw = -1;
	}

	free(c_order);
}

void show_coin()
{
	int row, col;
	char t_coin[5];
	snprintf(t_coin, sizeof(t_coin), "%d", coin);
	row = 34;
	col = LEFT_EDGE + 2;
	move(row, col);
	addstr("             ");
	move(row, col);
	addstr("Your coin: ");
	addstr(t_coin);
}

void initialize_game(struct card card_set[52], int* c_order)
{
	p_setnum = 0, d_setnum = 0, p_sum = 0, d_sum = 0;
	for (int i = 0; i < 2; i++)
	{
		card_to_p(card_set, c_order, 0);
		card_to_p(card_set, c_order, 1);
	}

	coin -= 10;
	show_coin();
	
	show_ncard();
	show_sum();
}

void card_to_p(struct card card_set[52], int* c_order, int who)
{
	int row, col;
	if (who == 0)
	{
		p_deck[p_setnum] = card_set[c_order[u_cardnum++]];
		n_cardnum--;
		p_sum += p_deck[p_setnum].score;
		if (p_setnum <= 5)
		{
			row = 6, col = 6 + p_setnum * 10;
			move(row, col);
			addstr(p_deck[p_setnum].shape);
			row += 1;
			col += 2;
			move(row, col);
			addstr(p_deck[p_setnum].num);
			p_setnum++;
			refresh();
		}
		else
		{
			row = 16, col = 6 + (p_setnum - 6) * 10;
			move(row, col);
			addstr(p_deck[p_setnum].shape);
			row += 1;
			col += 2;
			move(row, col);
			addstr(p_deck[p_setnum].num);
			p_setnum++;
			refresh();

		}
	}
	else if (who == 1)
	{
		d_deck[d_setnum] = card_set[c_order[u_cardnum++]];
		n_cardnum--;
		d_sum += d_deck[d_setnum].score;
		if (d_setnum <= 5)
		{
			row = 6, col = 70 + d_setnum * 10;
			move(row, col);
			addstr(d_deck[d_setnum].shape);
			row += 1;
			col += 2;
			move(row, col);
			addstr(d_deck[d_setnum].num);
			d_setnum++;
			refresh();
		}
		else
		{
			row = 16, col = 70 + (d_setnum - 6) * 10;
			move(row, col);
			addstr(d_deck[d_setnum].shape);
			row += 1;
			col += 2;
			move(row, col);
			addstr(d_deck[d_setnum].num);
			d_setnum++;
			refresh();
		}
	}
}

void show_sum()
{
	int row, col;
	char td_sum[5], tp_sum[5];
	snprintf(td_sum, sizeof(td_sum), "%d", d_sum);
	snprintf(tp_sum, sizeof(tp_sum), "%d", p_sum);
	row = 23;
	col = 28;
	move(row, col);
	addstr("             ");
	move(row, col);
	addstr("Player sum: ");
	addstr(tp_sum);
	col = 92;
	move(row, col);
	addstr("             ");
	move(row, col);
	addstr("Dealer sum: ");
	addstr(td_sum);
	refresh();
}

void show_ncard()
{
	int row, col;
	char tn_num[5], tu_num[5];
	snprintf(tn_num, sizeof(tn_num), "%d", n_cardnum);
	snprintf(tu_num, sizeof(tu_num), "%d", u_cardnum);
	row = 33;
	col = 95;
	move(row, col);
	addstr("                   ");
	move(row, col);
	addstr("Left card nums: ");
	addstr(tn_num);
	row += 1;
	move(row, col);
	addstr("                   ");
	move(row, col);
	addstr("Used card nums: ");
	addstr(tu_num);
	refresh();
}

int check_win(int mode)
{
	if(mode == 0)
	{
		if (p_sum == 21 && d_sum == 21)
			return 0;
		else if (p_sum == 21)
			return 1;
		else if (d_sum == 21)
			return -1;
		if (p_sum > 21 && d_sum > 21)
			return 0;
		else if (p_sum > 21)
			return -1;
		else if (d_sum > 21)
			return 1;
	}
	else if(mode == 1)
	{
		if (d_sum == 21)
			return -1;
		else if (d_sum > 21)
			return 1;
	}
	else if(mode == 2)
	{
		if (d_sum > p_sum)
			return -1;
		else if (d_sum < p_sum)
			return 1;
		else if (d_sum == p_sum)
			return 0;
	}

	return 0;
}

int check_draw(int mode)
{
	if(mode == 0)
	{
		if(p_sum == 21 && d_sum == 21)
			return 1;
		if(p_sum > 21 && d_sum > 21)
			return 1;
	}
	else if(mode == 2)
	{
		if(d_sum == p_sum)
			return 1;
	}
	
	return -1;
}

int* shuffle_deck()
{
	int size = 52;
	int* c_order = (int *)malloc(size * sizeof(int));
	for (int i = 0; i < size; i++)
	{
		c_order[i] = i;
	}
	srand(time(NULL));

	for (int i = size - 1; i > 0; i--)
	{
		int j = rand() % (i + 1);

		swap(&c_order[i], &c_order[j]);
	}

	return c_order;
}

void swap(int* a, int* b)
{
	int temp = *a;
	*a = *b;
	*b = temp;
}

