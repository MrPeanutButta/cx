#include "common.h"
#include "parser.h"

/** parse_type_definitions    parse a list of type definitions
 *                          separated by commas:
 *
 *                              <type> <id>, ... ;
 * 
 * @param p_function_id : ptr to symbol table node of program,
 *                     procedure, or function identifier.
 */
void cx_parser::parse_type_definitions(cx_symtab_node *p_function_id) {
    cx_symtab_node *p_last_id = nullptr;

    while (token == tc_identifier) {
        cx_symtab_node *p_type_id = enter_new_local(p_token->string__());

        if (!p_function_id->defn.routine.locals.p_type_ids) {
            p_function_id->defn.routine.locals.p_type_ids = p_type_id;
        } else {
            p_last_id->next__ = p_type_id;
        }

        p_last_id = p_type_id;

        get_token_append();
        conditional_get_token(tc_equal, err_missing_equal);

        //        set_type(p_type_id->p_type, parse_type_spec());
        p_type_id->defn.how = dc_type;

        if (!p_type_id->p_type->p_type_id) {
            p_type_id->p_type->p_type_id = p_type_id;
        }

        resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);
        conditional_get_token(tc_semicolon, err_missing_semicolon);

        while (token == tc_semicolon)get_token_append();

        resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);

    }
}

/** parse_type_spec       parse a type specification.
 * 
 * @param p_node : ptr to object node.
 * @return ptr to type object.
 */
cx_type *cx_parser::parse_type_spec(cx_symtab_node *p_node) {
    switch (token) {
        case tc_identifier:
        {
            cx_symtab_node *p_id = find(p_token->string__());

            switch (p_id->defn.how) {
                case dc_type: return parse_identifier_type(p_id);
                case dc_constant: return parse_subrange_type(p_id);
                default:
                    cx_error(err_not_a_type_identifier);
                    get_token_append();
                    return (p_dummy_type);
            }
        }

        case tc_left_bracket: return parse_enumeration_type();

            /* found empty subscript.
             * array must have initializer */
        case tc_right_subscript: return parse_assignment(p_node);
            //        case tcRecord: return parse_complex_type();
        case tc_plus:
        case tc_minus:
        case tc_number:
        case tc_char:
        case tc_string: return parse_subrange_type(nullptr);

        default:
            cx_error(err_invalid_type);
            return (p_dummy_type);

    }
}

/** parse_identifier_type     In a type defintion of the form
 *
 *                               <id-1> = <id-2>
 *
 *                          parse <id-2>.
 * 
 * @param p_id2 : ptr to symbol table node of <id-2>.
 * @return ptr to type object of <id-2>.
 */
cx_type *cx_parser::parse_identifier_type(const cx_symtab_node *p_id2) {
    get_token_append();
    return p_id2->p_type;
}

/** parse_enumeration_type    parse a enumeration type
 *                          specification:
 *      enum <id> { <enum-list> };
 *  
 * @return  
 */
cx_type *cx_parser::parse_enumeration_type(void) {
    cx_type *p_type = new cx_type(fc_enum, sizeof (int), nullptr);
    cx_symtab_node *p_last_id = nullptr;

    int const_value = -1;

    get_token_append();
    resync(tokenlist_enum_const_start);

    while (token == tc_identifier) {
        cx_symtab_node *p_const_id = enter_new_local(p_token->string__());
        ++const_value;

        if (p_const_id->defn.how == dc_undefined) {
            p_const_id->defn.how = dc_constant;

            p_const_id->defn.constant.value.int__ = const_value;
            set_type(p_const_id->p_type, p_type);

            if (!p_last_id) {
                p_type->enumeration.p_const_ids = p_last_id = p_const_id;
            } else {
                p_last_id->next__ = p_const_id;
                p_last_id = p_const_id;
            }
        }

        get_token_append();
        resync(tokenlist_enum_const_follow);

        if (token == tc_comma) {

            do {
                get_token_append();
                resync(tokenlist_enum_const_start, tokenlist_enum_const_follow);
                if (token == tc_comma) cx_error(err_missing_identifier);
            } while (token == tc_comma);
            if (token != tc_identifier) cx_error(err_missing_identifier);
        } else if (token == tc_identifier) cx_error(err_missing_comma);
    }

    conditional_get_token(tc_right_bracket, err_missing_right___bracket);

    p_type->enumeration.max = const_value;
    return p_type;
}

