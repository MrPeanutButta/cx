#include <cstdio>
#include <sstream>
#include "parser.h"
#include "common.h"
#include "types.h"
#include "std_members.h"

/** parse_expression     parse an expression (binary relational
 *                      operators = < > <> <= and >= ).
 *
 * @return ptr to the expression's type object.
 */
cx_type *cx_parser::parse_expression(void) {

    cx_type *p_result_type;
    cx_type *p_operand_type;

    p_result_type = parse_simple_expression();

    if (token_in(token, tokenlist_relation_ops)) {
        get_token_append();
        p_operand_type = parse_simple_expression();
        check_relational_op_operands(p_result_type, p_operand_type);
        p_result_type = p_boolean_type;
    }

    resync(tokenlist_expression_follow, tokenlist_statement_follow, tokenlist_statement_start);

    return p_result_type;
}

/** parse_simple_expression       parse a simple expression
 *                              (unary operators + or - , and
 *                              binary operators + - and ||).
 *
 * @return ptr to the simple expression's type object.
 */
cx_type *cx_parser::parse_simple_expression(void) {

    cx_type *p_result_type;
    cx_type *p_operand_type;
    cx_token_code op;
    cx_token_code unary_op;

    bool unary_op_flag = false;

    if (token_in(token, tokenlist_unary_ops)) {
        unary_op_flag = true;
        unary_op = token;
        get_token_append();
    }

    p_result_type = parse_term();

    if (unary_op_flag) {
        if (unary_op == tc_bit_NOT) {
            check_bitwise_integer(p_result_type);
        } else {
            check_integer_or_real(p_result_type);
        }
    }

    while (token_in(token, tokenlist_add_ops)) {
        op = token;
        get_token_append();

        p_operand_type = parse_term();

        switch (op) {
            case tc_plus:
                check_assignment_type_compatible(p_result_type, p_operand_type,
                        err_incompatible_types);

                p_result_type = parse_rvalue(p_result_type, p_operand_type);

                break;
            case tc_minus:
                check_assignment_type_compatible(p_result_type, p_operand_type,
                        err_incompatible_types);
                break;
            case tc_bit_leftshift:
            case tc_bit_rightshift:
            case tc_bit_AND:
            case tc_bit_XOR:
            case tc_bit_OR:
                check_bitwise_integer(p_result_type, p_operand_type);
                p_result_type = p_integer_type;
                break;
            case tc_logic_OR:
                check_boolean(p_result_type, p_operand_type);
                p_result_type = p_boolean_type;
                break;
            default:
                break;
        }
    }

    return p_result_type;
}

cx_type *cx_parser::parse_rvalue(cx_type* lhs, cx_type* rhs) {
    cx_type *p_result_type = lhs;
    cx_type *p_tmp_type = nullptr;

    if ((lhs->form == fc_array) || (rhs->form == fc_array)) {
        const int size = lhs->size + rhs->size;
        const int element_count = size / (lhs->form == fc_array ?
                lhs->base_type()->size :
                rhs->base_type()->size);

        p_tmp_type = new cx_type(fc_array, size, nullptr);
        p_tmp_type->is_temp_value = true;
        p_tmp_type->array.element_count = element_count;
        p_tmp_type->array.max_index = element_count;
        p_tmp_type->type_code = rhs->base_type()->type_code;

        if (lhs->form == fc_array) {
            set_type(p_tmp_type->array.p_element_type, lhs->array.p_element_type);
        } else {
            set_type(p_tmp_type->array.p_element_type, rhs->array.p_element_type);
        }

        p_result_type = p_tmp_type;

        if (rhs->is_temp_value) {
            remove_type(rhs);
        }

    }

    return p_result_type;
}

/** parse_term           parse a term (binary operators * /
 *                      % and &&).
 *
 * @return ptr to the term's type object.
 */
cx_type *cx_parser::parse_term(void) {

    cx_type *p_result_type;
    cx_type *p_operand_type;
    cx_token_code op;

    p_result_type = parse_factor();

    while (token_in(token, tokenlist_mul_ops)) {

        op = token;
        get_token_append();
        p_operand_type = parse_factor();

        switch (op) {
            case tc_star:
                if (integer_operands(p_result_type, p_operand_type)) {
                    p_result_type = p_integer_type;
                } else if (real_operands(p_result_type, p_operand_type)) {
                    p_result_type = p_float_type;
                } else cx_error(err_incompatible_types);
                break;
            case tc_divide:
                if (integer_operands(p_result_type, p_operand_type) ||
                        real_operands(p_result_type, p_operand_type)) {
                    p_result_type = p_integer_type;
                } else cx_error(err_incompatible_types);
                break;
            case tc_modulas:
                if (integer_operands(p_result_type, p_operand_type)) {
                    p_result_type = p_integer_type;
                } else cx_error(err_incompatible_types);
                break;
            case tc_logic_AND:
                check_boolean(p_result_type, p_operand_type);
                p_result_type = p_boolean_type;
                break;
            default:
                break;
        }
    }

    return p_result_type;
}

