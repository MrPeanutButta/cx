#ifndef parser_h
#define parser_h

#include <string>
#include "misc.h"
#include "buffer.h"
#include "error.h"
#include "token.h"
#include "scanner.h"
#include "complist.h"
#include "icode.h"
#include "exec.h"
#include "symtable.h"
#include "types.h"

extern cx_symtab cx_global_symtab;

///  cx_parser     Parser class.

class cx_parser {
    cx_text_scanner * const p_scanner; // ptr to the scanner
    cx_token *p_token; // ptr to the current token
    cx_token_code token; // code of current token
    cx_symtab_stack symtab_stack;
    cx_icode icode;
    bool is_module;
    const char *file_name;
    
    cx_symtab_node *parse_function_header(cx_symtab_node *p_function_id);

    void parse_block(cx_symtab_node *p_function_id);
    void parse_iterator_block (cx_symtab_node *p_function_id);
    cx_symtab_node *parse_formal_parm_list(cx_symtab_node *p_function_id,
            int &count, int &total_size);

    cx_type *parse_subroutine_call(const cx_symtab_node *p_function_id,
            bool parm_check_flag);
    cx_type *parse_declared_subroutine_call(const cx_symtab_node *p_function_id,
            int parm_check_flag);

    void parse_actual_parm_list(const cx_symtab_node *p_function_id,
            int parm_check_flag);
    void parse_actual_parm(const cx_symtab_node *p_formal_id,
            int parm_check_flag);

    // declarations
    cx_symtab_node *allocate_new_node(cx_symtab_node *p_function_id);
    void parse_declarations_or_assignment(cx_symtab_node *p_function_id);
    void parse_constant_declaration(cx_symtab_node *p_function_id);
    void parse_constant(cx_symtab_node *p_const_id);
    void parse_identifier_constant(cx_symtab_node *p_id1, cx_token_code sign);

    void parse_definitions(cx_symtab_node *p_function_id);
    //void ParseIntegerDeclaration(cx_symtab_node *p_function_id);
    void parse_type_definitions(cx_symtab_node *p_function_id);
    cx_type *parse_type_spec(cx_symtab_node *p_node);

    cx_type *parse_identifier_type(const cx_symtab_node *p_id2);

    cx_type *parse_enum_header(cx_symtab_node *p_function_id);
    cx_type *parse_enumeration_type(void);

    cx_type *parse_subrange_type(cx_symtab_node *p_min_id);
    cx_type *parse_subrange_limit(cx_symtab_node *p_limit_id, int &limit);

    cx_type *parse_string_type(cx_symtab_node* p_function_id,
            cx_symtab_node* p_string_node);
    cx_type *parse_array_type(cx_symtab_node *p_function_id,
            cx_symtab_node *p_array_node);
    cx_type *parse_unksize_array_type(cx_symtab_node *p_function_id,
            cx_symtab_node *p_array_node);

    void parse_index_type(cx_symtab_node *p_array_node);
    int array_size(cx_type *p_array_type);
    cx_type *parse_complex_type(cx_symtab_node *p_function_id, cx_symtab_node *p_node);

    //void parse_variable_declarations(cx_symtab_node *p_function_id);
    void parse_member_decls(cx_symtab_node *p_function_id, cx_type *p_complex_type, int offset);
    //void ParseVarOrFieldDecls(cx_symtab_node *p_function_id,
    //cx_type *p_complex_type,
    //int offset);
    //cx_symtab_node *parse_id_sublist(const cx_symtab_node *p_function_id,
    //const cx_type *p_complex_type, cx_symtab_node *&p_last_id);

    // expressions
    cx_type *parse_rvalue(cx_type *lhs, cx_type *rhs);
    cx_type *parse_expression(void);
    //cx_type *ParseSuffix(cx_symtab_node *p_node);
    //void ParseSizeOf(void);
    cx_type *parse_simple_expression(void);
    cx_type *parse_term(void);
    cx_type *parse_factor(void);
    cx_type *parse_variable(const cx_symtab_node *p_id);
    cx_type *parse_subscripts(const cx_type *p_type);
    cx_type *parse_field(const cx_symtab_node *p_node, cx_type *p_type);
    cx_type *parse_iterator(cx_symtab_node *p_iterator);
    
    // statements
    void parse_statement(cx_symtab_node* p_function_id);
    cx_type *parse_assignment(const cx_symtab_node* p_target_id);
    void parse_statement_list(cx_symtab_node* p_function_id, cx_token_code terminator);
    void parse_DO(cx_symtab_node* p_function_id);
    void parse_WHILE(cx_symtab_node* p_function_id);
    void parse_IF(cx_symtab_node* p_function_id);
    void parse_FOR(cx_symtab_node* p_function_id);
    void parse_SWITCH(cx_symtab_node* p_function_id);
    void parse_case_branch(cx_symtab_node* p_function_id, const cx_type *p_expr_type);
    void parse_case_label(cx_symtab_node* p_function_id, const cx_type *p_expr_type);
    void parse_compound(cx_symtab_node* p_function_id);
    void parse_RETURN(cx_symtab_node* p_function_id);

    void parse_execute_directive(cx_symtab_node *p_function_id);

    void get_token(void) {
        p_token = p_scanner->get();
        token = p_token->code();
    }

    void get_token_append(void) {
        get_token();
        icode.put(token);
    }

    void insert_line_marker(void) {
        icode.insert_line_marker();
    }

    int put_location_marker(void) {
        return icode.put_location_marker();
    }

    void fixup_location_marker(int location) {
        icode.fixup_location_marker(location);
    }

    cx_symtab_node *search_local(const char *p_string) {
        return symtab_stack.search_local(p_string);
    }

    cx_symtab_node *search_all(const char *p_string) const {
        return symtab_stack.search_all(p_string);
    }

    cx_symtab_node *find(const char *p_string) const {
        return symtab_stack.find(p_string);
    }

    void copy_quoted_string(char *p_string, const char *p_quoted_string) const {
        int length = strlen(p_quoted_string) - 2;
        strncpy(p_string, &p_quoted_string[1], length);
        p_string[length] = '\0';
    }

    cx_symtab_node *enter_local(const char *p_string,
            cx_define_code dc = dc_undefined) {
        return symtab_stack.enter_local(p_string, dc);
    }

    cx_symtab_node *enter_new_local(const char *p_string,
            cx_define_code dc = dc_undefined) {
        return symtab_stack.enter_new_local(p_string, dc);
    }

    void conditional_get_token(cx_token_code tc, cx_error_code ec) {
        if (tc == token)get_token();
        else cx_error(ec);
    }

    void conditional_get_token_append(cx_token_code tc, cx_error_code ec) {
        if (tc == token) get_token_append();
        else cx_error(ec);
    }

    void resync(const cx_token_code *p_list1,
            const cx_token_code *p_list2 = nullptr,
            const cx_token_code *p_list3 = nullptr);

public:

    cx_parser(cx_text_in_buffer *p_buffer, bool std_lib_module = false)
    : p_scanner(new cx_text_scanner(p_buffer)), is_module(std_lib_module) {
        file_name = p_buffer->file_name();
    }

    ~cx_parser(void) {
        delete p_scanner;
    }

    cx_symtab_node *parse(void);
};

#endif