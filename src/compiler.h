#ifndef COMPILER_H
#define COMPILER_H

#include "module.h"
#include "error_msg.h"
#include "namespace.h"
#include <fir.h>


typedef struct Compiler {

    // lookup paths for imports
    FirString main_dir_path;
    FirString std_dir_path;
    FirString lib_dir_path;

    // all the modules in this compilation
    // keeps track of the current module being processed
    DynArr(Module*) modules;
    Module         *cur_module;

    DynArr(ErrorMsg) errors;

    FirModule *fir;
} Compiler;

Compiler *compiler_init(void);
bool compiler_compile(Compiler *compiler, FirString file_path);

#endif