/** parse_factor         parse a factor (identifier, number,
 *                      string, ! <factor>, or parenthesized
 *                      subexpression).
 *
 * @return ptr to the factor's type object.
 */
cx_type *cx_parser::parse_factor(void) {

    cx_type *p_result_type = nullptr;

    switch (token) {
        case tc_identifier:
        {
            cx_symtab_node *p_node = search_all(p_token->string__());

            if (p_node == nullptr)
                cx_error(err_undefined_identifier);

            icode.put(p_node);

            switch (p_node->defn.how) {
                case dc_function:
                    get_token_append();
                    p_result_type = parse_subroutine_call(p_node, true);
                    if (token == tc_dot) {
                        p_result_type = parse_variable(p_node);
                    }
                    break;
                case dc_constant:
                    get_token_append();
                    p_result_type = p_node->p_type;
                    break;

                case dc_type:
                    get_token_append();
                    p_result_type = p_node->p_type;
                    break;
                case dc_variable:
                case dc_value_parm:
                case dc_reference:
                case dc_member:
				case dc_namespace:
                    get_token_append();
                    p_result_type = parse_variable(p_node);
                    break;
                default:
                    cx_error(err_undefined_identifier);
                    break;
            }
        }
            break;
        case tc_number:
        {
            cx_symtab_node *p_node = search_all(p_token->string__());

            if (!p_node) {
                p_node = enter_local(p_token->string__());

                if (p_token->type() == ty_integer) {
                    p_node->p_type = p_integer_type;
                    p_node->defn.constant.value.int__ = p_token->value().int__;
                } else {
                    p_node->p_type = p_float_type;
                    p_node->defn.constant.value.float__ = p_token->value().float__;
                }
            }

            p_result_type = p_node->p_type;
            icode.put(p_node);
        }
            get_token_append();
            break;

        case tc_char:
        {
            char *p_string = p_token->string__();
            cx_symtab_node *p_node = search_all(p_token->string__());
            const int length = strlen(p_string) - 2;

            if (!p_node) {
                p_node = enter_local(p_token->string__());
                p_result_type = p_char_type;
                set_type(p_node->p_type, p_char_type);

                p_node->defn.constant.value.char__ = p_string[1];
            }
            p_result_type = p_char_type;
            icode.put(p_node);
            get_token_append();
        }
            break;
        case tc_string:
        {

            char *p_string = p_token->string__();
            cx_symtab_node *p_node = search_all(p_token->string__());
            const int length = strlen(p_string) - 2;

            if (!p_node) {
                p_node = enter_local(p_token->string__());
                p_node->p_type = new cx_type(fc_array, length, nullptr);
                set_type(p_node->p_type->array.p_element_type, p_char_type);
                p_node->p_type->type_code = cx_char;
                const int size = sizeof (char) * (length + 1);
                p_node->defn.constant.value.addr__ = new char[size];
                memset(p_node->defn.constant.value.addr__, '\0', size);
                memcpy(p_node->defn.constant.value.addr__,
                        &p_string[1], size);

                // remove the quote
                char *t = (char *) p_node->defn.constant.value.addr__;
                t[length] = '\0';

                p_node->p_type->array.element_count = length;
                p_node->p_type->array.max_index = length;

            }

            p_result_type = p_node->p_type;
            icode.put(p_node);

            get_token_append();
        }
            break;

        case tc_left_paren:
            get_token_append();
            p_result_type = parse_expression();
            conditional_get_token_append(tc_right_paren, err_missing_right_paren);
            break;
        case tc_logic_NOT:
            get_token_append();
            parse_expression();
            p_result_type = p_boolean_type;
            break;
        case tc_left_bracket:
        {
            get_token_append();
            int total_size = 0;
            int elem_count = 0;
            bool comma = false;
            cx_type *p_prev_type = nullptr;

            do {
                p_result_type = parse_expression();
                p_prev_type = p_result_type;

                total_size += p_result_type->size;

                if (p_prev_type != nullptr) {
                    // make sure we init all of the same type
                    check_assignment_type_compatible(p_prev_type, p_result_type,
                            err_incompatible_types);
                }

                if (token == tc_comma) {
                    comma = true;
                    get_token_append();
                } else comma = false;

                ++elem_count;

            } while (comma);

            conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);

            cx_type *p_array_type = new cx_type(fc_array, total_size, nullptr);
            set_type(p_array_type->array.p_element_type, p_result_type);
            p_array_type->array.element_count = elem_count;
            p_array_type->array.max_index = elem_count;

            p_result_type = p_array_type;
        }
            break;
        case tc_semicolon:
            break;
        default:
            cx_error(err_invalid_expression);
            p_result_type = p_dummy_type;
            break;
    }

    return p_result_type;
}

