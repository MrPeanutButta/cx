#include <cstdio>
#include "parser.h"
#include "common.h"
#include "types.h"

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
    bool unary_op_flag = false;

    if (token_in(token, tokenlist_unary_ops)) {
        unary_op_flag = true;
        get_token_append();
    }

    p_result_type = parse_term();

    if (unary_op_flag) check_integer_or_real(p_result_type);

    while (token_in(token, tokenlist_add_ops)) {
        op = token;
        get_token_append();
        p_operand_type = parse_term();

        switch (op) {
            case tc_plus:
            case tc_minus:
                if (integer_operands(p_result_type, p_operand_type)) {
                    p_result_type = p_integer_type;
                } else if (real_operands(p_result_type, p_operand_type)) {
                    p_result_type = p_float_type;
                } else cx_error(err_incompatible_types);
                break;
            case tc_logic_OR:
                check_boolean(p_result_type, p_operand_type);
                p_result_type = p_boolean_type;
                break;
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
                    //conditional_get_token_append(tc_left_paren, ::err_missing_left_paren);
                    p_result_type = parse_subroutine_call(p_node, true);
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
        case tc_string:
        {

            char *p_string = p_token->string__();
            cx_symtab_node *p_node = search_all(p_token->string__());
            int length = strlen(p_string) - 2;
                                                // '\0' == -1
            p_result_type = ((length == 1) || (length == -1)) ?
                    p_char_type : new cx_type(length);

            if (!p_node) {
                p_node = enter_local(p_token->string__());
                set_type(p_node->p_type, p_result_type);

                if (length <= 1) {
                    p_node->defn.constant.value.char__ = p_string[1];
                } else {
                    p_node->defn.constant.value.p_string = new char[length];
                    
                    memcpy(p_node->defn.constant.value.p_string, 
                            &p_string[1], length);
                    
                    //p_node->defn.constant.value.p_string = &p_string[1];
                    p_node->defn.constant.value.p_string[length] = '\0';
                    p_node->p_type->form = fc_array;
                    p_node->p_type->array.element_count = length;
                    p_node->p_type->array.maxIndex = (p_node->p_type->array.element_count - 1);
                    p_node->p_type->array.min_index = 0;
                    p_node->p_type->array.p_element_type = p_char_type;
                    p_node->p_type->array.p_index_type = p_integer_type;
                    //p_node->p_type->size = length;
                }
            }
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
            check_boolean(parse_factor());
            p_result_type = p_boolean_type;

            break;
        case tc_semicolon:
            //p_result_type = p_dummy_type;
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
            break;

        default:
            p_result_type = p_dummy_type;
            cx_error(err_invalid_identifier_usage);
            break;
    }

    //  [ or . : Loop to parse any subscripts and fields.
    int done_flag = false;
    do {
        switch (token) {

            case tc_left_subscript:
                p_result_type = parse_subscripts(p_result_type);
                break;

            case tc_dot:
                p_result_type = parse_field(p_result_type);
                break;

            default: done_flag = true;
        }
    } while (!done_flag);

    if (token_in(token, tokenlist_assign_ops)) {
        cx_type *p_expr_type = nullptr;
        cx_type *p_expr_type_cast_follow = nullptr;

        switch (token) {
            case tc_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();

                // if not semicolon, this may be an expression following a cast.
                // Keep p_expr_type same type, but we need to parse the expr.
                if (token != tc_semicolon) {
                    p_expr_type_cast_follow = parse_expression();

                    // check if compatible
                    if (p_result_type->form != fc_stream) {
                        check_assignment_type_compatible(p_expr_type, p_expr_type_cast_follow,
                                err_incompatible_assignment);
                    }
                }

                if (p_result_type->form != fc_stream) {
                    check_assignment_type_compatible(p_result_type, p_expr_type,
                            err_incompatible_assignment);
                }
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
            }
                break;
            case tc_minus_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_star_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_divide_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_modulas_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_leftshift_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_rightshift_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_AND_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_XOR_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_OR_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_comma:
            case tc_semicolon:
                break;
                break;
            case tc_identifier:
                get_token_append();
                p_expr_type = p_result_type;
                check_assignment_type_compatible(p_result_type, p_expr_type,
                        err_incompatible_assignment);
            default:
                cx_error(err_invalid_assignment);
                break;
        }
    }

    while (token_in(token, tokenlist_subscript_or_field_start)) {
        p_result_type = token == tc_left_subscript ? parse_subscripts(p_result_type)
                : parse_field(p_result_type);
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
    do {
        get_token_append();

        if (p_type->form == fc_array) {
            check_assignment_type_compatible(p_type->array.p_index_type,
                    parse_expression(),
                    err_incompatible_types);

            p_type = p_type->array.p_element_type;
        } else {
            cx_error(err_too_many_subscripts);
            parse_expression();
        }

    } while (token == tc_comma);

    conditional_get_token_append(tc_right_subscript, err_missing_right_subscript);

    return (cx_type *) p_type;
}

/** parse_field          parse a field following a record
 *                      variable:
 *
 *                          . <id>
 * 
 * @param p_type : ptr to the record's type object
 * @return ptr to the field's type object.
 */
cx_type *cx_parser::parse_field(const cx_type* p_type) {
    get_token_append();

    if ((token == tc_identifier) && (p_type->form == fc_complex)) {
        //  cx_symtab_node *p_field_id = p_type->complex.//->search(p_token->string__());

        // if (!p_field_id) cx_error(err_invalid_field);

        //  icode.put(p_field_id);

        get_token_append();

        // return p_field_id ? p_field_id->p_type : p_dummy_type;
    } else {

        cx_error(err_invalid_field);
        get_token_append();
        return p_dummy_type;

    }
}