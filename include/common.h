//fig 2-7
//  *************************************************************
//  *                                                           *
//  *   C O M M O N   (Header)                                  *
//  *                                                           *
//  *   FILE:    prog2-1/common.h                               *
//  *                                                           *
//  *   MODULE:  Common                                         *
//  *                                                           *
//  *   Copyright__ (c) 1996 by Ronald Mak                        *
//  *   For instructional purposes only.  No warranties.        *
//  *                                                           *
//  *************************************************************

#ifndef common_h
#define common_h

#include "misc.h"
#include "symtable.h"
#include "icode.h"

extern int current_line_number;
extern int current_nesting_level;
extern cx_symtab cx_global_symtab;
extern int symtab_count;
extern cx_symtab *p_symtab_list;
extern cx_symtab **p_vector_symtabs;

extern cx_symtab cx_global_symtab;

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

#endif
//endfig

