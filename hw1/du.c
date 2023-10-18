#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string.h>
#include <limits.h>

char processed_dirs[1000][1024];
int num_processed_dirs = 0;

long long do_du_proto( int mode, char dirname[] );
void do_du( char dirname[], int all, int human_readable );
long long calc_size( char dirname[], int all );
long long calc_size_h( char dirname[], int all );
void change_unit( long size, char dirname[] );
int is_processed( char dirname[] );
void mark_as_processed( char dirname[] );

int main( int ac, char *av[] )
{
	int all = 0;
	int human_readable = 0;
	int opt_arr[1000] = { 0 };

	if( ac == 1 )
	{
		int mode = 0;
		do_du( ".", all, human_readable );
	}
	else
	{
		int opt = 0;
		for (int i = 1; i < ac; i++)
		{
			if( av[i][0] == '-' )
			{
				opt++;
				opt_arr[i] = 1;
				if (strcmp(av[i], "-ah") == 0 || strcmp(av[i], "-ha") == 0)
				{
					all = 1;
					human_readable = 1;
				}
				else
				{

					int len = strlen( av[i] );
					for ( int j = 1; j < len; j++ )
					{
						if (av[i][j] == 'a')
						{
							all = 1;
						}
						else if (av[i][j] == 'h')
						{
							human_readable = 1;
						}
						else
						{
							fprintf(stderr, "Unknown option: %c\n", av[i][j]);
							return EXIT_FAILURE;
						}
					}
				}
			}
		}
		for (int k = 1; k < ac; k++)
		{
			if (ac == (opt + 1))
			{
				do_du(".", all, human_readable);
			}
			if (!opt_arr[k])
			{
				do_du(av[k], all, human_readable);
			}
		}

	}
	return 0;
}

long long do_du_proto( int mode, char dirname[] )
{
	DIR *dir_ptr;
	struct stat fileStat;
	struct dirent *direntp;
	if ( ( dir_ptr = opendir( dirname ) ) == NULL )
		fprintf(stderr, "du: cannot open %s\n", dirname);
	else
	{
		long long size_s = 0;
		if( mode == 0 )
		{
			while ( ( direntp = readdir( dir_ptr ) ) != NULL )
			{
				if (strcmp(direntp->d_name, ".") == 0 || 
				strcmp(direntp->d_name, "..") == 0) 
				{
            				continue;
        			}
				char path[1024];
				snprintf(path, sizeof(path), "%s/%s", 
					dirname, direntp->d_name);
				
				if ( stat(path, &fileStat) < 0 )
				{
					perror("stat");
					continue;
				}
				if (!is_processed(path))
				{
					if( S_ISREG(fileStat.st_mode) )
					{
						size_s +=  (((fileStat.st_size / (1024 * 4)) + 1) * 4);
					}
					else if ( S_ISDIR(fileStat.st_mode) )
					{
						size_s += do_du_proto( mode, path );
					}
				}

			}
			size_s += 4;
			printf("%-8lld%s\n", size_s, dirname);
		}
		closedir(dir_ptr);
		return size_s;
	}
}

void do_du( char dirname[], int all, int human_readable )
{
	char canonical_path[1024];

	if (realpath(dirname, canonical_path) == NULL)
	{
		perror("realpath");
		return;
	}
	if (is_processed(canonical_path))
		return;
	struct stat fileStat;

	if (stat(canonical_path, &fileStat) < 0)
	{
		perror("stat");
		return ;
	}
		if (S_ISREG(fileStat.st_mode))
		{
			long long size = fileStat.st_size;
			if (human_readable)
			{
				if (fileStat.st_size >= 1)
				{
					size = fileStat.st_size / (1024 * 4);
					if (size * (1024 * 4) < fileStat.st_size)
					{
						size = (size + 1) * 4;
					}
					else
					{
						size = size * 4;
					}
					change_unit(size, dirname);
				}
				else
				{
					printf("%-8d%s\n", 0, dirname);	
				}
			}
			else
			{
				if (fileStat.st_size >= 1)
				{
					size = fileStat.st_size / (1024 * 4);
					if (size * (1024 * 4) < fileStat.st_size)
					{
						size = (size + 1) * 4;
					}
					else
					{
						size = size * 4;
					}
					printf("%-8lld%s\n", size, dirname);
				}
				else
				{
					printf("%-8d%s\n", 0, dirname);
				}	
			}
		}
		else if (S_ISDIR(fileStat.st_mode))
		{
			if (human_readable)
			{
				long long size = calc_size_h(dirname, all);
			}
			else
			{
				long long size = calc_size(dirname, all);
			}
		}
		else
		{
			fprintf(stderr, "Unsupported file type: %s\n", dirname);
		}
	mark_as_processed(canonical_path);
}

