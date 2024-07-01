#ifndef NAMESPACE_H
#define NAMESPACE_H

#include "map.h"


typedef enum NsEntryKind {
    NsEntry_Module,
    NsEntry_Item,
    NsEntry_Var,
} NsEntryKind;

typedef struct Module Module;
typedef struct AstItem AstItem;
typedef struct AstExpr AstExpr;
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

NsEntry *ns_local(Namespace *ns, FirString name);

typedef struct AstItemPath AstItemPath;
NsEntry *ns_resolve(Namespace *ns, AstItemPath *item_path);

NsEntry *ns_add_module(Namespace *ns, FirString name, Module *module);
NsEntry *ns_add_item(Namespace *ns, FirString name, AstItem *item);
NsEntry *ns_add_var(Namespace *ns, FirString name, AstExpr *var);

#endif
