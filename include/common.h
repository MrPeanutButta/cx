#ifndef common_h
#define common_h

#ifndef CX_DYNAMIC_API
#include "misc.h"
#include "symtable.h"
#include "icode.h"
#endif

extern int current_line_number;
extern int current_nesting_level;
extern cx_symtab cx_global_symtab;
extern cx_symtab *p_symtab_list;
// Pointers to predefined types.
extern cx_symtab_node *p_main_function_id;
extern cx_type *p_integer_type;
extern cx_type *p_float_type;
//extern cx_type *p_double_type;
extern cx_type *p_boolean_type;
extern cx_type *p_char_type;
extern cx_type *p_complex_type;
//extern  cx_type *p_file_type;
extern cx_type *p_dummy_type;

// tokens that can start a statement
extern const cx_token_code tokenlist_statement_start[];
// tokens that can follow a statement
extern const cx_token_code tokenlist_statement_follow[];
extern const cx_token_code tokenlist_caselabel_start[];
extern const cx_token_code tokenlist_expression_start[];
extern const cx_token_code tokenlist_expression_follow[];
extern const cx_token_code tokenlist_assign_ops[];
extern const cx_token_code tokenlist_relation_ops[];
extern const cx_token_code tokenlist_unary_ops[];
extern const cx_token_code tokenlist_add_ops[];
extern const cx_token_code tokenlist_mul_ops[];
extern const cx_token_code tokenlist_program_end[];
extern const cx_token_code tokenlist_equal_equal[];
extern const cx_token_code tokenlist_do[];
extern const cx_token_code tokenlist_left_bracket [];
extern const cx_token_code tokenlist_colon[];
extern const cx_token_code tokenlist_right___bracket[];
extern const cx_token_code tokenlist_semicolon[];
extern const cx_token_code tokenlist_right___paren[];
extern const cx_token_code tokenlist_left_paren[];
extern const cx_token_code tokenlist_declaration_start[];
extern const cx_token_code tokenlist_declaration_follow[];
extern const cx_token_code tokenlist_identifier_start[];
extern const cx_token_code tokenlist_sublist_follow[];
extern const cx_token_code tokenlist_field_decl_follow[];
extern const cx_token_code tokenlist_enum_const_start[];
extern const cx_token_code tokenlist_enum_const_follow[];
extern const cx_token_code tokenlist_subrange_limit_follow[];
extern const cx_token_code tokenlist_index_start[];
extern const cx_token_code tokenlist_index_follow[];
extern const cx_token_code tokenlist_index_list_follow[];
extern const cx_token_code tokenlist_subscript_or_field_start[];
extern const cx_token_code tokenlist_identifier_follow[];

bool token_in(cx_token_code tc, const cx_token_code *p_list);

// mains scope level on the symtab stack
#define __MAIN_ENTRY__ symtab_stack.find("main")

// env variable that holds the path to stdlib
#define __CX_STDLIB__   "CX_STDLIB"
#endif
//endfig

