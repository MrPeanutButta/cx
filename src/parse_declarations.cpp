/** parse Declarations
 * parse_declarations.cpp
 */

#include <cstdio>
#include <cstring>
#include "common.h"
#include "parser.h"

// might not always be true in some cases.
bool execFlag(true);

cx_symtab_node *pProgram_ptr = nullptr;

/** parse_declarations_or_assignment       Parses new declarations or 
 *                                      assignment statements.
 * 
 * NOTE:
 *      This should be broken up a bit. Function, complex, and type declaraions
 *      should be seperated.
 * 
 * @param p_function_id : ptr to the routine which owns the type being declared or
 *                     assigned a value.
 */
void cx_parser::parse_declarations_or_assignment(cx_symtab_node *p_function_id) {

    if (!pProgram_ptr->found_global_end) {
        pProgram_ptr->global_finish_location = icode.current_location();
    }

    cx_symtab_node *p_node = find(p_token->string__());

    // if complex then this is an object
    if (p_node->p_type->form == fcComplex) {
        parse_complex_type(p_function_id, p_node);
        // predefined type name found
    } else if ((p_node->defn.how == dc_type) && (p_node->p_type->form != fcComplex) &&
            (p_node->defn.how != dc_function) && (p_node->p_type->form != fcArray)) {

        get_token();

        do {
            while (token == tc_comma)get_token_append();

            cx_symtab_node *pNewId = nullptr;

            pNewId = search_local(p_token->string__());

            /* if not nullptr, it's already defined.
             * check if forwarded */
            if (pNewId != nullptr) {
                if (pNewId->defn.how == dc_function && pNewId->defn.routine.which == ::rc_forward) {
                    get_token_append();
                    parse_function_header(pNewId);
                } else cx_error(err_redefined_identifier);
            } else {
                pNewId = enter_new_local(p_token->string__());
                icode.put(pNewId);
            }

            // set type
            SetType(pNewId->p_type, p_node->p_type);

            get_token_append();

            // check for array type
            if (token == tc_left_subscript) {
                parse_array_type(p_function_id, pNewId);

            } else if (token == tc_left_paren) {

                parse_function_header(pNewId);
            } else if ((token != tc_comma) && (token != tc_end_of_file)) {

                // check for assignment
                parse_assignment(pNewId);
                pNewId->defn.how = dc_variable;
            }

            if (pNewId->defn.how == dc_variable) {
                // add variable to variable list
                if (p_function_id) {
                    cx_symtab_node *__var = p_function_id->defn.routine.locals.p_variable_ids;
                    if (!__var) {
                        p_function_id->defn.routine.locals.p_variable_ids = pNewId;
                        p_function_id->defn.routine.total_local_size += pNewId->p_type->size;
                    } else {
                        while (__var->next__)__var = __var->next__;

                        __var->next__ = pNewId;
                        p_function_id->defn.routine.total_local_size += pNewId->p_type->size;
                    }
                }
                // add function to routine list
            } else if (pNewId->defn.how == dc_function) {
                if (p_function_id) {
                    cx_symtab_node *__fun = p_function_id->defn.routine.locals.p_function_ids;
                    if (!__fun) {
                        p_function_id->defn.routine.locals.p_function_ids = pNewId;
                    } else {
                        while (__fun->next__)__fun = __fun->next__;

                        __fun->next__ = pNewId;
                    }
                }
            }

        } while (token == tc_comma);
    } else if (p_node->defn.how == dc_function) {
        icode.put(p_node);

        get_token_append();

        if (token == tc_left_paren) {
            parse_subroutine_call(p_node, true);
        } else {
            parse_assignment(p_node);
        }

    } else {
        icode.put(p_node);
        get_token_append();
        parse_assignment(p_node);
    }
}

/** parse_constant_declaration    'const' will only set it's qualifier as 
 *                              dc_constant all else is treated as a standard
 *                              declaration.
 *      const <type> <name>;
 *
 * @param p_function_id    ptr to the routine which owns the type being declared or
 *                      assigned a constant value.
 */
void cx_parser::parse_constant_declaration(cx_symtab_node* p_function_id) {
    cx_symtab_node *pLastId = nullptr;
    cx_symtab_node *p_const_id = nullptr;
    cx_symtab_node *pTypeNode = find(p_token->string__());

    get_token_append();

    p_const_id = enter_new_local(p_token->string__());

    if (!p_function_id->defn.routine.locals.p_constant_ids) {
        p_function_id->defn.routine.locals.p_constant_ids = p_const_id;
    } else {

        pLastId = p_function_id->defn.routine.locals.p_constant_ids;

        while (pLastId->next__)
            pLastId = pLastId->next__;

        pLastId->next__ = p_const_id;

    }

    get_token_append();
    conditional_get_token(tc_equal, err_missing_equal);

    SetType(p_const_id->p_type, pTypeNode->p_type);
    parse_constant(p_const_id);
    p_const_id->defn.how = dc_constant;


    resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);

}

