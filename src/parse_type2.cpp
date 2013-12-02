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
cx_type *cx_parser::parse_array_type (cx_symtab_node *p_function_id,
                                      cx_symtab_node *p_array_node) {

    cx_type *p_array_type = new cx_type(fc_array, 0, nullptr);
    cx_type *p_element_type = p_array_type;

    // Final element type.
    set_type(p_element_type->array.p_element_type, p_array_node->p_type);

    conditional_get_token(tc_left_subscript, err_missing_left_subscript);

    int min_index = 0;
    int max_index = 0;

    if (token == tc_number) {
        max_index = p_token->value().int__;
        get_token();

        set_type(p_element_type->array.p_index_type, p_integer_type);

    } else {
        cx_type *p_index_type = parse_expression();

        check_assignment_type_compatible(p_integer_type, p_index_type,
                                         err_invalid_index_type);

        set_type(p_element_type->array.p_index_type, p_index_type);
    }

    p_array_type->array.element_count = max_index;
    p_array_type->array.min_index = min_index;
    p_array_type->array.max_index = max_index;
    p_array_type->size = min_index * p_array_type->array.p_element_type->size;

    conditional_get_token(tc_right_subscript, err_missing_right_subscript);

    if (token_in(token, tokenlist_assign_ops))parse_assignment(p_array_node);

    set_type(p_array_node->p_type, p_array_type);

    if (p_array_node->defn.how == dc_undefined) {
        p_array_node->defn.how = dc_variable;
    }

    // If the type object doesn't have a name yet,
    // point it to the type id.
    if (!p_array_node->p_type->p_type_id) {
        p_array_node->p_type->p_type_id = p_array_node;
    }

    return p_array_type;
}

/** parse_unksize_array_type      parse an array type specification
 *                                of unknown size.
 *
 *      <type-id> *<id>;
 *
 * @param p_function_id : ptr to fuction which owns this array.
 * @param p_array_node : ptr to array symtab node.
 * @return ptr to array type object.
 */