/** parse_subrange_type       parse a subrange type specification.
 * 
 * NOTE:
 *      Not implemented.
 * 
 * @param p_min_id
 * @return 
 */
cx_type *cx_parser::parse_subrange_type(cx_symtab_node* p_min_id) {
    /*cx_type *p_type = new cx_type(fc_subrange, 0, nullptr);

    set_type(p_type->subrange.p_base_type, parse_subrange_limit(p_min_id, p_type->subrange.min));

    resync(tokenlist_subrange_limit_follow, tokenlist_declaration_start);
    conditional_get_token(tc_dot_dot, errMissingDotDot);

    cx_type *pMaxType = parse_subrange_limit(nullptr, p_type->subrange.max);

    if (pMaxType != p_type->subrange.p_base_type) {
        cx_error(err_incompatible_types);
        p_type->subrange.min = p_type->subrange.max = 0;
    } else if (p_type->subrange.min > p_type->subrange.max) {
        cx_error(errMinGtMax);

        int temp = p_type->subrange.min;
        p_type->subrange.min = p_type->subrange.max;
        p_type->subrange.max = temp;
    }

    p_type->size = p_type->subrange.p_base_type->size;
    return p_type;*/
}

/** parse_subrange_limit      parse a mininum or maximum limit
 *                          constant of a subrange type.
 * 
 * NOTE:
 *      Not implemented.
 * 
 * @param p_limit_id
 * @param limit
 * @return 
 */
cx_type *cx_parser::parse_subrange_limit(cx_symtab_node* p_limit_id, int& limit) {
    cx_type *p_type = p_dummy_type;
    cx_token_code sign = tc_dummy;

    limit = 0;

    if (token_in(token, tokenlist_unary_ops)) {
        if (token == tc_minus) sign = tc_minus;
        get_token_append();
    }

    switch (token) {
        case tc_number:
            if (p_token->type() == ty_integer) {
                limit = sign == tc_minus ?
                        -p_token->value().int__ :
                        p_token->value().int__;

                p_type = p_integer_type;

            } else cx_error(err_invalid_subrange_type);
            break;
        case tc_identifier:
            if (!p_limit_id) p_limit_id = find(p_token->string__());

            if (p_limit_id->defn.how == dc_undefined) {
                p_limit_id->defn.how = dc_constant;
                p_type = set_type(p_limit_id->p_type, p_dummy_type);
                break;
            } else if ((p_limit_id->p_type == p_float_type) ||
                    (p_limit_id->p_type == p_dummy_type) ||
                    (p_limit_id->p_type->form == fc_array)) {
                cx_error(err_invalid_subrange_type);
            } else if (p_limit_id->defn.how == dc_constant) {

                if (p_limit_id->p_type == p_integer_type) {
                    limit = sign == tc_minus
                            ? -p_limit_id->defn.constant.value.int__
                            : p_limit_id->defn.constant.value.int__;
                } else if (p_limit_id->p_type == p_char_type) {
                    if (sign != tc_dummy) cx_error(err_invalid_constant);
                    limit = p_limit_id->defn.constant.value.char__;
                } else if (p_limit_id->p_type->form == fc_enum) {
                    if (sign != tc_dummy) cx_error(err_invalid_constant);
                    limit = p_limit_id->defn.constant.value.int__;
                }
                p_type = p_limit_id->p_type;
            } else cx_error(err_not_a_constant_identifier);

            break;

        case tc_char:
        case tc_string:
            if (sign != tc_dummy) cx_error(err_invalid_constant);

            if (strlen(p_token->string__()) != 3) {
                cx_error(err_invalid_subrange_type);
            }

            limit = p_token->string__()[1];
            p_type = p_char_type;
            break;

        default:
            cx_error(err_missing_constant);
            return p_type;
    }

    get_token_append();
    return p_type;
}

