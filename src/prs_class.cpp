#include "parser.h"
#include "common.h"

void inherit(cx_symtab_node *dst, const cx_symtab_node *src) {
	if (src->p_type->complex.p_class_scope == nullptr) return;

	src->p_type->complex.p_class_scope->copy_into(dst->p_type->complex.p_class_scope);
}

void cx_parser::parse_class(cx_symtab_node *p_function_id) {
    get_token();
    cx_symtab_node *p_class_id = search_local(p_token->string__());

    if (p_class_id == nullptr) {
        p_class_id = enter_local(p_token->string__(), dc_type);
        p_class_id->p_type = new cx_type(fc_complex, 0, p_class_id);
        p_class_id->p_type->complex.p_class_scope = new cx_symtab();

        p_class_id->defn.routine.locals.p_parms_ids = nullptr;
        p_class_id->defn.routine.locals.p_constant_ids = nullptr;
        p_class_id->defn.routine.locals.p_type_ids = nullptr;
        p_class_id->defn.routine.locals.p_variable_ids = nullptr;
        p_class_id->defn.routine.locals.p_function_ids = nullptr;

    } else if (p_class_id->defn.how != dc_type) {
        cx_error(err_invalid_class_def);
    }

    cx_symtab *p_old_symtab = (cx_symtab *) symtab_stack.get_current_symtab();

    symtab_stack.set_scope(++current_nesting_level);
    symtab_stack.set_current_symtab(p_class_id->p_type->complex.p_class_scope);

    get_token(); // class ID

    if (token == tc_colon) {
        cx_symtab_node *p_node = nullptr;
        
        
        
        //copy symtab into current class ID
        do {
            get_token();
            
            if(token != tc_identifier){
                cx_error(err_missing_identifier);
            } else {
                p_node = find(p_token->string__());
                
                if(p_node->defn.how != dc_type){
                    cx_error(err_invalid_type);
                }
            }
            
            inherit(p_class_id, p_node);
            
            get_token();
            // while token == comma copy trees
        } while (token == tc_comma);
    }

    conditional_get_token(tc_left_bracket, err_missing_left_bracket); // open bracket

    parse_statement_list(p_class_id, tc_right_bracket);

    conditional_get_token(tc_right_bracket, err_missing_right_bracket);
    conditional_get_token_append(tc_semicolon, err_missing_semicolon); 
    
    symtab_stack.set_scope(--current_nesting_level);
    symtab_stack.set_current_symtab(p_old_symtab);

    cx_symtab_node *p_var_id = p_function_id->defn.routine.locals.p_variable_ids;
    if (!p_var_id) {
        p_function_id->defn.routine.locals.p_variable_ids = p_class_id;
        p_function_id->defn.routine.total_local_size += p_class_id->p_type->size;
    } else {
        while (p_var_id->next__)p_var_id = p_var_id->next__;

        p_var_id->next__ = p_class_id;
        p_function_id->defn.routine.total_local_size += p_class_id->p_type->size;
    }
}

