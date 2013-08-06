#include "common.h"
#include "parser.h"

/** parse_array_type      parse an array type specification.
 * 
 *      <type-id> <id> [ <const-index-size> ]; 
 * 
 * @param p_function_id : ptr to fuction which owns this array.
 * @param p_array_node : ptr to array symtab node.
 * @return ptr to array type object.
 */
cx_type *cx_parser::parse_array_type(cx_symtab_node *p_function_id, cx_symtab_node *p_array_node) {
    cx_type *p_array_type = new cx_type(fcArray, 0, nullptr);
    cx_type *pElmtType = p_array_type;

    // Final element type.
    SetType(pElmtType->array.pElmtType, p_array_node->p_type);

    conditional_get_token_append(tc_left_subscript, err_missing_left_subscript);


    if (token == tc_right_subscript) {
        // xxx fixme, need a way to get out of assignment
        parse_assignment(p_array_node);
    } else {
        int min_index = 0;
        int max_index = p_token->value().int__;
        SetType(pElmtType->array.pIndexType, pIntegerType);
        p_array_type->array.elmtCount = max_index;
        p_array_type->array.minIndex = min_index;
        p_array_type->array.maxIndex = max_index - 1;

        parse_expression();
    }

    if (p_array_type->form != fcNone) {
        p_array_type->size = array_size(p_array_type);
    }

    conditional_get_token_append(tc_right_subscript, err_missing_right_subscript);

    //p_array_type->array.pElmtType->size = array_size(p_array_type);
    SetType(p_array_node->p_type, p_array_type);
    p_array_node->defn.how = dc_variable;

    //add to routines variable list
    if (p_function_id) {
        cx_symtab_node *__array = p_function_id->defn.routine.locals.p_type_ids;
        if (!__array) {
            p_function_id->defn.routine.locals.p_type_ids = p_array_node;
        } else {
            while (__array->next__)__array = __array->next__;

            __array->next__ = p_array_node;
        }
    }

    // If the type object doesn't have a name yet,
    // point it to the type id.
    if (!p_array_node->p_type->pTypeId) {
        p_array_node->p_type->pTypeId = p_array_node;
    }

    return p_array_type;
}

/** array_size           Calculate the total byte size of an
 *                      array type by recursively calculating
 *                      the size of each dimension.
 * 
 * @param p_array_type : ptr to array type object.
 * @return byte size.
 */
int cx_parser::array_size(cx_type * p_array_type) {
    if (p_array_type->array.pElmtType->size == 0) {
        p_array_type->array.pElmtType->size = array_size(p_array_type->array.pElmtType);
    }

    return (p_array_type->array.elmtCount * p_array_type->array.pElmtType->size);
}

/** parse_complex_type
 * 
 * NOTE: 
 *      Not implemented.
 * 
 * @param p_function_id
 * @param p_node
 * @return 
 */
cx_type * cx_parser::parse_complex_type(cx_symtab_node *p_function_id, cx_symtab_node * p_node) {

    get_token();

    // <id>
    cx_type *p_type = new cx_type(fcComplex, 0, nullptr);

    //cx_symtab_node *next_type = p_node;

    //while (next_type->next__)
    //next_type = next_type->next__;

    cx_symtab_node *p_id = enter_new_local(p_token->string__());
    icode.put(p_id);

    //next_type->next__ = p_id;

    // Link the routine's local type id nodes together.
    if (!p_function_id->defn.routine.locals.p_type_ids) {
        p_function_id->defn.routine.locals.p_type_ids = p_id;
    } else {
        cx_symtab_node *__var = p_function_id->defn.routine.locals.p_type_ids;

        while (__var->next__)
            __var = __var->next__;

        __var->next__ = p_id;
    }

    SetType(p_id->p_type, p_type);
    p_id->defn.how = dc_type;

    parse_member_decls(p_function_id, p_id->p_type, 0);

    return p_id->p_type;
}

/** parse_member_decls
 * 
 * NOTE: 
 *      Not implemented.
 * 
 * @param p_function_id
 * @param p_complex_type
 * @param offset
 */
