#ifndef NAMESPACE_H
#define NAMESPACE_H

#include "map.h"


typedef struct Compiler Compiler;
typedef struct Module Module;
typedef struct AstItem AstItem;
typedef struct AstExpr AstExpr;
typedef struct AstItemPath AstItemPath;

typedef enum NsEntryKind {
    NsEntry_Module,
    NsEntry_Item,
    NsEntry_Var,
} NsEntryKind;

typedef struct NsEntry {
    NsEntryKind kind;

    union {
        Module  *module;
        AstItem *item;
        AstExpr *var;
    };
} NsEntry;

typedef struct Namespace Namespace;
typedef struct Namespace {
    Namespace   *parent;
    Map(NsEntry) symbols;
} Namespace;

void ns_init(Namespace *ns, Namespace *parent);

NsEntry *ns_local(Compiler *compiler, FirString name);
NsEntry *ns_resolve(Compiler *compiler, AstItemPath *item_path);

NsEntry *ns_add_module(Compiler *compiler, FirString name, Module *module);
NsEntry *ns_add_item(Compiler *compiler, AstItem *item);
NsEntry *ns_add_var(Compiler *compiler, AstExpr *var);

#endif
