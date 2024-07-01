#ifndef TRY_H
#define TRY_H


#define try(expr) __extension__    \
({                                 \
    __typeof__(expr) maybe = expr; \
    if (!maybe) { return 0; }      \
    maybe;                         \
})

#define try_else(expr, else_expr) __extension__      \
({                                                      \
    __typeof__(expr) maybe = expr;                      \
    if (!maybe) { else_expr; return 0; } \
    maybe;                                              \
})

#endif
