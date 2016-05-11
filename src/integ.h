#ifndef INTEG_H
#define INTEG_H

#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "hash.h"
#include "stat.h"
#include <stdio.h>
#include <stdlib.h>

#define RESET   "\033[0m"
#define RED     "\033[1;31m"
#define YELLOW  "\033[1;33m"
#define WHITE   "\033[1;37m"
#define GREEN	"\033[1;32m"

typedef struct _data {
	char name[261];
	char type[5];
	char parent_dir[4069];
	char hash[64];
	
}DATA;

FILE *f;

DATA *info;
int count;

int save_dir_list(char *dir, char *parent, int mode);
int check_dir_list(char *path, char *parent, int mode);
int check_path(char *path);
int save_info(char *file, char **output);
void found_info(DATA curent);

#endif
