#include "namespace.h"


void ns_init(Namespace *ns, Namespace *parent) {
    ns->parent = parent;
    map_init(&ns->symbols, 8);
}

NsEntry *ns_add_module(Namespace *ns, FirString name, Module *module) {
    (void)ns;
    (void)name;
    (void)module;

    return NULL;
}

NsEntry *ns_add_item(Namespace *ns, FirString name, AstItem *item) {
    (void)ns;
    (void)name;
    (void)item;

    return NULL;
}

NsEntry *ns_add_var(Namespace *ns, FirString name, AstExpr *var) {
    (void)ns;
    (void)name;
    (void)var;

    return NULL;
}

