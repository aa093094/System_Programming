#include <sys/types.h>

#define YES 1
#define NO  0

extern int background;
pid_t background_pid;

char	*next_cmd();

char	**splitline(char*);

void 	freelist(char**);

void 	*emalloc(size_t);

void 	*erealloc(void*, size_t);

int	execute(char**, int);

void 	fatal(char*, char*, int);

int	process(char**, int);

void handle_ctrl_c(int);

