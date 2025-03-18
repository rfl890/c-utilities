#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <linux/limits.h>

#define BUFFER_SIZE (1024*1024)

int main(void) {
    char rdbuf[BUFFER_SIZE];

    while (1) {
        char *name = NULL;
        size_t alloc = 0;
        ssize_t result = getline(&name, &alloc, stdin);
        if (result == -1) {
            break;
        }
        name[result - 1] = 0;
        
        struct stat info;
        int stat_result = stat(name, &info);
        if (stat_result == -1) {
            goto error;
        }

        if (!S_ISREG(info.st_mode)) {
            goto error;
        }

        FILE *f = fopen(name, "r");
        if (f == NULL) {
            goto error;
        }


        while (1) {
            size_t bytes_read = fread(rdbuf, 1, BUFFER_SIZE, f);
            char sbreak = 0;

            if (bytes_read < BUFFER_SIZE) {
                if (feof(f) && !ferror(f)) {
                    sbreak = 1;
                } else {
                    fclose(f);
                    goto error;
                }
            }

            fwrite(rdbuf, 1, bytes_read, stdout);
            
            if (sbreak) {
                break;
            }
        }
        
        error:
            free(name);
            continue;
    }
}
