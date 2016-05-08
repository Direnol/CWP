#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include "hash.h"
#include "stat.h"


int save_dir_list(char *dir, char *parent, int mode);
int check_dir_list(char *dat);
int check_path(char *path);
int save_info(char *file, char **output);


FILE *f = NULL;

typedef struct _data {
	char name[261];
	char type[5];
	char parent_dir[4096];
	char hash[64];
	
}DATA;

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
			}
			break;
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
		DATA parent;
		strcpy(parent.name, path);
		strcpy(parent.type, "dir");
		strcpy(parent.parent_dir, "");
		strcpy(parent.hash, "");
		// fprintf(f, "%s %s %s %s\n", parent.name, parent.type, parent.parent_dir, parent.hash);
		fwrite(&parent, sizeof(parent), 1, f);
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
		printf("mode %s\n", "Check infromation");
		check_dir_list(data);
	} else {
		fprintf(stderr, "Incorrect mode\n");
		return 8;
	}

    fclose(f);
    return 0;
}

int save_dir_list(char *path, char *parent, int mode)
{
	int n;
	DATA file, d;
	DIR *dir; // Директория
    struct dirent *entry;// Элемент директории
    char new_path[PATH_MAX];
	dir = opendir(path);
    if (!dir) {
		fprintf(stderr, "Incorrect path of directory\n");
		return 69;
	}

	while ((entry = readdir(dir)) != NULL) {
		if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0) && // If this's directory
			(strcmp(entry->d_name, "..")) != 0) {

			n = strlen(path) - 1;
			while (path[n] == '/') { // Delete slashes
				path[n] = '\0';
				n--;
			}
			strcpy(d.name, entry->d_name);
			strcpy(d.type, "dir");
			strcpy(d.parent_dir, parent);
			strcpy(d.hash, "");
			snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name); // Create the path for the directory
			// printf("Directory %d %s\n", parent, new_path);
			// fprintf(f, "%s %s %s %s\n", d.name, d.type, d.parent_dir, d.hash);
			fwrite(&d, sizeof(d), 1, f);
			if (mode == 1)
				save_dir_list(new_path, new_path, mode); // Read new the path
		} else if ((strcmp(entry->d_name, ".") != 0) && // Do something if it is not dot or directory
				   (strcmp(entry->d_name, "..")) != 0) {
			char *hash = NULL;
			snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
			save_info(new_path, &hash);
			// printf("File %d %s %s\n", parent, entry->d_name, hash);
			strcpy(file.name, entry->d_name);
			strcpy(file.type, "file");
			strcpy(file.parent_dir, parent);
			strcpy(file.hash, hash);
			// fprintf(f, "%s %s %s %s\n", file.name, file.type, file.parent_dir, file.hash);
			fwrite(&file, sizeof(file), 1, f);
		}
	}

	closedir(dir);
}

int check_dir_list(char *dat)
{
	struct stat buff;
	stat(dat, &buff);
	int i;
	int count = buff.st_size / sizeof(DATA);
	DATA info[count];
	
	for (i = 0; i < count; i++)  {
		fread(&info[i], sizeof(info), 1, f);
		printf("%s %s %s %s\n", info[i].name, info[i].type, info[i].parent_dir, info[i].hash);
	}
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
	if (path[0] == '/')
		return 0;
	else
		return 1;
}
