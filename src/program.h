#ifndef PROGRAM_H
#define PROGRAM_H

#include "list.h"

typedef struct Program {
    char *root_mod_path;
    List mods;
} Program;

Program program_init(char *root_mod_path);
void program_compile(Program *prg);

#endif
