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
    cx_symtab_node *pLastId = nullptr;

    while (token == tc_identifier) {
        cx_symtab_node *pTypeId = enter_new_local(p_token->string__());

        if (!p_function_id->defn.routine.locals.p_type_ids) {
            p_function_id->defn.routine.locals.p_type_ids = pTypeId;
        } else {
            pLastId->next__ = pTypeId;
        }

        pLastId = pTypeId;

        get_token_append();
        conditional_get_token(tc_equal, err_missing_equal);

        //        SetType(pTypeId->p_type, parse_type_spec());
        pTypeId->defn.how = dc_type;

        if (!pTypeId->p_type->pTypeId) {
            pTypeId->p_type->pTypeId = pTypeId;
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
                    return (pDummyType);
            }
        }

        case tc_left_bracket: return ParseEnumerationType();

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
            return (pDummyType);

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

/** ParseEnumerationType    parse a enumeration type
 *                          specification:
 *      enum <id> { <enum-list> };
 *  
 * @return  
 */
cx_type *cx_parser::ParseEnumerationType(void) {
    cx_type *p_type = new cx_type(fcEnum, sizeof (int), nullptr);
    cx_symtab_node *pLastId = nullptr;

    int constValue = -1;

    get_token_append();
    resync(tokenlist_enum_const_start);

    while (token == tc_identifier) {
        cx_symtab_node *p_const_id = enter_new_local(p_token->string__());
        ++constValue;

        if (p_const_id->defn.how == dc_undefined) {
            p_const_id->defn.how = dc_constant;

            p_const_id->defn.constant.value.int__ = constValue;
            SetType(p_const_id->p_type, p_type);

            if (!pLastId) {
                p_type->enumeration.pConstIds = pLastId = p_const_id;
            } else {
                pLastId->next__ = p_const_id;
                pLastId = p_const_id;
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

    conditional_get_token(tc_right_bracket, err_missing_right_bracket);

    p_type->enumeration.max = constValue;
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
    cx_type *p_type = new cx_type(fcSubrange, 0, nullptr);

    SetType(p_type->subrange.pBaseType, parse_subrange_limit(p_min_id, p_type->subrange.min));

    resync(tokenlist_subrange_limit_follow, tokenlist_declaration_start);
    conditional_get_token(tc_dot_dot, errMissingDotDot);

    cx_type *pMaxType = parse_subrange_limit(nullptr, p_type->subrange.max);

    if (pMaxType != p_type->subrange.pBaseType) {
        cx_error(err_incompatible_types);
        p_type->subrange.min = p_type->subrange.max = 0;
    } else if (p_type->subrange.min > p_type->subrange.max) {
        cx_error(errMinGtMax);

        int temp = p_type->subrange.min;
        p_type->subrange.min = p_type->subrange.max;
        p_type->subrange.max = temp;
    }

    p_type->size = p_type->subrange.pBaseType->size;
    return p_type;
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
    cx_type *p_type = pDummyType;
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

                p_type = pIntegerType;

            } else cx_error(err_invalid_subrange_type);
            break;
        case tc_identifier:
            if (!p_limit_id) p_limit_id = find(p_token->string__());

            if (p_limit_id->defn.how == dc_undefined) {
                p_limit_id->defn.how = dc_constant;
                p_type = SetType(p_limit_id->p_type, pDummyType);
                break;
            } else if ((p_limit_id->p_type == pFloatType) ||
                    (p_limit_id->p_type == pDummyType) ||
                    (p_limit_id->p_type->form == fcArray)) {
                cx_error(err_invalid_subrange_type);
            } else if (p_limit_id->defn.how == dc_constant) {

                if (p_limit_id->p_type == pIntegerType) {
                    limit = sign == tc_minus
                            ? -p_limit_id->defn.constant.value.int__
                            : p_limit_id->defn.constant.value.int__;
                } else if (p_limit_id->p_type == pCharType) {
                    if (sign != tc_dummy) cx_error(err_invalid_constant);
                    limit = p_limit_id->defn.constant.value.char__;
                } else if (p_limit_id->p_type->form == fcEnum) {
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
            p_type = pCharType;
            break;

        default:
            cx_error(err_missing_constant);
            return p_type;
    }

    get_token_append();
    return p_type;
}

