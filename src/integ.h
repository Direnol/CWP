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

enum ERRORS {
    ER_MODE = 4,
    ER_S_MODE,
    ER_C_MODE,
    ER_R_MODE,
    ER_F_MODE,
    ER_SAVE,
    ER_CHECK,
    ER_PATH,
    ER_DATA
};

enum MODES {
    REC_OFF,
    REC_ON,
    NO_MODE,
    SAVE_MODE,
    CHECK_MODE,
    FILE_MODE
};

typedef struct _data {
    char name[261];
    char type[5];
    char parent_dir[4069];
    char hash[64];

} DATA;

FILE *f;

DATA *info;
int count, gl_i;

int save_dir_list(char *dir, char *parent, int mode);
int check_dir_list(char *path, char *parent, int mode);
int check_path(char *path);
int save_info(char *file, char **output);
void found_info(DATA curent);

#endif
