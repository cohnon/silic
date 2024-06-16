#ifndef TRY_H
#define TRY_H

#define try(expr) __extension__    \
({                                 \
    __typeof__(expr) maybe = expr; \
    if (maybe == 0) { return 0; }  \
    maybe;                         \
})

#endif
