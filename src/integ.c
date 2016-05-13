#include "integ.h"

void save_dir_list(char *path, char *parent, int mode)
{
    DATA file, d;
    DIR *dir;                   // Директория
    struct dirent *entry;       // Элемент директории
    char new_path[PATH_MAX];
    
    dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "%sRoot directory %s%s\n%s", RED, WHITE,
                path, RESET);
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0) &&
            (strcmp(entry->d_name, "..")) != 0) {

            strcpy(d.name, entry->d_name);
            strcpy(d.type, "dir");
            strcpy(d.parent_dir, parent);
            strcpy(d.hash, "");
            
            if (path[strlen(path) - 1] == '/') {
				snprintf(new_path, PATH_MAX, "%s%s",
				path, entry->d_name);
			} else
				snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);

            fwrite(&d, sizeof(DATA), 1, f);

            if (mode == REC_ON)
                save_dir_list(new_path, new_path, mode);
        } else if (entry->d_type == 8) {
            char *hash = NULL;
            
            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
            save_info(new_path, &hash);

            strcpy(file.name, entry->d_name);
            strcpy(file.type, "file");
            strcpy(file.parent_dir, parent);
            strcpy(file.hash, hash);

            fwrite(&file, sizeof(DATA), 1, f);
        }
    }

    closedir(dir);
}

void check_dir_list(char *path, char *parent, int mode)
{
    DATA curent;
    char new_path[PATH_MAX];
    DIR *dir;
    struct dirent *entry; 
    
    dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "%s(c)Incorrect path of directory\n%s%s\n%s", RED,
                WHITE, path, RESET);
        return;
    }
    
    while ((entry = readdir(dir)) != NULL) {
        if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0)
            && (strcmp(entry->d_name, "..")) != 0) {

            strcpy(curent.name, entry->d_name);
            strcpy(curent.type, "dir");
            strcpy(curent.parent_dir, parent);
            strcpy(curent.hash, "");

            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
            found_info(curent);

            if (mode == 1)
                check_dir_list(new_path, new_path, mode);
        } else if (entry->d_type == 8) {
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

    closedir(dir);
}

void save_info(char *file, char **output)
{	
    FILE *f = fopen(file, "r");
    if (!f) {
        fprintf(stderr, "%sSysyem file %s %s\n%s", RED, WHITE, file, RESET);
        *output = "system";
        return;
    }
   
    long int i = 0;
    struct stat buff;
    stat(file, &buff);
    data_file = calloc((buff.st_size + 1), sizeof(char));
    
    while ((data_file[i] = fgetc(f)) != EOF  ) {                
        i++;      
    } 
	
	hash(data_file, &(*output));

    free(data_file);
    fclose(f);
}

int check_path(char *path)
{
    int n = strlen(path) - 1;
    if (n != 0) {
        while (path[n] == '/') {
            path[n] = '\0';
            n--;
        }

    }
    
    DIR *dir;
    dir = opendir(path);
    if (!dir) {
        		fprintf(stderr, "%s Incorrect path%s[%s]\n%s", 
								RED, WHITE, path, RESET);
        return EXIT_FAILURE;
    }

    if (path[0] == '/')
        return EXIT_SUCCESS;
    else {
		fprintf(stderr, "%sYou must use full path%s\n", RED, RESET);
        return EXIT_FAILURE;
	}
}

void found_info(DATA curent)
{
    int i;
    char *DELETED = "|";
    for (i = 0; i < count; i++) {
        if ((strcmp(info[i].name, curent.name) == 0)
            && (strcmp(info[i].type, curent.type) == 0)
            && (strcmp(info[i].parent_dir, curent.parent_dir) == 0)) {
            if (strcmp(curent.type, "file") == 0) {
                if (strcmp(info[i].hash, curent.hash) != 0) {
                    fprintf(stderr, "%s%s%s[%s] %sis changed%s\n", WHITE,
                            curent.name, GREEN, curent.parent_dir, RED, RESET);
                    strcpy(info[i].name, DELETED);
                    return;
                }
            }
            strcpy(info[i].name, DELETED);
            return;
        }
    }
    if (strcmp(curent.type, "dir") == 0) {
        fprintf(stderr, "%sNEW DIRECTORY: %s%s%s[%s] %s\n", RED, WHITE,
                curent.name, GREEN, curent.parent_dir, RESET);
        return;
    } else if (strcmp(curent.type, "file") == 0) {
        fprintf(stderr, "%sNEW FILE: %s%s%s[%s] %s\n", YELLOW, WHITE, curent.name,
                GREEN, curent.parent_dir, RESET);
        return;
    }
}
