#ifndef OS_H
#define OS_H

#include <fir.h>
#include <stdbool.h>

#define ANSI_RESET "\033[0m"
#define ANSI_RED   "\033[1;31m"
#define ANSI_BLUE  "\033[34m"
#define ANSI_GREY  "\033[90m"

#define os_alloc_T(T) (T*)os_priv_alloc(sizeof(T))
#define os_alloc(T, size) (T*)os_priv_alloc(sizeof(T) * size)
void *os_priv_alloc(size_t size);

typedef struct OsReadFileResult {
    bool   ok;
    FirSym src;
} OsReadFileResult;
OsReadFileResult os_read_file(FirSym path);

#endif
