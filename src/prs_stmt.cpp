#include <cstdio>
#include "parser.h"
#include "common.h"

void cx_parser::parse_namespace(cx_symtab_node *p_function_id) {
    get_token();
    cx_symtab_node *p_namespace_id = search_local(p_token->string__());

    if (p_namespace_id == nullptr) {
        p_namespace_id = enter_local(p_token->string__(), dc_namespace);
        set_type(p_namespace_id->p_type, new cx_type());// = ;
        p_namespace_id->p_type->form = fc_namespace;
        p_namespace_id->p_type->complex.p_class_scope = new cx_symtab();
    } else if (p_namespace_id->defn.how != dc_namespace) {
        cx_error(err_invalid_namespace);
    }

    cx_symtab *p_old_symtab = (cx_symtab *) symtab_stack.get_current_symtab();

    symtab_stack.set_scope(++current_nesting_level);
    symtab_stack.set_current_symtab(p_namespace_id->p_type->complex.p_class_scope);

    //icode.put(p_namespace_id); // insert into icode
    get_token(); // namespace ID

    conditional_get_token_append(tc_left_bracket, err_missing_left_bracket); // open bracket

    parse_statement_list(p_namespace_id, tc_right_bracket);

    conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);
    symtab_stack.set_scope(--current_nesting_level);
    symtab_stack.set_current_symtab(p_old_symtab);

    cx_symtab_node *p_var_id = p_function_id->defn.routine.locals.p_variable_ids;
    if (!p_var_id) {
        p_function_id->defn.routine.locals.p_variable_ids = p_namespace_id;
        p_function_id->defn.routine.total_local_size += p_namespace_id->p_type->size;
    } else {
        while (p_var_id->next__)p_var_id = p_var_id->next__;

        p_var_id->next__ = p_namespace_id;
        
        p_function_id->defn.routine.total_local_size += p_namespace_id->p_type->size;
    }
}

void cx_parser::parse_class(cx_symtab_node *p_function_id) {
    get_token();
    cx_symtab_node *p_class_id = search_local(p_token->string__());

    if (p_class_id == nullptr) {
        p_class_id = enter_local(p_token->string__(), dc_type);
        p_class_id->p_type = new cx_type(fc_complex, 0, p_class_id);
        p_class_id->p_type->complex.p_class_scope = new cx_symtab();
    } else if (p_class_id->defn.how != dc_type) {
        cx_error(err_invalid_class_def);
    }

    cx_symtab *p_old_symtab = (cx_symtab *) symtab_stack.get_current_symtab();

    symtab_stack.set_scope(++current_nesting_level);
    symtab_stack.set_current_symtab(p_class_id->p_type->complex.p_class_scope);

    get_token(); // class ID
    conditional_get_token_append(tc_left_bracket, err_missing_left_bracket); // open bracket

    parse_statement_list(p_class_id, tc_right_bracket);

    conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);
    symtab_stack.set_scope(--current_nesting_level);
    symtab_stack.set_current_symtab(p_old_symtab);

    cx_symtab_node *p_var_id = p_function_id->defn.routine.locals.p_variable_ids;
    if (!p_var_id) {
        p_function_id->defn.routine.locals.p_variable_ids = p_class_id;
        p_function_id->defn.routine.total_local_size += p_class_id->p_type->size;
    } else {
        while (p_var_id->next__)p_var_id = p_var_id->next__;

        p_var_id->next__ = p_class_id;
        p_function_id->defn.routine.total_local_size += p_class_id->p_type->size;
    }
}

/** parse_statement          parse a statement.
 *
 * @param p_function_id : function in which this statement is executed.
 */