long long calc_size( char dirname[], int all )
{
	DIR *dir_ptr;
	struct stat fileStat;
	struct dirent *direntp;
	long long size_s = 0;
	if ( ( dir_ptr = opendir( dirname ) ) == NULL )
		fprintf(stderr, "du: cannot open %s\n", dirname);
	else
	{	
		while ( ( direntp = readdir( dir_ptr ) ) != NULL )
		{
			if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) 
			{
            			continue;
        		}
			char path[1024];
			char canonical_path[1024];
			snprintf(path, sizeof(path), "%s/%s", dirname, direntp->d_name);
			if (realpath(path, canonical_path) == NULL)
			{
				perror("realpath");
				return 0;
			}

			if ( stat(path, &fileStat) < 0 )
			{
				perror("stat");
				continue;
			}
			
			if (!is_processed(canonical_path))
			{
				mark_as_processed(canonical_path);
			if( S_ISREG(fileStat.st_mode) )
			{
				if (fileStat.st_size >= 1)
				{
					long size = fileStat.st_size / (1024 * 4);
					if (size * (1024 * 4) < fileStat.st_size)
					{
						size = (size + 1) * 4;
					}
					else
					{
						size = size * 4;
					}
					size_s += size;

					if (all == 1)
					{
						printf("%-8ld%s\n", size, path);
					}
				}
				else
				{
					if (all == 1)
					{
						printf("%-8d%s\n", 0, path);
					}
				}
			}
			else if ( S_ISDIR(fileStat.st_mode) )
			{
				size_s += calc_size(path, all);
			}
			}
		}
		size_s += 4;
		printf("%-8lld%s\n", size_s, dirname);
	}

	closedir(dir_ptr);
	return size_s;
}

long long calc_size_h( char dirname[], int all )
{
	DIR *dir_ptr;
	struct stat fileStat;
	struct dirent *direntp;
	long long size_s = 0;
	if ( ( dir_ptr = opendir( dirname ) ) == NULL )
		fprintf(stderr, "du: cannot open %s\n", dirname);
	else
	{
		while ( ( direntp = readdir( dir_ptr ) ) != NULL )
		{
			if (strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0) 
			{
            			continue;
        		}
			char path[1024];
			char canonical_path[1024];
			snprintf(path, sizeof(path), "%s/%s", dirname, direntp->d_name);
			
			if (realpath(path, canonical_path) == NULL)
			{
				perror("realpath");
				return 0;
			}

			if ( stat(path, &fileStat) < 0 )
			{
				perror("stat");
				continue;
			}

			if (!is_processed(canonical_path))
			{
				mark_as_processed(canonical_path);
			if( S_ISREG(fileStat.st_mode) )
			{
				if (fileStat.st_size >= 1)
				{
					long size = fileStat.st_size / (1024 * 4);
					if (size * (1024 * 4) < fileStat.st_size)
					{
						size = (size + 1) * 4;
					}
					else
					{
						size = size * 4;
					}
	
					size_s += size;
					if (all == 1)
					{
						change_unit(size, path);
					}
				}
				else
				{
					if (all == 1)
					{
						printf("%-8d%s\n", 0, path);
					}
				}

			}
			else if ( S_ISDIR(fileStat.st_mode) )
			{
				size_s += calc_size_h(path, all);
			}
			}
		}
		size_s += 4;
		change_unit(size_s, dirname);
	}

	closedir(dir_ptr);
	return size_s;
}

void change_unit( long size, char dirname[] )
{
	float size_h = 0.0;
	if ( size >= (1024 * 1024) )
	{
		long size_r = 0;
		size_h = (float) size / (float) (1024 * 1024);
		size_r = (long) (10 * size_h);
		float size_s = (float) size_r / (float) 10;
		if (size_h * (1024 * 1024) > size_s)
		{
			size_s += 0.1;
		}
		printf("%.1fG\t%s\n", size_s, dirname);
	}
	else if ( size >= (1024) )
	{
		long size_r = 0;
		size_h = (float) size / (float) 1024;
		size_r = (long) (10 * size_h);
		float size_s = (float) size_r / (float) 10;
		if (size_h * 1024 > size_s)
		{
			size_s += 0.1;
		}
		printf("%.1fM\t%s\n", size_s, dirname);
	}
	else if ( size >= 12 )
	{
		printf("%ldK\t%s\n", size, dirname);
	}
	else if ( size >= 4 && size < 10)
	{
		size_h = (float) size;
		printf("%.1fK\t%s\n", size_h, dirname);
	}
	else
	{
		printf("%-8d%s\n", 0, dirname);
	}
}

int is_processed(char dirname[])
{
	for (int i = 0; i < num_processed_dirs; i++)
	{
		if (strcmp(processed_dirs[i], dirname) == 0)
			return 1;
	}
	return 0;
}

void mark_as_processed(char dirname[])
{
	strcpy(processed_dirs[num_processed_dirs], dirname);
	num_processed_dirs++;
}







