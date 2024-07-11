#include "namespace.h"


void ns_init(Namespace *ns, Namespace *parent) {
    ns->parent = parent;
    map_init(&ns->symbols, 8);
}

void ns_enter_scope(Compiler *compiler, Namespace *ns);
void ns_exit_scope(Compiler *compiler);

NsEntry *ns_local(Compiler *compiler, FirString name);
NsEntry *ns_resolve(Compiler *compiler, AstItemPath *item_path);

NsEntry *ns_add_module(Compiler *compiler, FirString name, Module *module) {
    (void)compiler;
    (void)name;
    (void)module;

    return NULL;
}

NsEntry *ns_add_item(Compiler *compiler, AstItem *item) {
    (void)compiler;
    (void)item;

    return NULL;
}

NsEntry *ns_add_var(Compiler *compiler, AstExpr *var) {
    (void)compiler;
    (void)var;

    return NULL;
}