void cx_parser::parse_statement(cx_symtab_node *p_function_id) {
    insert_line_marker();

    switch (token) {
        case tc_identifier: parse_declarations_or_assignment(p_function_id);

            break;
            // not a type but a cv-qualifier
        case tc_CONST:
            get_token_append();
            parse_constant_declaration(p_function_id);
            break;
            //case tcEnum:
            //get_token_append();
            //            parse_enum_header(p_function_id);
            //break;
        case tc_DO: parse_do(p_function_id);
            break;
        case tc_WHILE: parse_while(p_function_id);
            break;
        case tc_IF: parse_if(p_function_id);
            break;
        case tc_FOR: parse_for(p_function_id);
            break;
        case tc_SWITCH: parse_switch(p_function_id);
            break;
            //case tc_CASE:
            //case tc_DEFAULT:parse_case_label(p_function_id);
            //  break;
        case tc_BREAK: get_token_append();
            break;
        case tc_left_bracket: parse_compound(p_function_id);
            break;
        case tc_RETURN: parse_return(p_function_id);
            break;
        case tc_pound:
            get_token();
            parse_execute_directive(p_function_id);
            break;
        case tc_CLASS: parse_class(p_function_id);
            break;
        case tc_NAMESPACE: parse_namespace(p_function_id);
            break;
        default:
            break;
    }

    if (token != tc_end_of_file) {
        resync(tokenlist_statement_follow, tokenlist_statement_start);
    }
}

/** parse_statement_list      parse a statement list until the
 *                          terminator token.
 *
 * @param p_function_id : function in which these statements are executed.
 * @param terminator : the token that terminates the list.
 */
void cx_parser::parse_statement_list(cx_symtab_node* p_function_id, cx_token_code terminator) {

    do {
        parse_statement(p_function_id);
        while (token == tc_semicolon)get_token_append();
    } while ((token != terminator) && (token != tc_end_of_file));

}

/** parse_assignment         parse an assignment statement.
 *
 * NOTE:
 *      Just calls parse_variable; This is because expressions are fully
 *      recursive.
 *
 * @param p_target_id : ptr to target id's symbol table node
 * @return ptr to the p_target_id type object.
 */
cx_type *cx_parser::parse_assignment(cx_symtab_node *p_target_id) {

    cx_type *p_target_type = parse_variable(p_target_id);

    return p_target_type;
}

/** parse_DO     parse do/while statement.
 *
 *      do
 *        <statement>;
 *      while(<expression>);
 *
 * @param p_function_id : ptr to this statements function Id.
 */
void cx_parser::parse_do(cx_symtab_node* p_function_id) {


    int break_point = put_location_marker();
    get_token_append(); //do

    parse_statement_list(p_function_id, tc_WHILE);

    conditional_get_token_append(tc_WHILE, err_missing_WHILE);
    conditional_get_token_append(tc_left_paren, err_missing_left_paren);

    check_boolean(parse_expression());

    conditional_get_token_append(tc_right_paren, err_missing_right_paren);

    fixup_location_marker(break_point);
}

/** parse_WHILE          parse while statement.
 *
 *      while(<expression>)
 *            <statement>;
 *
 * @param p_function_id : ptr to this statements function Id.
 */
void cx_parser::parse_while(cx_symtab_node* p_function_id) {

    int break_point = put_location_marker();

    get_token_append(); // while
    conditional_get_token_append(tc_left_paren, err_missing_left_paren);

    check_boolean(parse_expression());

    conditional_get_token_append(tc_right_paren, err_missing_right_paren);

    parse_statement(p_function_id);

    fixup_location_marker(break_point);
}

/** parse_IF             parse if/else statements.
 *
 *      if(<expression>)
 *         <statement>;
 *      else if (<expression>)
 *         <statement>;
 *      else
 *         <statement>;
 *
 * @param p_function_id : ptr to this statements function Id.
 */
void cx_parser::parse_if(cx_symtab_node* p_function_id) {

    // Append a placeholder location marker for where to go to if
    // <expr> is false.  Remember the location of this placeholder
    // so it can be fixed up below.
    int at_false_location_marker = put_location_marker();

    get_token_append();
    conditional_get_token_append(tc_left_paren, err_missing_left_paren);

    check_boolean(parse_expression());

    conditional_get_token_append(tc_right_paren, err_missing_right_paren);

    parse_statement(p_function_id);
    while (token == tc_semicolon) get_token_append();

    fixup_location_marker(at_false_location_marker);
    if (token == tc_ELSE) {
        // Append a placeholder location marker for the token that
        // follows the IF statement.  Remember the location of this
        // placeholder so it can be fixed up below.
        int at_follow_location_marker = put_location_marker();

        get_token_append();
        parse_statement(p_function_id);
        while (token == tc_semicolon) get_token_append();

        fixup_location_marker(at_follow_location_marker);
    }
}

