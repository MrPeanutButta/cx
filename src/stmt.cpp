/** Executor (Statements)
 * exec_routine.cpp
 *
 * Execute standard Cx statements
 */

#include <iostream>
#include "exec.h"
#include "common.h"

/** execute_statement   	Execute a Cx statement
 *
 * @param p_function_id : ptr to the routine symtab node
 */
void cx_executor::execute_statement(cx_symtab_node *p_function_id) {
    if (token != tc_left_bracket) {
        ++statement_count;
        trace_statement();
    }

    switch (token) {
        case tc_identifier:
        {
            if (p_node->defn.how == dc_function) {
                execute_function_call(p_node);
            } else {
                cx_symtab_node *p_var = p_node;
                get_token();
                if (token_in(token, tokenlist_assign_ops)) {
                    execute_assignment(p_var);
                } else {
                    //cx_symtab_node *p_var = p_node;
                    //get_token();
                    
                    //if (token_in(token, tokenlist_assign_ops)) {
                        execute_assignment(p_var);
                   // } else {
                      //  execute_variable(p_var, true);
                    //}
                }
            }
        }
            break;
        case tc_DO: execute_DO(p_function_id);
            break;
        case tc_WHILE: execute_WHILE(p_function_id);
            break;
        case tc_IF: execute_IF(p_function_id);
            break;
        case tc_FOR: execute_FOR(p_function_id);
            break;
        case tc_SWITCH: //parse_SWITCH();
            break;
        case tc_CASE:
        case tc_DEFAULT://parse_case_label();
            break;
        case tc_BREAK:
            get_token();
            break_loop = true;
            break;
        case tc_left_bracket: execute_compound(p_function_id);
            break;
        case tc_RETURN: execute_RETURN(p_function_id);
            break;

        case tc_CLASS:
        case tc_NAMESPACE:
            /** TODO 
             * should initialize locals here
             */
            get_token();
            break;
        default:
            break;
    }
}

/** execute_statement_list        Execute a list or compounded
 *                              statements until a terminator token
 *                              is reached.
 *
 * @param p_function_id : ptr to the routine symtab node
 * @param terminator : token to terminate compound execution.
 */
void cx_executor::execute_statement_list(cx_symtab_node *p_function_id, cx_token_code terminator) {
    do {
        execute_statement(p_function_id);

        while (token == tc_semicolon) get_token();
    } while ((token != terminator) && (token != tc_dummy) && (!break_loop));
}

/** execute_compound     Execute statement block.
 *
 *      {       // begin
 *              <statements>;
 *      }       // end
 *
 * @param p_function_id : routine ID this statement is apart of.
 */
void cx_executor::execute_compound(cx_symtab_node * p_function_id) {

    get_token();

    execute_statement_list(p_function_id, tc_right_bracket);

    if (token == tc_right_bracket)get_token();
}

/** execute_IF   Executes if statements.
 *
 *      if(<boolean expression>)
 *              <statement>;
 *      else if (<boolean expression>)
 *              <statement>;
 *      else
 *              <statement>;
 *
 * @param p_function_id : routine ID this statement is apart of.
 */
void cx_executor::execute_IF(cx_symtab_node * p_function_id) {
    //  if
    get_token();

    // get the location of where to go to if <expr> is false.
    int at_false = get_location_marker();
    get_token();

    // (
    //get_token();

    execute_expression();
    bool condition = top()->basic_types.bool__;

    // )
    //get_token();

    if (condition) {

        // True: { or single statement
        execute_statement(p_function_id);
        while (token == tc_semicolon)get_token();

        // If there is an ELSE part, jump around it.
        if (token == tc_ELSE) {
            get_token();
            go_to(get_location_marker());
            get_token(); // token following the IF statement
        }
    } else {

        // False: Go to the false location.
        go_to(at_false);
        get_token();

        if (token == tc_ELSE) {

            // ELSE <stmt-2>
            get_token();
            get_location_marker(); // skip over location marker
            // { or single statement
            get_token();
            execute_statement(p_function_id);

            while (token == tc_semicolon)get_token();
        }
    }
}

/** execute_FOR  Executes for statement.
 *          initialize   condition     increment
 *      for(<statement>; <expression>; <expression>)
 *              <statement>;
 *
 * @param p_function_id
 */
void cx_executor::execute_FOR(cx_symtab_node * p_function_id) {

    bool condition = false;

    get_token(); // for
    // get the location of where to go to if <expr> is false.
    int break_point = get_location_marker();
    get_token();
    int statement_location = get_location_marker();
    get_token();
    int condition_marker = get_location_marker();
    get_token();
    int increment_marker = get_location_marker();


    get_token();

    // (
    get_token();

    if (token != tc_semicolon) {
        // declaration would go here //
        execute_assignment(p_node);
    }

    do {
        get_token(); //  ;
        if (token != tc_semicolon) {

            // expr 2
            execute_expression();
            get_token(); //  ;
        } else get_token();

        condition = top()->basic_types.bool__;
        pop();
        if (condition) {
            go_to(statement_location);
            get_token();
            execute_statement(p_function_id);
            if (break_loop) go_to(break_point);
        } else {
            go_to(break_point);
            get_token();
            break;
        }

        go_to(increment_marker);
        get_token();
        // expr 3
        execute_expression();

        go_to(condition_marker);
    } while (current_location() == condition_marker);

    break_loop = false;
}