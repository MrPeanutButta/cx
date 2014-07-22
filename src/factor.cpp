#include "exec.h"
#include "common.h"
#include "members.h"

/** execute_factor       Execute a factor (identifier, number,
 *                      string, NOT <factor>, or parenthesized
 *                      subexpression).  An identifier can be
 *                      a function, constant, or variable.
 *
 * @return: ptr to factor's type object
 */
cx_type *cx_executor::execute_factor(void) {

    cx_type *p_result_type = nullptr; // ptr to result type
    cx_symtab_node *p_id = nullptr;

    switch (token) {
        case tc_identifier:
        {
            switch (p_node->defn.how) {

                case dc_function:
                {
                    cx_symtab_node *p_function = p_node;
                    p_result_type = execute_function_call(p_node);

                    if (token == tc_dot) {
                        p_function->runstack_item = top();
                        set_type(p_function->p_type, p_result_type);
                        pop();
                        //p_function->defn.how = dc_variable;
                        p_result_type = execute_variable(p_function, false);
                        //p_function->defn.how = dc_function;
                    }
                }
                    break;

                case dc_constant:
                    p_result_type = execute_constant(p_node);
                    break;
                case dc_type:
                    p_result_type = p_node->p_type;
                    get_token();
                    break;
                default:
                    p_id = p_node;
                    get_token();

                    if (token_in(token, tokenlist_assign_ops)) {
                        execute_assignment(p_id);
                        p_result_type = execute_variable(p_id, false);
                    } else {
                        p_result_type = execute_variable(p_id, false);
                    }

                    break;
            }
        }
            break;

        case tc_number:
            p_result_type = number(p_node);
            get_token();
            break;
        case tc_char:
            push((char) p_node->defn.constant.value.char__);
            p_result_type = p_char_type;
            get_token();
            break;
        case tc_string:
            push((void*) p_node->defn.constant.value.addr__);
            p_result_type = p_node->p_type;
            get_token();
            break;
        case tc_logic_NOT:
            // Execute boolean factor and invert its value.
            logic_not();
            p_result_type = p_boolean_type;
            break;
        case tc_left_paren:
            // Parenthesized subexpression:  Call execute_expression
            //                               recursively.
            get_token(); // first token after (
            p_result_type = execute_expression();
            get_token(); // first token after )

            break;
        case tc_left_bracket:
            p_result_type = execute_initialization_list();
            break;
        case tc_semicolon:
            break;
        default:
            break;
    }

    return p_result_type;
}

cx_type *cx_executor::number(cx_symtab_node* num) {

    cx_type *p_result_type = num->p_type;

    switch (num->p_type->type_code) {
        case cx_int:
            push(p_node->defn.constant.value.int__);
            break;
        case cx_float:
            push(p_node->defn.constant.value.float__);
            break;
        case cx_uint8:
            push(p_node->defn.constant.value.uint8__);
            break;
        default:break;
    }

    return p_result_type;
}

/** execute_constant     push a constant onto the runtime stack.
 *
 * @param p_id : ptr to constant identifier's symbol table node
 *
 * @return: ptr to constant's type object
 */
cx_type *cx_executor::execute_constant(const cx_symtab_node *p_id) {
    cx_type *p_type = p_id->p_type;
    const cx_data_value *value = &p_id->defn.constant.value;

    if (p_type->is_scalar_type()) {

        switch (p_type->type_code) {
            case cx_uint8:
                push((uint8_t) value->uint8__);
                break;
            case cx_int:
                push((int) value->int__);
                break;
            case cx_char:
                push((char) value->char__);
                break;
            case cx_bool:
                push((bool)value->bool__);
                break;
            case cx_float:
                push((float) value->float__);
                break;
            case cx_wchar:
                push((wchar_t)value->wchar__);
                break;
            default: break;
        }
    } else {
        push((void *) value->addr__);
    }

    get_token();
    trace_data_fetch(p_id, *top(), p_type);
    return p_type;
}

/**  execute_variable     push a variable's value or address onto
 *                        the runtime stack.
 *
 * @param p_id         : ptr to variable's symbol table node
 * @param address_flag : true to push address, false to push value
 *
 * @return: ptr to variable's type object
 */
