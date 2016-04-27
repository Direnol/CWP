#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <dirent.h>
#include <unistd.h> 
#include "hash.h"
#include "stat.h"

int list_dir(char *dir, FILE *f);

FILE *file = NULL;

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
   
    /* s-Save integrity info; c-check integrity info; 
     * */
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
				mode = mode + 2;
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
    if (mode == 1) {
		// printf("mode %s\n", "Save information");
	} else if (mode == 2) {
		// printf("mode %s\n", "Check infromation");
	} else {
		fprintf(stderr, "Incorrect mode\n");
		return 8;
	}
    
    if (list_dir(path, file) == 69)
		return 69;
    
    return 0;
}

int list_dir(char *path, FILE *f)
{
	DIR *dir; // Директория
    struct dirent *entry;// Элемент директории
    char new_path[PATH_MAX];//Так, конечно, пишут только говнори
	dir = opendir(path);
    if (!dir) {
		fprintf(stderr, "Incorrect path of directory\n");
		return 69;
	}
	
	while ((entry = readdir(dir)) != NULL) {
		printf("%s\n", entry->d_name);
		
		if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0) &&
			(strcmp(entry->d_name, "..")) != 0) {
			snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
			printf("This is directory %s\n", new_path);
			list_dir(new_path, f);
		}
		
	}
	
	closedir(dir);
}
