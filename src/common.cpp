/**
 * common.cpp
 *
 * Data and routines common to the front and back ends.
 *
 * NOTE:
 *      token lists below are subject to change drastically
 *      once the parser and scanner are complete. -aaron
 */


#include "common.h"

// current scope level
int current_nesting_level = 0;
int current_line_number = 0;

/** Global symbols
 *
 * NOTE:
 *      when writing the Cx stdlib, this global symtab should be visible
 *      to the dynamic library to allow a library to add it's own symbols. - aaron
 */
cx_symtab cx_global_symtab;

// number of symtabs
int symtab_count = 0;

cx_symtab *p_symtab_list = nullptr;
cx_symtab **p_vector_symtabs = nullptr;

/// Tokens for resyncing the parser

// tokens that start a declaration
extern const cx_token_code tokenlist_declaration_start[] = {
                                                            tc_SIGNED,
                                                            tc_STATIC, tc_UNSIGNED,
                                                            tc_NAMESPACE, tc_USING, tc_char, tc_VIRTUAL,
                                                            tc_EXPORT,
                                                            tc_EXTERN, tc_TEMPLATE, tc_CONST, tc_PRIVATE,
                                                            tc_PROTECTED, tc_THREADLOCAL, tc_PUBLIC, tc_TYPEDEF,
                                                            tc_pound, tc_dummy
};

// tokens that follow a declaration
extern const cx_token_code tokenlist_declaration_follow[] = {
                                                             tc_semicolon, tc_identifier, tc_dummy
};

// identifier start
extern const cx_token_code tokenlist_identifier_start[] = {
                                                           tc_identifier, tc_dummy
};

// tokens that follow a sublist
extern const cx_token_code tokenlist_sublist_follow[] = {
                                                         tc_colon, tc_dummy
};

// tokens that follow a member/field declaration
extern const cx_token_code tokenlist_field_decl_follow[] = {
                                                            tc_semicolon, tc_identifier, tc_right_bracket, tc_dummy
};

// tokens that start an enum
// XXX broken
extern const cx_token_code tokenlist_enum_const_start[] = {
                                                           tc_identifier, tc_dummy
};

// tokens that follow an enum
// XXX not implemented
extern const cx_token_code tokenlist_enum_const_follow[] = {
                                                            tc_right_bracket, tc_comma, tc_semicolon, tc_dummy
};

// tokens that follow subrange
// XXX deprecated from the old Pascal Interp
extern const cx_token_code tokenlist_subrange_limit_follow[] = {
                                                                tc_dot_dot, tc_identifier, tc_plus, tc_minus, tc_string,
                                                                tc_right_subscript, tc_comma, tc_semicolon, tc_dummy
};

// tokens that start an index
// XXX broken
extern const cx_token_code tokenlist_index_start[] = {
                                                      tc_identifier, tc_number, tc_string, tc_left_paren, tc_plus, tc_minus,
                                                      tc_left_subscript, tc_right_subscript, tc_dummy
};

// tokens that follow an index
extern const cx_token_code tokenlist_index_follow[] = {
                                                       tc_comma, tc_right_subscript, tc_semicolon, tc_dummy
};

// XXX idk if this is used
extern const cx_token_code tokenlist_index_list_follow[] = {
                                                            tc_identifier, tc_right_subscript, tc_left_paren, tc_plus, tc_minus, tc_number,
                                                            tc_string, tc_semicolon,
                                                            tc_dummy
};

// XXX deprecated
extern const cx_token_code tokenlist_subscript_or_field_start[] = {
                                                                   tc_colon_colon,
                                                                   tc_pointer_member, tc_member_pointer, tc_dot, tc_left_subscript, tc_dummy
};

// tokens that follow an identifier
extern const cx_token_code tokenlist_identifier_follow[] = {
                                                            tc_comma, tc_identifier, tc_colon, tc_colon_colon, tc_semicolon,
                                                            tc_pointer_member, tc_right_paren, tc_member_pointer, tc_dot, tc_dummy
};

