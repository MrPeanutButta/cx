#include "cx-debug/exec.h"
#include "common.h"

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

    void *p_target_address = nullptr;

    if (p_target_id->defn.how == dc_function) {
        p_target_type = p_target_id->p_type;
        p_target = run_stack.get_value_address(p_target_id);
    }/* Assignment to variable or formal parameter.
      * execute_variable leaves the target address on
      * top of the runtime stack. */
    else if ((p_target_id->defn.how != dc_type)) {
        if (!token_in(token, tokenlist_assign_ops))get_token();
        p_target_type = execute_variable(p_target_id, true);

        if (p_target_type->form != fc_stream) {
            if (!p_target_type->is_scalar_type()) {
                p_target_address = top()->basic_types.addr__;
                pop();
            } else {
                p_target = (cx_stack_item *) top()->basic_types.addr__;
                pop();
            }
        }
    }

    switch (token) {
        case tc_RETURN:
        case tc_equal:
        {
            get_token();
            //p_expr_type = execute_expression();
            assign(p_target_id, p_target_type,
                    execute_expression(),
                    p_target, p_target_address);
        }
            break;
        case tc_plus_plus:
        {
            get_token();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->basic_types.float__++;
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {
                p_target->basic_types.int__++;
            } else if (p_target_type->base_type() == p_char_type) {
                p_target->basic_types.char__++;
            }

        }
            break;
        case tc_minus_minus:
        {
            get_token();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->basic_types.float__--;
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {
                p_target->basic_types.int__--;
            } else if (p_target_type->base_type() == p_char_type) {
                p_target->basic_types.char__--;
            }

        }
            break;
        case tc_plus_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->basic_types.float__ += p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();
            } else if (((p_target_type == p_integer_type) &&
                    (p_target_type->is_scalar_type())) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();
                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ += value;
            } else if (p_target_type == p_char_type) {
                char value = top()->basic_types.char__;
                pop();
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ += value;
            } else {
                const int size = p_expr_type->size;
                const int old_size = p_target_type->size;
                const int num_of_elements = (old_size + size) / p_expr_type->base_type()->size;

                p_target_address = realloc(p_target_address, old_size + size);

                if (p_target_address == nullptr) {
                    perror("realloc");
                    exit(0);
                }

                char *tmp = (char *) p_target_address;

                if (p_expr_type->is_scalar_type()) {
                    if (p_expr_type == p_integer_type) {
                        int value = top()->basic_types.int__;
                        memcpy(&tmp[old_size], &value, size);
                    } else if (p_expr_type == p_float_type) {
                        float value = top()->basic_types.float__;
                        memcpy(&tmp[old_size], &value, size);
                    } else if (p_expr_type == p_char_type) {
                        char value = top()->basic_types.char__;
                        memcpy(&tmp[old_size], &value, size);
                    }
                } else {
                    void *p_source = top()->basic_types.addr__;
                    memcpy(&tmp[old_size], p_source, size);
                }

                pop();
                p_target_id->runstack_item->basic_types.addr__ = p_target_address;
                p_target_id->p_type->array.element_count = num_of_elements;
                p_target_id->p_type->array.max_index = num_of_elements;
                p_target_id->p_type->size += size;
            }
        }

            break;
        case tc_minus_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->basic_types.float__ -= p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();
                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ -= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                pop();

                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ -= value;
            }

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
                p_target->basic_types.float__ *= p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ *= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                pop();

                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ -= value;
            }

        }
            break;
        case tc_divide_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if (p_target_type == p_float_type) {
                p_target->basic_types.float__ /= p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();
            } else if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = p_expr_type->base_type() == p_integer_type
                        ? top()->basic_types.int__ // real := integer
                        : top()->basic_types.float__; // real := real

                pop();
                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ /= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ /= value;
            }

        }
            break;
        case tc_modulas_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = top()->basic_types.int__; // real := integer
                pop();

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ %= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ %= value;
            }

        }
            break;
        case tc_bit_leftshift_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = top()->basic_types.int__; // real := integer

                pop();

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ <<= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ <<= value;
            }

        }
            break;
        case tc_bit_rightshift_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = top()->basic_types.int__; // real := integer

                pop();
                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ >>= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ >>= value;
            }

        }
            break;
        case tc_bit_AND_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = top()->basic_types.int__; // real := integer

                pop();

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ &= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ &= value;
            }

        }
            break;
        case tc_bit_XOR_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = top()->basic_types.int__; // real := integer

                pop();

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ ^= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ ^= value;
            }

        }
            break;
        case tc_bit_OR_equal:
        {
            get_token();
            p_expr_type = execute_expression();
            // Do the assignment.
            if ((p_target_type->base_type() == p_integer_type) ||
                    (p_target_type->base_type()->form == fc_enum)) {

                int value = top()->basic_types.int__; // real := integer

                pop();

                range_check(p_target_type, value);

                // integer     := integer
                // enumeration := enumeration
                p_target->basic_types.int__ |= value;
            } else if (p_target_type->base_type() == p_char_type) {
                char value = top()->basic_types.char__;
                pop();
                range_check(p_target_type, value);

                // character := character
                p_target->basic_types.char__ |= value;
            }

        }
            break;
        default:
            break;
    }

    //trace_data_store(p_target_id, *p_target_id->runstack_item, p_target_type);

}

void cx_executor::assign(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type, cx_stack_item* p_target,
        void* &p_target_address) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    if (p_target_type->is_scalar_type()) {

        memcpy(&p_target->basic_types, &top()->basic_types, p_expr_type->size);
        pop();
    } else if (p_target_type == p_file_type) {
      
        // location in io.cpp
        pop();
      
    } else {

        const int size = p_expr_type->size;
        const int old_size = p_target_type->size;
        const int num_of_elements = size / p_expr_type->base_type()->size;

        p_target_address = realloc(p_target_address, size);

        if (p_target_address == nullptr) {
            perror("realloc");
            exit(0);
        }

        char *tmp = (char *) p_target_address;

        if (p_expr_type->is_scalar_type()) {
            switch (expr_type) {
                case cx_short:
                {
                    short value = top()->basic_types.short__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_int:
                {
                    int value = top()->basic_types.int__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_char:
                {
                    char value = top()->basic_types.char__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value = top()->basic_types.wchar__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_long:
                {
                    long value = top()->basic_types.long__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_float:
                {
                    float value = top()->basic_types.float__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_double:
                {
                    double value = top()->basic_types.double__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_bool:
                {
                    bool value = top()->basic_types.bool__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value = top()->basic_types.uint8__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_uint16:
                {
                    uint16_t value = top()->basic_types.uint16__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_uint32:
                {
                    uint32_t value = top()->basic_types.uint32__;
                    memcpy(tmp, &value, size);
                }
                    break;
                case cx_uint64:
                {
                    uint64_t value = top()->basic_types.uint64__;
                    memcpy(tmp, &value, size);
                }
                    break;
                default:
                    break;
            }

        } else {
            void *p_source = top()->basic_types.addr__;
            memcpy(tmp, p_source, size);
        }

        pop();
        p_target_id->runstack_item->basic_types.addr__ = p_target_address;
        
        p_target_id->p_type->array.element_count = num_of_elements;
        p_target_id->p_type->array.max_index = num_of_elements;
        p_target_id->p_type->size = size;

    }
}