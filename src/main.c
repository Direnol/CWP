#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "hash.h"
#include "stat.h"

typedef struct _data {
	char name[261];
	char type[5];
	char parent_dir[PATH_MAX];
	char hash[64];
	
}DATA;

DATA *info;
int count;

int save_dir_list(char *dir, char *parent, int mode);
int check_dir_list(char *path, char *parent, int mode);
int check_path(char *path);
int save_info(char *file, char **output);
void found_info(DATA curent);


FILE *f = NULL;

int main(int argc, char **argv)
{
    if ((argc < 5) || (argc > 6)) {
        fprintf(stderr, "Please specify the file names!\n");
        fprintf(stderr, "Basic usage: integrctrl –s –f data path\n");
        fprintf(stderr, "Basic usage: integrctrl –s -r –f data path\n");
        fprintf(stderr, "Basic usage: integrctrl –c –f data path\n");
        return EXIT_FAILURE;
    }

    int key, mode = 0, recursive = 0, file_count = 0;
    char *data = NULL;
    char *path = NULL;

    // opterr = 0; // запретить вывод ошибок от getopt()

    // s-Save integrity info; c-check integrity info
    while ((key = getopt(argc, argv, "scrf:")) != -1) {
        switch (key) {
            default: {
                fprintf(stderr, "Basic usage: integrctrl –s –f data path\n");
                fprintf(stderr, "Basic usage: integrctrl –s -r –f data path\n");
                fprintf(stderr, "Basic usage: integrctrl –c –f data path\n");
                return 4;
                break;
			}
            case 's': {
				if (mode != 0) {
					fprintf(stderr, "You're using keys -s and -c together or more than one\n");
					return 5;
				}
				mode++;
				break;
			}
			case 'c': {
				if (mode != 0) {
					fprintf(stderr, "You're using keys -s and -c together or more than one\n");
					return 5;
				}
				mode += 2;
				break;
			}
			case 'r': {
				if (recursive != 0) {
					fprintf(stderr, "You use -r more than one time\n");
					return 5;
				}
				recursive++;
				break;
			}
			case 'f': {
				if (file_count != 0) {
					fprintf(stderr, "You use -f more than one time\n");
					return 5;
				}
				if ((strcmp(optarg, "-f") == 0) || (strcmp(argv[optind], "-f") == 0)) {
					fprintf(stderr, "You use -f more than one time\n");
					return 5;
				}
				if ((strcmp(optarg, "-r") == 0) || (strcmp(argv[optind], "-r") == 0)) {
					fprintf(stderr, "Value is key\n");
					return 6;
				}
				if ((strcmp(optarg, "-s") == 0) || (strcmp(argv[optind], "-s") == 0)) {
					fprintf(stderr, "Value is key\n");
					return 6;
				}
				if ((strcmp(optarg, "-c") == 0) || (strcmp(argv[optind], "-c") == 0)) {
					fprintf(stderr, "Value is key\n");
					return 6;
				}
				if (!argv[optind]) {
					fprintf(stderr, "Could not find the path directory\n");
					return 6;
				}
				data = optarg;
				path = argv[optind];
				break;
			}
        }
    }

    // printf("%s\n%s\n", data, path);
    if (check_path(path)) {
		fprintf(stderr, "You must use full path\n");
		return 13;
	}
    
    if (mode == 1) { // Save information
		f = fopen(data, "wb");
		fwrite(&recursive, sizeof(int), 1, f);
		
        if (save_dir_list(path, path, recursive) == 69){
            printf("Oops\n");
            return 69;
        }
	} else if (mode == 2) { // Check infromation
		f = fopen(data, "rb");
		if (!f) {
			fprintf(stderr, "Your data is not exsist\n");
			return 11;
		}
		int check;
		fread(&check, sizeof(int), 1, f);
		if (check != recursive) {
			fprintf(stderr, "Incorrect recursive mod\n");
			return 88;
		}
		struct stat buff;
		stat(data, &buff);
		int i;
		count = buff.st_size / sizeof(DATA);
		info = malloc(sizeof(DATA) * count);
		for (i = 0; i < count; i++)  {
			fread(&info[i], sizeof(DATA), 1, f);
			// printf("%s %s %s %s\n", info[i].name, info[i].type, info[i].parent_dir, info[i].hash);
	}
		check_dir_list(path, path, recursive);
	} else {
		fprintf(stderr, "Incorrect mode\n");
		return 8;
	}

    fclose(f);
    return 0;
}

