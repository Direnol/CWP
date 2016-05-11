#include "integ.h"

int main(int argc, char **argv)
{	
    if ((argc < 5) || (argc > 6)) {
        fprintf(stderr, "%sPlease specify the file names!\n", RED);
        fprintf(stderr, "Basic usage: integrctrl –s –f data path\n");
        fprintf(stderr, "Basic usage: integrctrl –s -r –f data path\n");
        fprintf(stderr, "Basic usage: integrctrl –c –f data path\n%s", RESET);
        return EXIT_FAILURE;
    }
    
    int key, mode = 0, recursive = 0, file_count = 0;
    int i;
    char *data = NULL;
    char *path = NULL;

    // opterr = 0; // запретить вывод ошибок от getopt()

    // s-Save integrity info; c-check integrity info
    while ((key = getopt(argc, argv, "scrf:")) != -1) {
        switch (key) {
            default: {
                fprintf(stderr, "%sBasic usage: integrctrl –s –f data path\n", RED);
                fprintf(stderr, "Basic usage: integrctrl –s -r –f data path\n");
                fprintf(stderr, "Basic usage: integrctrl –c –f data path\n%s", RESET);
                return 4;
                break;
			}
            case 's': {
				if (mode != 0) {
					fprintf(stderr, "%sYou're using keys -s and -c together or more than one\n%s", RED, RESET);
					return 5;
				}
				mode++;
				break;
			}
			case 'c': {
				if (mode != 0) {
					fprintf(stderr, "%sYou're using keys -s and -c together or more than one\n%s", RED, RESET);
					return 5;
				}
				mode += 2;
				break;
			}
			case 'r': {
				if (recursive != 0) {
					fprintf(stderr, "%sYou use -r more than one time\n%s", RED, RESET);
					return 5;
				}
				recursive++;
				break;
			}
			case 'f': {
				if (file_count != 0) {
					fprintf(stderr, "%sYou use -f more than one time\n%s", RED, RESET);
					return 5;
				}
				if ((strcmp(optarg, "-f") == 0) || (strcmp(argv[optind], "-f") == 0)) {
					fprintf(stderr, "%sYou use -f more than one time\n%s", RED, RESET);
					return 5;
				}
				if ((strcmp(optarg, "-r") == 0) || (strcmp(argv[optind], "-r") == 0)) {
					fprintf(stderr, "%sValue is key\n%s", RED, RESET);
					return 6;
				}
				if ((strcmp(optarg, "-s") == 0) || (strcmp(argv[optind], "-s") == 0)) {
					fprintf(stderr, "%sValue is key\n%s", RED, RESET);
					return 6;
				}
				if ((strcmp(optarg, "-c") == 0) || (strcmp(argv[optind], "-c") == 0)) {
					fprintf(stderr, "%sValue is key\n%s", RED, RESET);
					return 6;
				}
				if (!argv[optind]) {
					fprintf(stderr, "%sCould not find the path directory\n%s", RED, RESET);
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
		fprintf(stderr, "%sYou must use full path%s\n", RED, RESET);
		return 13;
	}
    
    //~ DATA origin_dir;
    //~ strcpy(origin_dir.name, path);
    if (mode == 1) { // Save information
		f = fopen(data, "wb");
		fwrite(&recursive, sizeof(int), 1, f);
		//~ fwrite(&origin_dir, sizeof(DATA), 1, f);
		
        if (save_dir_list(path, path, recursive) == 69){
            // printf("Oops\n");
            return 69;
        }
	} else if (mode == 2) { // Check infromation
		f = fopen(data, "rb");
		if (!f) {
			fprintf(stderr, "%sYour data is not exsist%s\n", RED, RESET);
			return 11;
		}
		
		int check;
		fread(&check, sizeof(int), 1, f);
		if (check != recursive) {
			fprintf(stderr, "%sIncorrect recursive mod\n%s", RED, RESET);
			return 88;
		}
		
		struct stat buff;
		stat(data, &buff);
		int i;
		count = (buff.st_size - sizeof(int)) / sizeof(DATA);
		info = malloc(sizeof(DATA) * count);
		for (i = 0; i < count; i++)  {
			fread(&info[i], sizeof(DATA), 1, f);
			// printf("%s %s %s %s\n", info[i].name, info[i].type, info[i].parent_dir, info[i].hash);
		}
		if(strcmp(info[0].parent_dir, path) != 0) {
			fprintf(stderr, "%s Your origin dir is %s\"%s\"%s\n", WHITE, RED, info[0].parent_dir, RESET);
			return 300;
		}
		
		check_dir_list(path, path, recursive);
	} else {
			fprintf(stderr, "%sIncorrect mode\n%s", RED, RESET);
			return 8;
	}
	
	for (i = 0; i < count; i++) {
		if (strcmp(info[i].name, "|") != 0)
			fprintf(stderr, "%s%s %s %sis DELETED\n%s", WHITE, info[i].name, info[i].type, RED, RESET);
	}

	free(info);
    fclose(f);
    return 0;
}
