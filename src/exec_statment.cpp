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
                execute_subroutine_call(p_node);
            } else {
                execute_assignment(p_node);
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

/** execute_assignment 	Execute assignment.
 *
 *      p_target_id =, +=, -=, ++, --, /=, *=, %=, ^=
 *                >>=, <<=, &=, |= <expression>;
 * 
 * @param p_target_id : ptr to the symtab node being assigned some value
 *                    on the stack.
 */
void cx_executor::execute_assignment(const cx_symtab_node *p_target_id) {
    cx_stack_item *p_target = nullptr; // runtime stack address of target
    cx_type *p_target_type = nullptr; // ptr to target type object
    cx_type *p_expr_type = nullptr; // ptr to expression type object
    cx_type *p_expr2_type = nullptr; // reserved for casting

    if (p_target_id->defn.how == dc_function) {
        p_target_type = p_target_id->p_type;
        p_target = run_stack.get_value_address(p_target_id);
    }// Assignment to variable or formal parameter.
        // execute_variable leaves the target address on
        // top of the runtime stack.
    else if ((p_target_id->defn.how != dc_type)) {
        if (!token_in(token, tokenlist_assign_ops))get_token();
        p_target_type = execute_variable(p_target_id, true);

        if (p_target_type->form != fc_stream) {
            p_target = (cx_stack_item *) pop()->addr__;
        }
    }

    switch (token) {
        case tc_RETURN:
        case tc_equal:
        {
            get_token();
            p_expr_type = execute_expression();

            if (token != tc_semicolon) p_expr2_type = execute_expression();

            // Do the assignment.
            if (p_target_type == p_float_type) {

                if (!p_expr2_type) {

                    p_target->float__ =
                            (p_expr_type->base_type() == p_integer_type)
                            ? pop()->int__ // real := integer
                            : pop()->float__; // real := real

                } else {

                    p_target->float__ =
                            (p_expr2_type->base_type() == p_integer_type)
                            ? pop()->int__ // real := integer
                            : pop()->float__; // real := real

                }
            } else if (((p_target_type->base_type() == p_integer_type) &&
                    (p_target_type->base_type()->form != fc_array)) ||
                    (p_target_type->base_type()->form == fc_enum)) {
                int value(0);

                if (!p_expr2_type) {
                    value = ((p_expr_type->base_type() == p_integer_type) ||
                            (p_expr_type->base_type() == p_boolean_type))
                            ? pop()->int__ // real := integer
                            : pop()->float__; // real := real
                } else {
                    value = ((p_expr2_type->base_type() == p_integer_type) ||
                            (p_expr2_type->base_type() == p_boolean_type))
                            ? pop()->int__ // real := integer
                            : pop()->float__; // real := real
                }

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration


                p_target->int__ = value;


            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ = value;
            } else if (p_target_type->base_type() == p_file_type) {

                if (p_expr_type == p_integer_type) {
                    int int__ = pop()->int__;
                    fprintf(p_target_id->p_type->stream.p_file_stream, "%i", int__);
                } else if (p_expr_type == p_float_type) {
                    float float__ = pop()->float__;
                    fprintf(p_target_id->p_type->stream.p_file_stream, "%f", float__);
                } else if (p_expr_type == p_char_type) {
                    char char__ = pop()->char__;
                    fprintf(p_target_id->p_type->stream.p_file_stream, "%c", char__);
                } else {
                    void *p_source = pop()->addr__;
                    fprintf(p_target_id->p_type->stream.p_file_stream, "%s", (char *)p_source);
                }
            } else {
                void *p_source = pop()->addr__;

                // array  := array
                // record := record
                memcpy(p_target, p_source, p_target_type->size);
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_plus_plus:
        {
            get_token();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->float__++;
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {
                p_target->int__++;
            } else if (p_target_type->base_type() == p_char_type) {
                p_target->char__++;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_minus_minus:
        {
            get_token();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->float__--;
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {
                p_target->int__--;
            } else if (p_target_type->base_type() == p_char_type) {
                p_target->char__--;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_plus_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->float__ += p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ += value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ += value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_minus_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->float__ -= p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ -= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ -= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_star_equal:
        {
            get_token();
            p_expr_type = execute_expression();

            // if not semicolon, this may be an expression following a cast.
            if (token != tc_semicolon)execute_expression();

            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->float__ *= p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ *= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ -= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_divide_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->float__ /= p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? pop()->int__ // real := integer
                        : pop()->float__; // real := real

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ /= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ /= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_modulas_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = pop()->int__; // real := integer

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ %= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ %= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_bit_leftshift_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = pop()->int__; // real := integer

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ <<= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ <<= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_bit_rightshift_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = pop()->int__; // real := integer

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ >>= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ >>= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_bit_AND_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = pop()->int__; // real := integer

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ &= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ &= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_bit_XOR_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = pop()->int__; // real := integer

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ ^= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ ^= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
        case tc_bit_OR_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = pop()->int__; // real := integer

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->int__ |= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = pop()->char__;
                range_check(p_target_type, value);

                // character := character
                p_target->char__ |= value;
            }

            trace_data_store(p_target_id, p_target, p_target_type);
        }
            break;
    }
}

/** execute_DO   Executes do/while statement while <expression> is true.
 * 
 *      do
 *      <staement>;
 *      while(<expression>);
 * 
 * @param p_function_id : routine ID this statement is apart of. 
 */
void cx_executor::execute_DO(cx_symtab_node * p_function_id) {

    int break_point; // = get_location_marker();
    int at_loop_start = current_location(); // location of loop start in icode;
    int condition = 0;

    do {

        get_token(); // do
        break_point = get_location_marker();
        get_token();

        execute_statement_list(p_function_id, tc_WHILE);

        if (break_loop) {
            go_to(break_point);
            get_token();
            break;
        }

        get_token(); //while
        execute_expression(); // (condition)
        condition = pop()->int__;

        if (condition != 0) this->go_to(at_loop_start);
    } while (current_location() == at_loop_start);

    // reset break flag
    break_loop = false;

}

/** execute_WHILE        Executes while statement while <expression> is true.
 * 
 *      while(<expression>)
 *            <statement>;
 * 
 * @param p_function_id : routine ID this statement is apart of. 
 */
void cx_executor::execute_WHILE(cx_symtab_node * p_function_id) {

    int break_point;
    int at_loop_start = current_location();
    int condition = 0;

    do {

        get_token(); // while
        break_point = get_location_marker();
        get_token();

        get_token(); //  (
        execute_expression();
        condition = pop()->int__;
        get_token(); //  )
        if (condition != 0) {
            execute_statement(p_function_id);

            if (break_loop) {
                go_to(break_point);
                get_token();
                break;
            }

            go_to(at_loop_start);
        } else go_to(break_point);



    } while (current_location() == at_loop_start);

    // reset break
    break_loop = false;
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
    get_token();

    execute_expression();
    int condition = pop()->int__;

    // )
    get_token();

    if (condition != 0) {

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

    int condition = 0;

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

        condition = pop()->int__;
        if (condition != 0) {
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