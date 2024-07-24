#ifndef COMPILER_H
#define COMPILER_H

#include "module.h"
#include "error_msg.h"

#include <fir/fir.h>

typedef struct Compiler {

    // lookup paths for imports
    String main_dir_path;
    String std_dir_path;
    String lib_dir_path;

    // all the modules in this compilation
    // keeps track of the current module being processed
    DynArr(Module*) modules;
    Module         *cur_module;

    DynArr(ErrorMsg) errors;

    FirModule *fir;
} Compiler;

Compiler *compiler_init(void);
bool compiler_compile(Compiler *compiler, String file_path);

#endif
