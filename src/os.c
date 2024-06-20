#include "os.h"

#include <stdio.h>
#include <stdlib.h>

void *os_priv_alloc(size_t size) {
    return malloc(size);
}

OsReadFileResult os_read_file(Span path) {
    OsReadFileResult result;
    result.ok = false;

    FILE *file = fopen(path.ptr, "rb");
    if (file == NULL) {
        return result;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        return result;
    }

    long file_len = ftell(file);
    if (file_len == -1) {
        return result;
    }

    char *ptr = malloc(sizeof(char)  *file_len + 1);
    if (ptr == NULL) {
        return result;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        free(ptr);
        return result;
    }

    long file_read = fread(ptr, sizeof(char), file_len, file);
    if (file_read != file_len) {
        free(ptr);
        return result;
    }

    fclose(file);

    result.ok = true;
    result.src = (Span){ ptr, file_len };

    return result;
}