cx_type *cx_executor::execute_variable(const cx_symtab_node *p_id,
        bool address_flag) {

    cx_type *p_type = p_id->p_type;
    p_type->is_address = address_flag;

    if (p_type->form != fc_complex) {

        // get the variable's runtime stack address.
        cx_stack_item *p_entry_id = run_stack.get_value_address(p_id);

        if (p_entry_id != nullptr) {
            push((p_id->defn.how == dc_reference) || (!p_type->is_scalar_type())
                    ? p_entry_id->basic_types.addr__ : p_entry_id);
        }
    } else {
        // if is 'this' pointer push pointer to p_node
        if (p_id->defn.is_this_ptr) {
            push((void*) p_id->defn.this_ptr.p_node);
        } else {
            push((void *) p_id);
        }
    }

    // Loop to execute any subscripts and field designators,
    // which will modify the data address at the top of the stack.
    int done_flag = false;
    cx_type *p_prev_type = p_type;
    do {
        switch (token) {
            case tc_left_subscript:
                p_type = execute_subscripts(p_type);
                break;

            case tc_dot:
            case tc_colon_colon:
                p_type = execute_field(p_type);
                break;
            default: done_flag = true;
        }

        p_prev_type = p_type;
    } while (!done_flag);

    // If address_flag is false, and the data is not an array
    // or a record, replace the address at the top of the stack
    // with the data value.
    if ((!address_flag) && (p_type->is_scalar_type())) {
		cx_stack_item *t = (cx_stack_item *)top();// ->basic_types.addr__;
        pop();
        switch (p_type->type_code) {
            case cx_uint8:
                push((uint8_t) t->basic_types.uint8__);
                break;
            case cx_int:
                push((int) t->basic_types.int__);
                break;
            case cx_char:
                push((char) t->basic_types.char__);
                break;
            case cx_bool:
                push((bool)t->basic_types.bool__);
                break;
            case cx_float:
                push((float) t->basic_types.float__);
                break;
            case cx_wchar:
                push((wchar_t)t->basic_types.wchar__);
                break;
            default: break;
        }
    }

    if (!address_flag) {
        //        void *p_data_value = (p_type->is_scalar_type()) ?
        //                top() : top()->basic_types.addr__;

        trace_data_fetch(p_id, *top(), p_type);
    }

    return p_type;
}

/**  execute_subscripts   Execute each subscript expression to
 *                      modify the data address at the top of
 *                      the runtime stack.
 *
 * @param p_type : ptr to array type object
 *
 * @return: ptr to subscripted variable's type object
 */
cx_type *cx_executor::execute_subscripts(const cx_type *p_type) {

    char *addr = (char *) top()->basic_types.addr__;

    // Loop to executed subscript lists enclosed in brackets.
    while (token == tc_left_subscript) {
        // Loop to execute comma-separated subscript expressions
        // within a subscript list.

        get_token(); // index
        execute_expression();

        // Evaluate and range check the subscript.
        int value = top()->basic_types.int__;

        pop();

        range_check(p_type, value);

        // Modify the data address at the top of the stack.
        pop();

        push(addr + (p_type->array.p_element_type->size * value));

        char *o = (char *) top()->basic_types.addr__;

        // Prepare for another subscript list.
        get_token(); // ]
        if (token == tc_left_subscript) {
            p_type = p_type->array.p_element_type;
        }
    }

    return (cx_type *) p_type->array.p_element_type;
}

/** execute_field         Execute a field designator to modify the
 *                       data address at the top of the runtime
 *                       stack
 *
 * @return: ptr to record field's type object
 */
cx_type *cx_executor::execute_field(cx_type *p_type) {
    get_token();
    cx_symtab_node *p_field_id = p_node;
    cx_type *p_result_type = nullptr;

    if (p_field_id->defn.how == dc_function) {
        if (p_field_id->defn.routine.which == func_std_member) {
            p_result_type = execute_std_member_call(p_field_id, p_type);
        } else {

            if (p_field_id->defn.routine.locals.p_variable_ids != nullptr) {
                if (p_type->form == fc_complex) {
                    p_field_id->defn.routine.locals.p_variable_ids->defn.this_ptr.p_node =
                            (cx_symtab_node *) top()->basic_types.addr__;
                } else if (p_field_id->defn.routine.locals.p_variable_ids != nullptr) {
                    p_field_id->defn.routine.locals.p_variable_ids->defn.this_ptr.p_stack_item = top();
                }
            }

            p_result_type = execute_function_call(p_field_id);

            if (p_result_type->is_scalar_type()) {
                cx_stack_item *address = top();
                pop();
                push((void *) address);
            }

            p_field_id->runstack_item = top();

        }
    } else if (p_field_id->defn.how == dc_variable) {
        push((void *) p_field_id->runstack_item);
        p_result_type = p_field_id->p_type;

        get_token();
    } else if (p_field_id->defn.how == dc_constant) {
        push((void*) &p_field_id->defn.constant.value);
        p_result_type = p_field_id->p_type;
        get_token();
    } else { // includes nested namespaces
        get_token();
    }

    return p_result_type;
}

