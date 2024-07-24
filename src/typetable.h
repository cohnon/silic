#ifndef TYPETABLE_H
#define TYPETABLE_H

#include <fir/dynarr.h>
#include <stdbool.h>

typedef int TypeId;

typedef enum TypeKind {
    Type_Void,
    Type_Ptr,
    Type_Int,
    Type_Float,
    Type_Func,
} TypeKind;

typedef struct TypePtr {
    bool   mutable;
    TypeId to;
} TypePtr;

typedef struct TypeFunc {
    DynArr(TypeId) params;
    TypeId         ret;
} TypeFunc;

typedef struct Type {
    TypeKind kind;

    union {
        TypePtr  ptr;
        TypeFunc func;
    };
} Type;

typedef struct TypeTable {
    DynArr(Type) types;
} TypeTable;

TypeTable typetable_init(void);

TypeId typetable_create(TypeKind kind);

#endif
