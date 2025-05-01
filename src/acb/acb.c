#include "acb.h"
#include <stdio.h>

typedef struct acb_Context {
    acb_Function *cur_fn;
    acb_Block *cur_blk;
} acb_Context;

acb_Context ctx;

void acb_init(void) {
}

void acb_emit_c(FILE *fp) {
    fprintf(fp, "#include <stdio.h>\n\nint main(void) { printf(\"hello world\\n\"); }\n");
}
