#include "integ.h"

int save_dir_list(char *path, char *parent, int mode)
{
    DATA file, d;
    DIR *dir;                   // Директория
    struct dirent *entry;       // Элемент директории
    char new_path[PATH_MAX];
    dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "%sIncorrect path of directory\n%s%s\n%s", RED, WHITE,
                path, RESET);
        return 69;
    }

    while ((entry = readdir(dir)) != NULL) {
        if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0) &&        // If this's directory
            (strcmp(entry->d_name, "..")) != 0) {

            int n = strlen(path) - 1;
            while (path[n] == '/') {    // Delete slashes
                path[n] = '\0';
                n--;
            }

            strcpy(d.name, entry->d_name);
            strcpy(d.type, "dir");
            strcpy(d.parent_dir, parent);
            strcpy(d.hash, "");
            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name); // Create the path for the directory

            fwrite(&d, sizeof(DATA), 1, f);

            if (mode == 1)
                if (save_dir_list(new_path, new_path, mode) == 69)
                    return 69;  // Read new the path
        } else if ((strcmp(entry->d_name, ".") != 0) && // Do something if it is not dot or directory
                   (strcmp(entry->d_name, "..")) != 0) {
            char *hash = NULL;
            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
            if (save_info(new_path, &hash) == 99)
                return 69;;

            strcpy(file.name, entry->d_name);
            strcpy(file.type, "file");
            strcpy(file.parent_dir, parent);
            strcpy(file.hash, hash);

            fwrite(&file, sizeof(DATA), 1, f);
        }
    }

    closedir(dir);
    return 0;
}

int check_dir_list(char *path, char *parent, int mode)
{
    DATA curent;
    char new_path[PATH_MAX];
    int i;
    DIR *dir;                   // Директория
    struct dirent *entry;       // Элемент директории
    dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "%s(c)Incorrect path of directory\n%s%s\n%s", RED,
                WHITE, path, RESET);
        return 69;
    }
    while ((entry = readdir(dir)) != NULL) {
        if ((entry->d_type == 4) && (strcmp(entry->d_name, ".") != 0)   // If this's a directory
            && (strcmp(entry->d_name, "..")) != 0) {

            strcpy(curent.name, entry->d_name);
            strcpy(curent.type, "dir");
            strcpy(curent.parent_dir, parent);
            strcpy(curent.hash, "");

            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name); // Create the path for the directory
            found_info(curent);

            if (mode == 1)
                if (check_dir_list(new_path, new_path, mode) == 69)
                    return 69;;
        } else if ((strcmp(entry->d_name, ".") != 0) && // Do something if it is not dot or directory
                   (strcmp(entry->d_name, "..")) != 0) {
            char *hash = NULL;

            snprintf(new_path, PATH_MAX, "%s/%s", path, entry->d_name);
            if (save_info(new_path, &hash) == 99)
                return 69;;

            strcpy(curent.name, entry->d_name);
            strcpy(curent.type, "file");
            strcpy(curent.parent_dir, parent);
            strcpy(curent.hash, hash);

            found_info(curent);
        }
    }

    closedir(dir);
    return 0;
}

int save_info(char *file, char **output)
{
    FILE *f = fopen(file, "r");
    if (!f) {
        fprintf(stderr, "%scan't open file%s %s\n%s", RED, WHITE, file, RESET);
        return 99;
    }
    struct stat buff;
    stat(file, &buff);
    char *tmp = malloc(sizeof(char) * buff.st_size);
    fscanf(f, "%s", tmp);
    hash(tmp, &(*output));

    // free(tmp);
    fclose(f);
}

int check_path(char *path)
{
    int n = strlen(path) - 1;
    if (n != 0) {               // length of path isn't equal 1
        while (path[n] == '/') {        // Delete slashes
            path[n] = '\0';
            n--;
        }

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
        fprintf(stderr, "%sNEW FILE: %s%s%s[%s] %s\n", RED, WHITE, curent.name,
                GREEN, curent.parent_dir, RESET);
        return;
    }
}