/** parse_FOR            parse for statements.
 *
 *      for(<statement>; <expression>; <expression>)
 *              <statement>;
 *
 * @param p_function_id : ptr to this statements function Id.
 */
void cx_parser::parse_for(cx_symtab_node* p_function_id) {

    int break_point = put_location_marker();
    int statementMarker = put_location_marker();
    int condition_marker = put_location_marker();
    int increment_marker = put_location_marker();

    get_token_append(); // for

    conditional_get_token_append(tc_left_paren, err_missing_left_paren);

    if (token != tc_semicolon) {
        // declaration would go here //
        parse_declarations_or_assignment(p_function_id);
        conditional_get_token_append(tc_semicolon, err_missing_semicolon);
    } else get_token_append();

    fixup_location_marker(condition_marker);
    if (token != tc_semicolon) {

        // expr 2
        check_boolean(parse_expression());
        conditional_get_token_append(tc_semicolon, err_missing_semicolon);
    } else get_token_append();

    fixup_location_marker(increment_marker);
    if (token != tc_right_paren) {
        // expr 3
        parse_expression();
    }

    conditional_get_token_append(tc_right_paren, err_missing_right_paren);
    fixup_location_marker(statementMarker);
    parse_statement(p_function_id);
    fixup_location_marker(break_point);

}

/** parse_SWITCH         parse switch statements.
 *
 *      switch(<expression>){
 *              case <const-expression>:
 *              default:
 *      }
 *
 * NOTE:
 *      Broken/not implemented yet.
 *
 * @param p_function_id : ptr to this statements function Id.
 */
void cx_parser::parse_switch(cx_symtab_node* p_function_id) {

    get_token_append();
    conditional_get_token_append(tc_left_paren, err_missing_left_paren);

    cx_type *p_expr_type = parse_expression()->base_type();

    conditional_get_token_append(tc_right_paren, err_missing_right_paren);

    if ((p_expr_type != p_integer_type)
            && (p_expr_type != p_char_type)
            && (p_expr_type->form != fc_enum)) {
        cx_error(err_incompatible_types);
    }

    parse_statement(p_function_id);

}

/// XXX fixme

void cx_parser::parse_case_branch(cx_symtab_node* p_function_id, const cx_type *p_expr_type) {
    // c switch easier to parse that Pascal???
}

/// XXX fixme

void cx_parser::parse_case_label(cx_symtab_node* p_function_id, const cx_type *p_expr_type) {
    get_token_append();

    bool sign_flag(false);

    if (token_in(token, tokenlist_unary_ops)) {
        sign_flag = true;
        get_token_append();
    }

    switch (token) {
        case tc_identifier:
            if (!search_all(p_token->string__())) {
                cx_error(err_undefined_identifier);
            }

            get_token_append();
            break;
        case tc_number:
            if (p_token->type() != ty_integer) cx_error(err_invalid_constant);
            get_token_append();
            break;
        case tc_string:
            if (sign_flag || (strlen(p_token->string__()) != 3)) {
                cx_error(err_invalid_constant);
            }
            break;
        default:
            break;
    }

    conditional_get_token_append(tc_colon, err_missing_colon);

    parse_statement_list(p_function_id, tc_BREAK);
}

/** parse_compound       parse compounded statements.
 *
 *      {
 *         <statements>;
 *      }
 *
 * @param p_function_id : ptr to this statements function Id.
 */
void cx_parser::parse_compound(cx_symtab_node* p_function_id) {
    get_token_append();

    parse_statement_list(p_function_id, tc_right_bracket);

    //    if ((p_function_id->defn.routine.which == func_std_iterator)) return;


    conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);
}

/** parse_RETURN         parse return statements.
 *
 *      return;
 *      or
 *      return <expression>;
 *
 * @param p_function_id : ptr to this statements function Id.
 */
void cx_parser::parse_return(cx_symtab_node* p_function_id) {
    get_token_append();

    // expr 1
    check_assignment_type_compatible(p_function_id->p_type, parse_expression(),
            err_incompatible_types);
}