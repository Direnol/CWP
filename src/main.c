#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <dirent.h>
#include <unistd.h> 
#include "hash.h"

int main(int argc, char **argv)
{
    if (argc < 2) {
        printf ("Please specify the file names!\n");
        return EXIT_FAILURE;
    }
    
    int key;
    //opterr = 0; // запретить вывод ошибок от getopt()
   
    /* s-Save integrity info; c-check integrity info; 
     * */
    while ((key = getopt(argc, argv, "scrf:")) != -1) {
        switch (key) {
            default:
                fprint(stderr, "Basic usage: integrctrl –s –f data path\n
                                integrctrl -s -r -f data path\n
                                integctrl -c -f data path\n
                                integctrl -c -r -f data path\n"
                break;
        }
    }
    
    return 0;
}
