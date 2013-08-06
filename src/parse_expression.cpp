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

    cx_type *pResultType;
    cx_type *pOperandType;

    pResultType = parse_simple_expression();

    if (token_in(token, tokenlist_relation_ops)) {
        get_token_append();
        pOperandType = parse_simple_expression();
        CheckRelOpOperands(pResultType, pOperandType);
        pResultType = pBooleanType;
    }

    resync(tokenlist_expression_follow, tokenlist_statement_follow, tokenlist_statement_start);

    return pResultType;
}

/** parse_simple_expression       parse a simple expression
 *                              (unary operators + or - , and
 *                              binary operators + - and ||).
 * 
 * @return ptr to the simple expression's type object.
 */
cx_type *cx_parser::parse_simple_expression(void) {

    cx_type *pResultType;
    cx_type *pOperandType;
    cx_token_code op;
    bool unaryOpFlag = false;

    if (token_in(token, tokenlist_unary_ops)) {
        unaryOpFlag = true;
        get_token_append();
    }

    pResultType = parse_term();

    if (unaryOpFlag) CheckIntegerOrReal(pResultType);

    while (token_in(token, tokenlist_add_ops)) {
        op = token;
        get_token_append();
        pOperandType = parse_term();

        switch (op) {
            case tc_plus:
            case tc_minus:
                if (IntegerOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else if (RealOperands(pResultType, pOperandType)) {
                    pResultType = pFloatType;
                } else cx_error(err_incompatible_types);
                break;
            case tc_logic_OR:
                CheckBoolean(pResultType, pOperandType);
                pResultType = pBooleanType;
                break;
        }
    }

    return pResultType;
}

/** parse_term           parse a term (binary operators * / 
 *                      % and &&).
 * 
 * @return ptr to the term's type object.
 */
cx_type *cx_parser::parse_term(void) {

    cx_type *pResultType;
    cx_type *pOperandType;
    cx_token_code op;

    pResultType = parse_factor();

    while (token_in(token, tokenlist_mul_ops)) {

        op = token;
        get_token_append();
        pOperandType = parse_factor();

        switch (op) {
            case tc_star:
                if (IntegerOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else if (RealOperands(pResultType, pOperandType)) {
                    pResultType = pFloatType;
                } else cx_error(err_incompatible_types);
                break;
            case tc_divide:
                if (IntegerOperands(pResultType, pOperandType) ||
                        RealOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else cx_error(err_incompatible_types);
                break;
            case tc_modulas:
                if (IntegerOperands(pResultType, pOperandType)) {
                    pResultType = pIntegerType;
                } else cx_error(err_incompatible_types);
                break;
            case tc_logic_AND:
                CheckBoolean(pResultType, pOperandType);
                pResultType = pBooleanType;
                break;
        }
    }

    return pResultType;
}

/** parse_factor         parse a factor (identifier, number,
 *                      string, ! <factor>, or parenthesized
 *                      subexpression).
 * 
 * @return ptr to the factor's type object.
 */
cx_type *cx_parser::parse_factor(void) {

    cx_type *pResultType;

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
                    pResultType = parse_subroutine_call(p_node, true);
                    break;
                case dc_constant:
                    get_token_append();
                    pResultType = p_node->p_type;
                    break;

                case dc_type:
                    get_token_append();
                    pResultType = p_node->p_type;
                    break;
                case dc_variable:
                case dc_value_parm:
                case dc_reference:
                case dc_member:
                    get_token_append();
                    pResultType = parse_variable(p_node);
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
                    p_node->p_type = pIntegerType;
                    p_node->defn.constant.value.int__ = p_token->value().int__;
                } else {
                    p_node->p_type = pFloatType;
                    p_node->defn.constant.value.float__ = p_token->value().float__;
                }
            }

            pResultType = p_node->p_type;
            icode.put(p_node);
        }
            get_token_append();
            break;

        case tc_char:
        case tc_string:
        {

            char *p_string = p_token->string__();
            cx_symtab_node *p_node = search_all(p_token->string__());

            if (!p_node) {
                p_node = enter_local(p_token->string__());


                //p_string = p_node->string__();
                //int length = strlen(p_string) - 2;
                int length = p_node->string_length = strlen(p_string) - 2;
                //pResultType = length == 1 ?
                // pCharType : new cx_type(length);

                SetType(p_node->p_type, pCharType);

                if (length == 1) {
                    p_node->defn.constant.value.char__ = p_string[1];
                } else {
                    p_node->defn.constant.value.p_string = &p_string[1];
                    p_node->p_type->form = fcArray;
                    p_node->p_type->array.elmtCount = length;
                    p_node->p_type->array.maxIndex = (p_node->p_type->array.elmtCount - 1);
                    p_node->p_type->array.minIndex = 0;
                    p_node->p_type->array.pElmtType = pCharType;
                    p_node->p_type->array.pIndexType = pIntegerType;
                }

                pResultType = p_node->p_type;
            }
            icode.put(p_node);

            get_token_append();
        }
            break;

        case tc_left_paren:
            get_token_append();
            pResultType = parse_expression();

            conditional_get_token_append(tc_right_paren, err_missing_right_paren);
            break;
        case tc_logic_NOT:
            get_token_append();
            CheckBoolean(parse_factor());
            pResultType = pBooleanType;

            break;
        case tc_semicolon:
            //pResultType = pDummyType;
            break;
        default:
            cx_error(err_invalid_expression);
            pResultType = pDummyType;
            break;
    }

    return pResultType;
}