int save_dir_list(char *path, char *parent, int mode)
{
	DATA file, d;
	DIR *dir; // Директория
    struct dirent *entry;// Элемент директории
    char new_path[PATH_MAX];
	dir = opendir(path);
    if (!dir) {
		fprintf(stderr, "Incorrect path of directory\n%s\n", path);
		return 69;
	}

	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0) && // If this's directory
			(strcmp(entry->d_name, "..")) != 0) {

			int n = strlen(path) - 1;
			while (path[n] == '/') { // Delete slashes
				path[n] = '\0';
				n--;
			}
			
			strcpy(d.name, entry->d_name);
			strcpy(d.type, "dir");
			strcpy(d.parent_dir, parent);
			strcpy(d.hash, "");
			snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name); // Create the path for the directory

			fwrite(&d, sizeof(d), 1, f);
			
			if (mode == 1)
				save_dir_list(new_path, new_path, mode); // Read new the path
		} else if ((strcmp(entry->d_name, ".") != 0) && // Do something if it is not dot or directory
				   (strcmp(entry->d_name, "..")) != 0) {
			char *hash = NULL;
			snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
			save_info(new_path, &hash);
			
			strcpy(file.name, entry->d_name);
			strcpy(file.type, "file");
			strcpy(file.parent_dir, parent);
			strcpy(file.hash, hash);
			
			fwrite(&file, sizeof(file), 1, f);
		}
	}

	closedir(dir);
}

int check_dir_list(char *path, char *parent, int mode)
{
	
	DATA curent;
	char new_path[PATH_MAX];
	int i;
	DIR *dir; // Директория
    struct dirent *entry;// Элемент директории
    dir = opendir(path);
    if (!dir) {
		fprintf(stderr, "(c)Incorrect path of directory\n%s\n", path);
		return 69;
	}
	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0) // If this's a directory
			&& (strcmp(entry->d_name, "..")) != 0) {

			strcpy(curent.name, entry->d_name);
			strcpy(curent.type, "dir");
			strcpy(curent.parent_dir, parent);
			strcpy(curent.hash, "");
			
			snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name); // Create the path for the directory
			
			found_info(curent);
			
			if (mode == 1)
				check_dir_list(new_path, new_path, mode);
		} else if ((strcmp(entry->d_name, ".") != 0) && // Do something if it is not dot or directory
				   (strcmp(entry->d_name, "..")) != 0) {
			char *hash = NULL;
			
			snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
			save_info(new_path, &hash);
			
			strcpy(curent.name, entry->d_name);
			strcpy(curent.type, "file");
			strcpy(curent.parent_dir, parent);
			strcpy(curent.hash, hash);
			
			found_info(curent);
			
		}
	}

	for (i = 0; i < count; i++) {
		if (strcmp(info[i].name, "|") != 0)
			fprintf(stderr, "%s %s is DELETED\n", info[i].name, info[i].type);
	}
	
	closedir(dir);
}

int save_info(char *file, char **output)
{
    FILE *f = fopen(file, "r");
    if (!f) {
		fprintf(stderr, "can't open file %s\n", file);
		return 99;
	}
    struct stat buff;
    stat(file, &buff);
    char *tmp = malloc(sizeof(char) * buff.st_size);
    fscanf(f, "%s", tmp);
    hash(tmp, &(*output));
    fclose(f);

}

int check_path(char *path)
{
	int n = strlen(path) - 1;
	while (path[n] == '/') { // Delete slashes
		path[n] = '\0';
		n--;
	}
			
	if (path[0] == '/')
		return 0;
	else
		return 1;
}


void found_info(DATA curent)
{
		int i;
		char *DELETED = "|";
		for (i = 0 ; i < count; i++) {
			//~ printf("I %d\n", i);
			//~ printf("%s %s %s\n", curent.name, curent.type, curent.parent_dir);
			//~ printf("%s %s %s\n", info[i].name, info[i].type, info[i].parent_dir);
			//~ 
			if ((strcmp(info[i].name, curent.name) == 0)
			 && (strcmp(info[i].type, curent.type) == 0)
			 && (strcmp(info[i].parent_dir, curent.parent_dir) == 0)) {
				if (strcmp(curent.type, "file") == 0) {
					if (strcmp(info[i].hash, curent.hash) != 0) {
						fprintf(stderr, "%s is changed\n", curent.name);
						strcpy(info[i].name, DELETED);
						return;
					}
				}
				strcpy(info[i].name, DELETED);
				return;
			}
		}
		if (strcmp(curent.type, "dir") == 0) {
				fprintf(stderr, "NEW DIRECTORY: %s %s\n", curent.name, curent.parent_dir);
				return;
		} else if (strcmp(curent.type, "file") == 0) {
				fprintf(stderr, "NEW FILE: %s %s\n", curent.name, curent.parent_dir);
				return;
		}
}
