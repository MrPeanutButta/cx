#include "common.h"
#include "parser.h"

/** parse_formal_parm_list     parse a formal parameter list:
 *
 *                              ( <type-id> <id-list> ); or {
 * 
 * @param count
 * @param total_size
 * @return 
 */
cx_symtab_node *cx_parser::parse_formal_parm_list(int &count, int &total_size) {

    cx_symtab_node *pParmId; // ptrs to parm symtab nodes
    cx_symtab_node *pFirstId, *pLastId;
    cx_symtab_node *pPrevSublistLastId = nullptr;
    cx_symtab_node *pParmList = nullptr; // ptr to list of parm nodes
    cx_define_code parmDefn; // how a parm is defined

    cx_symtab_node *p_node = nullptr;
    count = total_size = 0;

    // Loop to parse the comma-separated sublist of parameter ids.
    cx_type *pParmType; // ptr to parm's type object
    while (token == tc_identifier) {


        // find param type
        p_node = find(p_token->string__());
        //icode.put(p_node);

        if (p_node->defn.how != dc_type) {
            cx_error(err_invalid_type);
        }

        pParmType = p_node->p_type;
        // get param name
        get_token();

        pFirstId = nullptr;

        // Reference or value parameter?
        if (token == tc_bit_AND) {
            parmDefn = dc_reference;
            get_token_append();
        } else parmDefn = dc_value_parm;

        pParmId = enter_new_local(p_token->string__(), parmDefn);
        icode.put(pParmId);

        ++count;
        if (!pParmList) pParmList = pParmId;

        // Link the parm id nodes together.
        if (!pFirstId) pFirstId = pLastId = pParmId;
        else {
            pLastId->next__ = pParmId;
            pLastId = pParmId;
        }

        //  ,
        get_token_append();
        resync(tokenlist_identifier_follow);
        if (token == tc_comma) {

            // Saw comma.
            // Skip extra commas and look for an identifier.
            do {
                get_token_append();
                resync(tokenlist_identifier_start, tokenlist_identifier_follow);
                if (token == tc_comma) {
                    cx_error(err_missing_identifier);
                }
            } while (token == tc_comma);
            if (token != tc_identifier) {
                cx_error(err_missing_identifier);
            }
        } else if (token == tc_identifier) cx_error(err_missing_comma);



        // Loop to assign the offset and type to each
        // parm id in the sublist.
        for (pParmId = pFirstId; pParmId; pParmId = pParmId->next__) {
            pParmId->defn.data.offset = total_size++;
            SetType(pParmId->p_type, pParmType);
        }

        // Link this sublist to the previous sublist.
        if (pPrevSublistLastId) pPrevSublistLastId->next__ = pFirstId;
        pPrevSublistLastId = pLastId;

        //  ; or )
        // resync(tlFormalParmsFollow, tokenlist_declaration_follow);

    }

    if (token == tc_identifier) {//|| (token == tcVAR)) {
        cx_error(err_missing_semicolon);
    } else while (token == tc_semicolon) get_token_append();
    //  :
    //resync(tokenlist_sublist_follow, tokenlist_declaration_follow);
    //conditional_get_token(tc_colon, err_missing_colon);

    return pParmList;
}

/** parse_subroutine_call     parse a call to a declared or a
 *                          standard procedure or function.
 * 
 * @param p_function_id    : ptr to routine id's symbol table node.
 * @param parm_check_flag : true to check parameter, false not to
 * @return ptr to the subroutine's type object
 */
cx_type *cx_parser::parse_subroutine_call(const cx_symtab_node *p_function_id,
        bool parm_check_flag) {
    //get_token_append();

    return (p_function_id->defn.routine.which == rc_declared) ||
            (p_function_id->defn.routine.which == rc_forward)
            ||
            !parm_check_flag
            ? parse_declared_subroutine_call(p_function_id, parm_check_flag)
            : parse_declared_subroutine_call(p_function_id, parm_check_flag);
}

/** parse_declared_subroutine_call parse a call to a declared
 *                              procedure or function.
 * 
 * @param p_function_id    : ptr to subroutine id's symbol table node.
 * @param parm_check_flag : true to check parameter, false not to.
 * @return ptr to the subroutine's type object.
 */
cx_type *cx_parser::parse_declared_subroutine_call
(const cx_symtab_node *p_function_id,
        int parm_check_flag) {
    parse_actual_parm_list(p_function_id, parm_check_flag);
    return p_function_id->p_type;
}

/** parse_actual_parm_list     parse an actual parameter list:
 *
 *                              ( <expr-list> )
 * 
 * @param p_function_id    : ptr to routine id's symbol table node.
 * @param parm_check_flag : true to check parameter, false not to.
 */
void cx_parser::parse_actual_parm_list(const cx_symtab_node *p_function_id,
        int parm_check_flag) {
    cx_symtab_node *p_formal_id = p_function_id ? p_function_id->defn.routine.
            locals.p_parms_ids
            : NULL;

    // If there are no actual parameters, there better not be
    // any formal parameters either.
    if (token != tc_left_paren) {
        if (parm_check_flag && p_formal_id) cx_error(err_wrong_number_of_parms);
        return;
    }

    // Loop to parse actual parameter expressions
    // separated by commas.
    do {
        //  ( or ,
        get_token_append();

        parse_actual_parm(p_formal_id, parm_check_flag);
        if (p_formal_id) p_formal_id = p_formal_id->next__;
    } while (token == tc_comma);

    //  )
    conditional_get_token_append(tc_right_paren, err_missing_right_paren);

    // There better not be any more formal parameters.
    if (parm_check_flag && p_formal_id) cx_error(err_wrong_number_of_parms);
}

/** parse_actual_parm     parse an actual parameter.  Make sure it
 *                      matches the corresponding formal parm.
 * 
 * @param p_formal_id     : ptr to the corresponding formal parm
 *                        id's symbol table node
 * @param parm_check_flag : true to check parameter, false not to.
 */
void cx_parser::parse_actual_parm(const cx_symtab_node *p_formal_id,
        int parm_check_flag) {
    // If we're not checking the actual parameters against
    // the corresponding formal parameters (as during error
    // recovery), just parse the actual parameter.
    if (!parm_check_flag) {
        parse_expression();
        return;
    }

    // If we've already run out of formal parameter,
    // we have an error.  Go into error recovery mode and
    // parse the actual parameter anyway.
    if (!p_formal_id) {
        cx_error(err_wrong_number_of_parms);
        parse_expression();
        return;
    }

    // Formal value parameter: The actual parameter can be an
    //                         arbitrary expression that is
    //                         assignment type compatible with
    //                         the formal parameter.
    if (p_formal_id->defn.how == dc_value_parm) {
        CheckAssignmentTypeCompatible(p_formal_id->p_type,
                parse_expression(),
                err_incompatible_types);
    }// Formal VAR parameter: The actual parameter must be a
        //                       variable of the same type as the
        //                       formal parameter.
    else if (token == tc_identifier) {
        cx_symtab_node *pActualId = find(p_token->string__());

        // skip type declaration
        if (pActualId->defn.how == ::dc_type) {
            get_token();

            if (token == tc_bit_AND)get_token();

            pActualId = find(p_token->string__());
        }

        icode.put(pActualId);

        get_token_append();
        if (p_formal_id->p_type != parse_variable(pActualId)) {
            cx_error(err_incompatible_types);
        }
        resync(tokenlist_expression_follow, tokenlist_statement_follow, tokenlist_statement_start);
    }// cx_error: parse the actual parameter anyway for error recovery.
    else {
        parse_expression();
        cx_error(err_invalid_reference);
    }


}