/** parse_variable       parse variable type, and assignment operators (= -- ++
 *                      += -= *= /= %= <<= >>= &= ^= |=).
 *                      Also parsed ([] and .).
 * 
 * @param p_id : variable node id.
 * @return variables type object ptr.
 */
cx_type *cx_parser::parse_variable(const cx_symtab_node* p_id) {
    cx_type *pResultType = p_id->p_type;

    switch (p_id->defn.how) {
        case dc_variable:
        case dc_value_parm:
        case dc_reference:
        case dc_pointer:
        case dc_function:
        case dc_undefined:
            break;

        default:
            pResultType = pDummyType;
            cx_error(err_invalid_identifier_usage);
            break;
    }

    //  [ or . : Loop to parse any subscripts and fields.
    int doneFlag = false;
    do {
        switch (token) {

            case tc_left_subscript:
                pResultType = parse_subscripts(pResultType);
                break;

            case tc_dot:
                pResultType = parse_field(pResultType);
                break;

            default: doneFlag = true;
        }
    } while (!doneFlag);

    if (token_in(token, tokenlist_assign_ops)) {
        cx_type *p_expr_type = nullptr;
        cx_type *pExprTypeCastFollow = nullptr;

        switch (token) {
            case tc_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();

                // if not semicolon, this may be an expression following a cast.
                // Keep p_expr_type same type, but we need to parse the expr.
                if (token != tc_semicolon) {
                    pExprTypeCastFollow = parse_expression();

                    // check if compatible
                    if (pResultType->form != fcStream) {
                        CheckAssignmentTypeCompatible(p_expr_type, pExprTypeCastFollow,
                                err_incompatible_assignment);
                    }
                }

                if (pResultType->form != fcStream) {
                    CheckAssignmentTypeCompatible(pResultType, p_expr_type,
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
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_minus_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_star_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_divide_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_modulas_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_leftshift_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_rightshift_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_AND_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_XOR_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_bit_OR_equal:
            {
                get_token_append();
                p_expr_type = parse_expression();
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            }
                break;
            case tc_comma:
            case tc_semicolon:
                break;
                break;
            case tc_identifier:
                get_token_append();
                p_expr_type = pResultType;
                CheckAssignmentTypeCompatible(pResultType, p_expr_type,
                        err_incompatible_assignment);
            default:
                cx_error(err_invalid_assignment);
                break;
        }
    }

    while (token_in(token, tokenlist_subscript_or_field_start)) {
        pResultType = token == tc_left_subscript ? parse_subscripts(pResultType)
                : parse_field(pResultType);
    }

    return pResultType;
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

        if (p_type->form == fcArray) {
            CheckAssignmentTypeCompatible(p_type->array.pIndexType,
                    parse_expression(),
                    err_incompatible_types);

            p_type = p_type->array.pElmtType;
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

    if ((token == tc_identifier) && (p_type->form == fcComplex)) {
        //  cx_symtab_node *pFieldId = p_type->complex.//->search(p_token->string__());

        // if (!pFieldId) cx_error(err_invalid_field);

        //  icode.put(pFieldId);

        get_token_append();

        // return pFieldId ? pFieldId->p_type : pDummyType;
    } else {

        cx_error(err_invalid_field);
        get_token_append();
        return pDummyType;

    }
}