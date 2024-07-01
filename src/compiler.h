#ifndef COMPILER_H
#define COMPILER_H

#include "module.h"
#include "namespace.h"
#include <fir.h>


typedef struct Compiler {
    DynArr(Module*) modules;
    Namespace ns;

    FirString main_dir_path;
    FirString std_dir_path;
    FirString lib_dir_path;

    FirModule *fir;
} Compiler;

Compiler *compiler_init(void);
bool compiler_compile(Compiler *compiler, FirString file_path);

#endif