/** parse_constant       parse a constant.
 * 
 * @param p_const_id : ptr to symbol table node of the identifier
 *                   being defined
 */
void cx_parser::parse_constant(cx_symtab_node *p_const_id) {
    cx_token_code sign = tc_dummy;

    if (token_in(token, tokenlist_unary_ops)) {
        if (token == tc_minus) sign = tc_minus;
        get_token_append();
    }

    switch (token) {
        case tc_number:
            if ((p_token->type() == ty_integer) && (p_const_id->p_type == pIntegerType)) {
                p_const_id->defn.constant.value.int__ = sign == tc_minus ?
                        -p_token->value().int__ : p_token->value().int__;
            } else if ((p_token->type() == ty_real) &&
                    (((p_const_id->p_type == pFloatType)))) {

                if (p_const_id->p_type == pFloatType) {
                    p_const_id->defn.constant.value.float__ = sign == tc_minus ?
                            -p_token->value().float__ : p_token->value().float__;
                } else {
                    p_const_id->defn.constant.value.double__ = sign == tc_minus ?
                            -p_token->value().float__ : p_token->value().float__;
                }
            }

            get_token_append();
            break;

        case tc_identifier:
            parse_identifier_constant(p_const_id, sign);
            break;
        case tc_char:
        case tc_string:
            if (p_const_id->p_type == pCharType) {
                int length = strlen(p_token->string__()) - 2;

                if (sign != tc_dummy) cx_error(err_invalid_constant);

                if (length == 1) {
                    p_const_id->defn.constant.value.char__ = p_token->string__()[1];

                    //SetType(p_const_id->p_type, pCharType);
                } else {
                    char *p_string = new char[length];
                    copy_quoted_string(p_string, p_token->string__());

                    p_const_id->defn.constant.value.p_string = p_string;

                    //SetType(p_const_id->p_type, new cx_type(length));
                }

                get_token_append();
            } else cx_error(err_invalid_type);
            break;
    }
}

/** parse_identifier_constant     In a constant definition of the
 *                              form
 *
 *                                      <id-1> = <id-2>
 *
 *                              parse <id-2>. The type can be
 *                              integer, real, character,
 *                              enumeration, or string
 *                              (character array).
 * 
 * @param p_id1 : ptr to symbol table node of <id-1>.
 * @param sign : unary + or - sign, or none.
 */
void cx_parser::parse_identifier_constant(cx_symtab_node* p_id1, cx_token_code sign) {
    cx_symtab_node *p_id2 = find(p_token->string__());

    if (p_id2->defn.how != dc_constant) {
        cx_error(err_not_a_constant_identifier);
        SetType(p_id1->p_type, pDummyType);
        get_token_append();
        return;
    }

    if (p_id2->p_type == pIntegerType) {
        p_id2->defn.constant.value.int__ = sign == tc_minus ?
                -p_id2->defn.constant.value.int__ :
                p_id2->defn.constant.value.int__;

        SetType(p_id1->p_type, pIntegerType);
    } else if (p_id2->p_type == pFloatType) {
        p_id1->defn.constant.value.float__ = sign == tc_minus ?
                -p_id2->defn.constant.value.float__ :
                p_id2->defn.constant.value.float__;
        SetType(p_id1->p_type, pFloatType);
    } else if (p_id2->p_type == pCharType) {
        if (sign != tc_dummy) cx_error(err_invalid_constant);

        p_id1->defn.constant.value.char__ = p_id2->defn.constant.value.char__;

        SetType(p_id1->p_type, pCharType);
    } else if (p_id2->p_type->form == fcEnum) {
        if (sign != tc_dummy)cx_error(err_invalid_constant);

        p_id1->defn.constant.value.int__ = p_id2->defn.constant.value.int__;

        SetType(p_id1->p_type, p_id2->p_type);
    } else if (p_id2->p_type->form == fcArray) {
        if ((sign != tc_dummy) || (p_id2->p_type->array.pElmtType != pCharType)) {
            cx_error(err_invalid_constant);
        }

        p_id1->defn.constant.value.p_string = p_id2->defn.constant.value.p_string;

        SetType(p_id1->p_type, p_id2->p_type);
    }

    get_token_append();
}