#ifndef ACB_H
#define ACB_H

#include <stdint.h>
#include <stdio.h>

typedef struct acb_Type acb_Type;
typedef struct acb_Function acb_Function;
typedef struct acb_Block acb_Block;
typedef struct acb_Value acb_Value;

void acb_init(void);
void acb_emit_c(FILE *fp);

// types
acb_Type *acb_type_int(size_t bits, _Bool is_signed);
acb_Type *acb_type_float(size_t bits);

acb_Type *acb_type_declare_struct(void);
void acb_type_defined_struct(acb_Type **fields, size_t fields_len);

// functions
acb_Function acb_create_function(char *name, acb_Type *ret, acb_Type **params, size_t params_len);

// blocks
acb_Block *acb_create_block(void);
void acb_set_current_block(acb_Block *blk);

// control flow
void acb_goto(acb_Block *blk);
void acb_return(acb_Value *val);

typedef struct { acb_Block *then_lbl, *else_lbl; } acb_IfControl;
acb_IfControl acb_if(acb_Value *cond);

// instructions
acb_Value *acb_int_literal(uint64_t val);
acb_Value *acb_float_literal(double val);

acb_Value *acb_add(acb_Value *lhs, acb_Value *rhs);
acb_Value *acb_sub(acb_Value *lhs, acb_Value *rhs);
acb_Value *acb_mul(acb_Value *lhs, acb_Value *rhs);
acb_Value *acb_div(acb_Value *lhs, acb_Value *rhs);

#endif