void cx_parser::parse_member_decls(cx_symtab_node *p_function_id, cx_type *p_complex_type, int offset) {
    // copy of base class would go here

    // if no '{' this must be a forward

    get_token();

    if (token == tc_semicolon) {
        get_token_append();
        return;
    }

    // if '{' this is a class body

    conditional_get_token_append(tc_left_bracket, err_missing_left_bracket);

    // scope
    cx_token_code scope;
    cx_symtab_node *member = nullptr;

    // pointer to scoped table
    cx_symtab *member_table = nullptr;

    cx_symtab_node *p_node = nullptr;

    cx_symtab_node *pLastId = nullptr; // ptrs to symtab nodes
    cx_symtab_node *pFirstId = nullptr; // ptr to last node of previous sublist

    //    p_complex_type->complex.MemberTable.insert(pair<cx_token_code, cx_symtab *>(tc_PUBLIC, new cx_symtab));
    //   p_complex_type->complex.MemberTable.insert(pair<cx_token_code, cx_symtab *>(tc_PRIVATE, new cx_symtab));
    //   p_complex_type->complex.MemberTable.insert(pair<cx_token_code, cx_symtab *>(tc_PROTECTED, new cx_symtab));

    //default to public
    scope = tc_PUBLIC;

    do {

        // check scope and fast forward list to the end
        switch (token) {
            case tc_PUBLIC:
                scope = tc_PUBLIC;
                get_token();
                conditional_get_token(tc_colon, err_missing_colon);
                ///pFirstId = p_complex_type->complex.MemberTable[scope]->root();

                offset = 0;
                while (pFirstId) {
                    offset += pFirstId->p_type->size;
                    pFirstId = pFirstId->next__;
                }

                break;
            case tc_PRIVATE:
                scope = tc_PRIVATE;
                get_token();
                conditional_get_token(tc_colon, err_missing_colon);
                //pFirstId = p_complex_type->complex.MemberTable[scope]->root();

                offset = 0;
                while (pFirstId) {
                    offset += pFirstId->p_type->size;
                    pFirstId = pFirstId->next__;
                }

                break;
            case tc_PROTECTED:
                scope = tc_PROTECTED;
                get_token();
                conditional_get_token(tc_colon, err_missing_colon);
                // pFirstId = p_complex_type->complex.MemberTable[scope]->root();

                offset = 0;
                while (pFirstId) {
                    offset += pFirstId->p_type->size;
                    pFirstId = pFirstId->next__;
                }

                break;
        }

        //member_table = p_complex_type->complex.MemberTable[scope];

        // find our declared type
        p_node = find(p_token->string__());

        // if complex then this is an object
        if (p_node->p_type->form == fcComplex) {
            parse_complex_type(p_function_id, p_node);
            // predefined type name found
        } else if ((p_node->defn.how == dc_type) && (p_node->p_type->form != fcComplex)) {
            do {
                get_token();

                // enter new local
                member = member_table->enter_new(p_token->string__());
                member->defn.how = dc_member;

                // set type
                SetType(member->p_type, p_node->p_type);

                // Record fields
                member->defn.data.offset = offset;
                offset += p_node->p_type->size;

                get_token_append();

                // check for array type
                // check for array type
                if (token == tc_left_subscript) {
                    //parse_array_type(member);
                    member->defn.how = dc_variable;
                } else if (token == tc_left_paren) {
                    parse_function_header(member);
                } else if (token != tc_comma) {
                    // check for assignment
                    //parse_assignment(pNewId);
                    member->defn.how = dc_variable;
                }

                if (!pFirstId) pFirstId = pLastId = member;
                else {
                    pLastId->next__ = member;
                    pLastId = member;
                }

            } while (token == tc_comma);

            conditional_get_token(tc_semicolon, err_missing_semicolon);
        }



    } while (token != tc_right_bracket);

    // connect all symtabs for use within the class
    p_complex_type->complex.pSymtabClassScope = new cx_symtab;
    // p_complex_type->complex.pSymtabClassScope->connect_tables(p_complex_type->complex.MemberTable);

    conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);
    conditional_get_token_append(tc_semicolon, err_missing_semicolon);
}