cx_type *cx_parser::parse_unksize_array_type (cx_symtab_node* p_function_id,
                                              cx_symtab_node* p_array_node) {

    cx_type *p_array_type = new cx_type(fc_array, 0, nullptr);
    cx_type *p_expr_type = nullptr;

    // Final element type.
    set_type(p_array_type->array.p_element_type, p_array_node->p_type);
    p_array_type->type_code = p_array_node->p_type->type_code;

    bool is_function = false;
    const bool is_expression = token_in(token, tokenlist_assign_ops);

    if ((token != tc_left_paren) && (token != tc_right_paren) &&
        (!is_expression)) get_token_append();
    else if ((token != tc_right_paren) && (!is_expression)) is_function = true;

    int min_index = 0;
    int max_index = 0;

    set_type(p_array_type->array.p_index_type, p_integer_type);
    p_array_type->array.element_count = max_index;
    p_array_type->array.min_index = min_index;
    p_array_type->array.max_index = max_index;

    if (is_function) {
        set_type(p_array_node->p_type, p_array_type);
        p_array_node->p_type->form = fc_array;
        p_array_node->p_type->p_type_id = p_array_node;
        parse_function_header(p_array_node);

        return p_array_type;
    }

    if (is_expression) {

        //p_expr_type = parse_assignment(p_array_node);

        delete p_array_type;

        set_type(p_array_node->p_type, parse_assignment(p_array_node));
        p_array_node->p_type->form = fc_array;
        //set_type(p_array_node->p_type->array.p_index_type, p_integer_type);
        //set_type(p_array_node->p_type->array.p_element_type, p_expr_type->base_type());
        p_array_node->p_type->p_type_id = p_array_node;
        p_array_type = p_array_node->p_type;

    } else {
        set_type(p_array_node->p_type, p_array_type);
    }

    if (p_array_node->defn.how == dc_undefined) {
        p_array_node->defn.how = dc_variable;
    }

    /* If the type object doesn't have a name yet,
     * point it to the type id. */
    if (!p_array_node->p_type->p_type_id) {
        p_array_node->p_type->p_type_id = p_array_node;
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
int cx_parser::array_size (cx_type * p_array_type) {
    if (p_array_type->array.p_element_type->size == 0) {
        p_array_type->array.p_element_type->size = array_size
                (p_array_type->array.p_element_type);
    }

    return (p_array_type->array.element_count *
            p_array_type->array.p_element_type->size);
}

///** parse_string_type      parse an string type specification.
// *
// *      <type-id> <id> [ <const-index-size> ];
// *
// * @param p_function_id : ptr to fuction which owns this array.
// * @param p_array_node : ptr to string symtab node.
// * @return ptr to array type object.
// */
//cx_type *cx_parser::parse_string_type(cx_symtab_node* p_function_id,
//        cx_symtab_node* p_string_node) {
//
//    cx_type *p_array_type = new cx_type(fc_array, 0, nullptr);
//    cx_type *p_element_type = p_array_type;
//
//    // Final element type.
//    set_type(p_element_type->array.p_element_type, p_char_type);
//
//    int min_index = 0;
//    int max_index = 0;
//
//    //set_type(p_element_type->array.p_index_type, p_integer_type);
//    p_array_type->array.element_count = max_index;
//    p_array_type->array.min_index = min_index;
//    p_array_type->array.max_index = max_index - 1;
//
//    set_type(p_string_node->p_type, p_array_type);
//
//    get_token_append();
//
//    if (token_in(token, tokenlist_assign_ops))parse_assignment(p_string_node);
//
//    p_string_node->defn.how = dc_variable;
//
//    //add to routines variable list
//    /*if (p_function_id != nullptr) {
//        cx_symtab_node *array = p_function_id->defn.routine.locals.p_type_ids;
//        if (!array) {
//            p_function_id->defn.routine.locals.p_type_ids = p_string_node;
//        } else {
//            while (array->next__)array = array->next__;
//
//            array->next__ = p_string_node;
//        }
//    }*/
//
//    // If the type object doesn't have a name yet,
//    // point it to the type id.
//    if (!p_string_node->p_type->p_type_id) {
//        p_string_node->p_type->p_type_id = p_string_node;
//    }
//
//    return p_array_type;
//
//}

/** parse_complex_type
 *
 * NOTE:
 *      Not implemented.
 *
 * @param p_function_id
 * @param p_node
 * @return
 */
cx_type * cx_parser::parse_complex_type (cx_symtab_node *p_function_id, cx_symtab_node * p_node) {

    get_token();

    // <id>
    cx_type *p_type = new cx_type(fc_complex, 0, nullptr);

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
        cx_symtab_node *p_var_id = p_function_id->defn.routine.locals.p_type_ids;

        while (p_var_id->next__)
            p_var_id = p_var_id->next__;

        p_var_id->next__ = p_id;
    }

    set_type(p_id->p_type, p_type);
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
void cx_parser::parse_member_decls (cx_symtab_node *p_function_id, cx_type *p_complex_type, int offset) {
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

    cx_symtab_node *p_last_id = nullptr; // ptrs to symtab nodes
    cx_symtab_node *p_first_id = nullptr; // ptr to last node of previous sublist

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
                ///p_first_id = p_complex_type->complex.MemberTable[scope]->root();

                offset = 0;
                while (p_first_id) {
                    offset += p_first_id->p_type->size;
                    p_first_id = p_first_id->next__;
                }

                break;
            case tc_PRIVATE:
                scope = tc_PRIVATE;
                get_token();
                conditional_get_token(tc_colon, err_missing_colon);
                //p_first_id = p_complex_type->complex.MemberTable[scope]->root();

                offset = 0;
                while (p_first_id) {
                    offset += p_first_id->p_type->size;
                    p_first_id = p_first_id->next__;
                }

                break;
            case tc_PROTECTED:
                scope = tc_PROTECTED;
                get_token();
                conditional_get_token(tc_colon, err_missing_colon);
                // p_first_id = p_complex_type->complex.MemberTable[scope]->root();

                offset = 0;
                while (p_first_id) {
                    offset += p_first_id->p_type->size;
                    p_first_id = p_first_id->next__;
                }

                break;
            default:
                break;
        }

        //member_table = p_complex_type->complex.MemberTable[scope];

        // find our declared type
        p_node = find(p_token->string__());

        // if complex then this is an object
        if (p_node->p_type->form == fc_complex) {
            parse_complex_type(p_function_id, p_node);
            // predefined type name found
        } else if ((p_node->defn.how == dc_type) && (p_node->p_type->form != fc_complex)) {
            do {
                get_token();

                // enter new local
                member = member_table->enter_new(p_token->string__());
                member->defn.how = dc_member;

                // set type
                set_type(member->p_type, p_node->p_type);

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
                    //parse_assignment(p_new_id);
                    member->defn.how = dc_variable;
                }

                if (!p_first_id) p_first_id = p_last_id = member;
                else {
                    p_last_id->next__ = member;
                    p_last_id = member;
                }

            } while (token == tc_comma);

            conditional_get_token(tc_semicolon, err_missing_semicolon);
        }



    } while (token != tc_right_bracket);

    // connect all symtabs for use within the class
    p_complex_type->complex.p_class_scope_symtab = new cx_symtab;
    // p_complex_type->complex.p_class_scope_symtab->connect_tables(p_complex_type->complex.MemberTable);

    conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);
    conditional_get_token_append(tc_semicolon, err_missing_semicolon);
}