/** parse_variable       parse variable type, and assignment operators (= -- ++
 *                      += -= *= /= %= <<= >>= &= ^= |=).
 *                      Also parsed ([] and .).
 *
 * @param p_id : variable node id.
 * @return variables type object ptr.
 */
cx_type *cx_parser::parse_variable(const cx_symtab_node* p_id) {
    cx_type *p_result_type = p_id->p_type;

    switch (p_id->defn.how) {
        case dc_variable:
        case dc_value_parm:
        case dc_reference:
        case dc_pointer:
        case dc_function:
        case dc_undefined:
		case dc_namespace:
            break;

        default:
            p_result_type = p_dummy_type;
            cx_error(err_invalid_identifier_usage);
            break;
    }

    //  [ or . : Loop to parse any subscripts and fields.
    int done_flag = false;
    cx_type *p_prev_type = p_result_type;
    do {
        switch (token) {

            case tc_left_subscript:
                p_result_type = parse_subscripts(p_result_type);
                break;

            case tc_dot:
			case tc_colon_colon:
				p_result_type = parse_field(p_id, p_prev_type);
                p_prev_type = p_result_type;
                break;

            default: done_flag = true;
        }
    } while (!done_flag);

    if (token_in(token, tokenlist_assign_ops)) {
        cx_type *p_expr_type = nullptr;

        switch (token) {
            case tc_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();

                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_minus_minus:
                get_token_append();
                break;
            case tc_plus_plus:
                get_token_append();
                break;
            case tc_plus_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_minus_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_star_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_divide_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_modulas_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_bit_leftshift_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_bit_rightshift_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_bit_AND_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_bit_XOR_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_bit_OR_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);

                p_result_type = p_expr_type;
            }
                break;
            case tc_comma:
            case tc_semicolon:
            case tc_RETURN:
                break;
            case tc_identifier:
                get_token_append();
                p_expr_type = p_result_type;

            default:
                cx_error(err_invalid_assignment);
                break;
        }
    }

    while (token_in(token, tokenlist_subscript_or_field_start)) {
        p_result_type = token == tc_left_subscript ? parse_subscripts(p_result_type)
                : parse_field(p_id, p_result_type);
    }

    return p_result_type;
}

/** parse_subscripts     parse a bracketed list of subscripts
 *                      following an array variable:
 *
 *                          [ <expr> ]
 *
 * @param p_type : ptr to the array's type object.
 * @return ptr to the array element's type object.
 */
cx_type *cx_parser::parse_subscripts(const cx_type* p_type) {

    //cx_type *p_result_type = (cx_type *) p_type;

    /*if (p_type->array.p_element_type->form == fc_array){
            p_type = p_type->array.p_element_type;
    }*/

    int column = 0;

    do {
        get_token_append();

        if (p_type->form == fc_array) {
            check_assignment_type_compatible(p_integer_type,
                    parse_expression(),
                    err_incompatible_types);
        } else {
            cx_error(err_too_many_subscripts);
            parse_expression();
        }

        conditional_get_token_append(tc_right_subscript, err_missing_right_subscript);
        if (token == tc_left_subscript)++column;
    } while (token == tc_left_subscript);


    cx_type *elem = (cx_type *) p_type;
    for (int i = 0; i < column; i++) {
        if (elem->form == fc_array) {
            elem = elem->array.p_element_type;
        }
    }

    return elem->array.p_element_type;
}

std::string unique_name(const std::string &prefix, const int &postfix) {
    std::stringstream ss;
    ss.clear();

    ss << prefix << "_" << postfix << '\0';

    return ss.str();
}

/** parse_field          parse a field following a record
 *                      variable:
 *
 *                          . <id>
 *
 * @param p_type : ptr to the record's type object
 * @return ptr to the field's type object.
 */
cx_type *cx_parser::parse_field(const cx_symtab_node *p_node, cx_type* p_type) {
    get_token_append();

    if (token == tc_identifier) {
        cx_symtab_node *p_field_id = p_type->complex.p_class_scope->search(p_token->string__());
        if (p_field_id == nullptr) cx_error(err_invalid_field);

        icode.put(p_field_id);
        get_token_append();

        if (p_field_id->defn.how == dc_function) {
            parse_subroutine_call(p_field_id, true);
        }

        return p_field_id != nullptr ?
                p_field_id->p_type :
                p_dummy_type;

    } else {
        cx_error(err_invalid_field);
        get_token_append();
        return p_dummy_type;
    }

    return p_dummy_type;
}