// tokens that can start a statement
extern const cx_token_code tokenlist_statement_start[] = {
                                                          tc_SWITCH, tc_FOR, tc_DO, tc_WHILE, tc_identifier,
                                                          tc_colon_colon, tc_RETURN, tc_CONTINUE, tc_IF,
                                                          tc_FRIEND, tc_GOTO, tc_TRY, tc_DELETE,
                                                          tc_SIGNED, tc_BREAK, tc_STATIC,
                                                          tc_UNSIGNED, tc_CATCH, tc_NAMESPACE, tc_USING, tc_char,
                                                          tc_VIRTUAL, tc_EXPORT,
                                                          tc_EXTERN, tc_TEMPLATE, tc_CONST,
                                                          tc_PRIVATE, tc_THIS, tc_PROTECTED, tc_THREADLOCAL,
                                                          tc_PUBLIC, tc_THROW, tc_TYPEDEF, tc_pound, tc_left_bracket,
                                                          tc_dummy
};

// tokens that can follow a statement
extern const cx_token_code tokenlist_statement_follow[] = {
                                                           tc_semicolon, tc_left_bracket, tc_right_bracket, tc_ELSE, tc_WHILE,
                                                           tc_dummy
};

extern const cx_token_code tokenlist_caselabel_start[] = {
                                                          tc_left_bracket, tc_CASE, tc_dummy
};

extern const cx_token_code tokenlist_expression_start[] = {
                                                           tc_plus, tc_minus, tc_identifier, tc_number, tc_string,
                                                           tc_bit_NOT, tc_logic_NOT, tc_left_paren, tc_bit_AND,
                                                           tc_left_subscript, tc_dummy
};

extern const cx_token_code tokenlist_expression_follow[] = {
                                                            tc_comma, tc_right_paren, tc_right_subscript, tc_plus_plus,
                                                            tc_minus_minus, tc_colon, tc_right_bracket, tc_DO, tc_semicolon,
                                                            tc_equal_equal, tc_not_equal, tc_lessthan, tc_greaterthan, tc_lessthan_equal, tc_greaterthan_equal,
                                                            tc_dummy
};

extern const cx_token_code tokenlist_assign_ops[] = {
                                                     tc_bit_XOR_equal, // ^=
                                                     tc_bit_AND_equal, // &=
                                                     tc_bit_OR_equal, // |=
                                                     tc_bit_leftshift_equal, // <<=
                                                     tc_bit_rightshift_equal, // >>=

                                                     tc_minus_equal, // -=
                                                     tc_plus_equal, // +=
                                                     tc_minus_minus, // --
                                                     tc_plus_plus, // ++
                                                     tc_divide_equal, // /=
                                                     tc_star_equal, // *=
                                                     tc_modulas_equal, // %=
                                                     tc_equal, // =
                                                     tc_RETURN, // return (expr)
                                                     tc_dummy
};

extern const cx_token_code tokenlist_relation_ops[] = {
                                                       tc_equal_equal, tc_not_equal, tc_lessthan, tc_greaterthan, tc_lessthan_equal, tc_greaterthan_equal, tc_dummy
};

extern const cx_token_code tokenlist_unary_ops[] = {
                                                    tc_plus, tc_minus, tc_bit_NOT, tc_dummy
};

extern const cx_token_code tokenlist_add_ops[] = {
                                                  tc_plus, tc_minus, tc_logic_OR, tc_bit_leftshift, tc_bit_rightshift,
                                                  tc_bit_AND, tc_bit_XOR, tc_bit_OR, tc_dummy
};

extern const cx_token_code tokenlist_mul_ops[] = {
                                                  tc_star, tc_divide, tc_modulas, tc_logic_AND, tc_dummy
};

extern const cx_token_code tokenlist_program_end[] = {
                                                      tc_RETURN, tc_right_bracket, tc_end_of_file, tc_dummy
};

extern const cx_token_code tokenlist_equal_equal[] = {tc_equal_equal, tc_dummy};
extern const cx_token_code tokenlist_do[] = {tc_DO, tc_dummy};
extern const cx_token_code tokenlist_left_bracket [] = {tc_left_bracket, tc_dummy};
extern const cx_token_code tokenlist_colon[] = {tc_colon, tc_dummy};
extern const cx_token_code tokenlist_right___bracket[] = {tc_right_bracket, tc_dummy};
extern const cx_token_code tokenlist_semicolon[] = {tc_semicolon, tc_dummy};
extern const cx_token_code tokenlist_right___paren[] = {tc_right_paren, tc_dummy};
extern const cx_token_code tokenlist_left_paren[] = {tc_left_paren, tc_dummy};

bool token_in (cx_token_code tc, const cx_token_code *p_list) {
    const cx_token_code *p_code;

    if (!p_list) return false;

    for (p_code = p_list; *p_code; ++p_code) {
        if (*p_code == tc) return true;
    }

    return false;
}