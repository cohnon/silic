#include "os.h"

#include <stdio.h>
#include <stdlib.h>

void *os_priv_alloc(size_t size) {
    return malloc(size);
}

bool os_read_file(Span *out, Span path) {
    FILE *file = fopen(path.ptr, "rb");
    if (file == NULL) {
        return false;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        return false;
    }

    long file_len = ftell(file);
    if (file_len == -1) {
        return false;
    }

    char *ptr = malloc(sizeof(char)  *file_len + 1);
    if (ptr == NULL) {
        return false;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        free(ptr);
        return false;
    }

    long file_read = fread(ptr, sizeof(char), file_len, file);
    if (file_read != file_len) {
        free(ptr);
        return false;
    }

    fclose(file);

    *out = (Span){ ptr, file_len };

    return true;
}

