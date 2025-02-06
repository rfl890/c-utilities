#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
#endif

const size_t BUFFER_SIZE = 1024 * 1024;

int main(int argc, const char **argv) {
    if (argc < 2) {
        printf("usage: %s [padding amount]", argv[0]);
        exit(EXIT_FAILURE);
    }

    setvbuf(stdout, NULL, _IONBF, 0);

    #ifdef _WIN32
    _setmode(_fileno(stdin), _O_BINARY);
    _setmode(_fileno(stdout), _O_BINARY);
    #endif

    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        exit(EXIT_FAILURE);
    }

    size_t total_bytes_read = 0;
    
    unsigned long long padding_amount = strtoull(argv[1], NULL, 10);
    if (padding_amount > SIZE_MAX || padding_amount < 1) {
        exit(EXIT_FAILURE);
    }

    while (1) {
        size_t bytes_read = fread(buffer, 1, BUFFER_SIZE, stdin);
        total_bytes_read += bytes_read;

        if (bytes_read < BUFFER_SIZE && ferror(stdin)) {
            exit(EXIT_FAILURE);
        }


        if (fwrite(buffer, 1, bytes_read, stdout) < bytes_read) {
            exit(EXIT_FAILURE);
        }

        if (bytes_read < BUFFER_SIZE) {
            size_t remainder = total_bytes_read % padding_amount;
            if (remainder != 0) {
                size_t padding_bytes_count = padding_amount - (total_bytes_read % padding_amount);
                char *padding_bytes = malloc(padding_bytes_count);
                if (padding_bytes == NULL) {
                    exit(EXIT_FAILURE);
                }
                memset(padding_bytes, 0, padding_bytes_count);
                if (fwrite(padding_bytes, 1, padding_bytes_count, stdout) < padding_bytes_count) {
                    exit(EXIT_FAILURE);
                }
            }

            exit(EXIT_SUCCESS);
        }
    }
}
