#include "hash.h"

void hash(char *input, char **output)
{
    md5_state_t HASH;
    md5_byte_t digest[16];
    char *hex_output = NULL;
    hex_output = malloc((16*2 + 1) * sizeof(char));
    int di;
    
    md5_init(&HASH);
    md5_append(&HASH, (const md5_byte_t *)input, strlen(input));
    md5_finish(&HASH, digest);
    
    for (di = 0; di < 16; ++di) {
	    sprintf(hex_output + di * 2, "%02x", digest[di]);
    }
    *output = hex_output;
}
