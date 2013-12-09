#include "std_members.h"
#include "types.h"
#include "symtable.h"

void init_std_members(void){
    
            // allocate std member functions for basic types
    cx_symtab *std_members = new cx_symtab;
    p_integer_type->complex.p_class_scope = std_members;
    p_uint8_type->complex.p_class_scope = std_members;
    p_float_type->complex.p_class_scope = std_members;
    p_boolean_type->complex.p_class_scope = std_members;
    p_char_type->complex.p_class_scope = std_members;
    p_wchar_type->complex.p_class_scope = std_members;

    cx_symtab_node *p_size_id = std_members->enter("size", dc_function);
    p_size_id->defn.routine.std_member = &cx_std_members::size;
    
    cx_symtab_node *p_tostr_id = std_members->enter("to_str", dc_function);
    cx_symtab_node *p_towstr_id = std_members->enter("to_wstr", dc_function);
    cx_symtab_node *p_toint_id = std_members->enter("to_int", dc_function);
    cx_symtab_node *p_tochr_id = std_members->enter("to_chr", dc_function);
    cx_symtab_node *p_toflt_id = std_members->enter("to_flt", dc_function);
    cx_symtab_node *p_tobool_id = std_members->enter("to_bool", dc_function);
    cx_symtab_node *p_towchr_id = std_members->enter("to_wchr", dc_function);
    cx_symtab_node *p_tobyte_id = std_members->enter("to_byte", dc_function);
    
    p_size_id->defn.routine.which = func_std_member;
    p_tostr_id->defn.routine.which = func_std_member;
    p_towstr_id->defn.routine.which = func_std_member;
    p_toint_id->defn.routine.which = func_std_member;
    p_tochr_id->defn.routine.which = func_std_member;
    p_toflt_id->defn.routine.which = func_std_member;
    p_tobool_id->defn.routine.which = func_std_member;
    p_towchr_id->defn.routine.which = func_std_member;
    p_tobyte_id->defn.routine.which = func_std_member;
    
    // set std member return types
    set_type(p_size_id->p_type, p_integer_type);
    set_type(p_tostr_id->p_type, new cx_type(fc_array, 0, nullptr));
    set_type(p_tostr_id->p_type->array.p_element_type, p_char_type);
    set_type(p_toint_id->p_type, p_integer_type);
    set_type(p_tochr_id->p_type, p_char_type);
    set_type(p_toflt_id->p_type, p_float_type);
    set_type(p_tobool_id->p_type, p_boolean_type);
    set_type(p_towchr_id->p_type, p_wchar_type);
    set_type(p_tobyte_id->p_type, p_uint8_type);

}

cx_type *cx_std_members::size(cx_executor* cx, 
                              const cx_symtab_node* cx_function_id, 
                              const cx_type *p_type){
    cx->pop();
    cx_stack_item *p_size_val = new cx_stack_item;
    p_size_val->basic_types.int__ =  p_type->size;
    cx->push((void*) p_size_val);
    
    return cx_function_id->p_type;
}