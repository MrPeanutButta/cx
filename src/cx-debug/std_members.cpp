#include <sstream>
#include "std_members.h"
#include "types.h"
#include "symtable.h"

cx_symtab *std_members = nullptr;

void init_std_members(void){
    
            // allocate std member functions for basic types
	std_members = new cx_symtab;

	p_integer_type->complex.p_class_scope = std_members;
	p_uint8_type->complex.p_class_scope = std_members;
	p_float_type->complex.p_class_scope = std_members;
	p_boolean_type->complex.p_class_scope = std_members;
	p_char_type->complex.p_class_scope = std_members;
	p_wchar_type->complex.p_class_scope = std_members;

	cx_symtab_node *p_each_id = std_members->enter("each", dc_function);
	p_each_id->defn.routine.std_member = &cx_std_members::each;
	p_each_id->defn.routine.parm_count = 1;
	p_each_id->defn.routine.total_parm_size = 0;
	p_each_id->defn.routine.locals.p_parms_ids = nullptr;
	p_each_id->defn.routine.locals.p_constant_ids = nullptr;
	p_each_id->defn.routine.locals.p_type_ids = nullptr;
	p_each_id->defn.routine.locals.p_variable_ids = nullptr;
	p_each_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_size_id = std_members->enter("size", dc_function);
    p_size_id->defn.routine.std_member = &cx_std_members::size;
	p_size_id->defn.routine.parm_count = 0;
	p_size_id->defn.routine.total_parm_size = 0;
	p_size_id->defn.routine.locals.p_parms_ids = nullptr;
	p_size_id->defn.routine.locals.p_constant_ids = nullptr;
	p_size_id->defn.routine.locals.p_type_ids = nullptr;
	p_size_id->defn.routine.locals.p_variable_ids = nullptr;
	p_size_id->defn.routine.locals.p_function_ids = nullptr;

	// array length
	cx_symtab_node *p_length_id = std_members->enter("length", dc_function);
	p_length_id->defn.routine.std_member = &cx_std_members::length;
	p_length_id->defn.routine.parm_count = 0;
	p_length_id->defn.routine.total_parm_size = 0;
	p_length_id->defn.routine.locals.p_parms_ids = nullptr;
	p_length_id->defn.routine.locals.p_constant_ids = nullptr;
	p_length_id->defn.routine.locals.p_type_ids = nullptr;
	p_length_id->defn.routine.locals.p_variable_ids = nullptr;
	p_length_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_tostr_id = std_members->enter("to_str", dc_function);
	p_tostr_id->defn.routine.std_member = &cx_std_members::to_str;
	p_tostr_id->defn.routine.parm_count = 0;
	p_tostr_id->defn.routine.total_parm_size = 0;
	p_tostr_id->defn.routine.locals.p_parms_ids = nullptr;
	p_tostr_id->defn.routine.locals.p_constant_ids = nullptr;
	p_tostr_id->defn.routine.locals.p_type_ids = nullptr;
	p_tostr_id->defn.routine.locals.p_variable_ids = nullptr;
	p_tostr_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_towstr_id = std_members->enter("to_wstr", dc_function);
	p_towstr_id->defn.routine.std_member = &cx_std_members::to_wstr;
	p_towstr_id->defn.routine.parm_count = 0;
	p_towstr_id->defn.routine.total_parm_size = 0;
	p_towstr_id->defn.routine.locals.p_parms_ids = nullptr;
	p_towstr_id->defn.routine.locals.p_constant_ids = nullptr;
	p_towstr_id->defn.routine.locals.p_type_ids = nullptr;
	p_towstr_id->defn.routine.locals.p_variable_ids = nullptr;
	p_towstr_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_toint_id = std_members->enter("to_int", dc_function);
	p_toint_id->defn.routine.std_member = &cx_std_members::to_int;
	p_toint_id->defn.routine.parm_count = 0;
	p_toint_id->defn.routine.total_parm_size = 0;
	p_toint_id->defn.routine.locals.p_parms_ids = nullptr;
	p_toint_id->defn.routine.locals.p_constant_ids = nullptr;
	p_toint_id->defn.routine.locals.p_type_ids = nullptr;
	p_toint_id->defn.routine.locals.p_variable_ids = nullptr;
	p_toint_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_tochr_id = std_members->enter("to_chr", dc_function);
	p_tochr_id->defn.routine.std_member = &cx_std_members::to_chr;
	p_tochr_id->defn.routine.parm_count = 0;
	p_tochr_id->defn.routine.total_parm_size = 0;
	p_tochr_id->defn.routine.locals.p_parms_ids = nullptr;
	p_tochr_id->defn.routine.locals.p_constant_ids = nullptr;
	p_tochr_id->defn.routine.locals.p_type_ids = nullptr;
	p_tochr_id->defn.routine.locals.p_variable_ids = nullptr;
	p_tochr_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_toflt_id = std_members->enter("to_flt", dc_function);
	p_toflt_id->defn.routine.std_member = &cx_std_members::to_flt;
	p_toflt_id->defn.routine.parm_count = 0;
	p_toflt_id->defn.routine.total_parm_size = 0;
	p_toflt_id->defn.routine.locals.p_parms_ids = nullptr;
	p_toflt_id->defn.routine.locals.p_constant_ids = nullptr;
	p_toflt_id->defn.routine.locals.p_type_ids = nullptr;
	p_toflt_id->defn.routine.locals.p_variable_ids = nullptr;
	p_toflt_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_tobool_id = std_members->enter("to_bool", dc_function);
	p_tobool_id->defn.routine.std_member = &cx_std_members::to_bool;
	p_tobool_id->defn.routine.parm_count = 0;
	p_tobool_id->defn.routine.total_parm_size = 0;
	p_tobool_id->defn.routine.locals.p_parms_ids = nullptr;
	p_tobool_id->defn.routine.locals.p_constant_ids = nullptr;
	p_tobool_id->defn.routine.locals.p_type_ids = nullptr;
	p_tobool_id->defn.routine.locals.p_variable_ids = nullptr;
	p_tobool_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_towchr_id = std_members->enter("to_wchr", dc_function);
	p_towchr_id->defn.routine.std_member = &cx_std_members::to_wchr;
	p_towchr_id->defn.routine.parm_count = 0;
	p_towchr_id->defn.routine.total_parm_size = 0;
	p_towchr_id->defn.routine.locals.p_parms_ids = nullptr;
	p_towchr_id->defn.routine.locals.p_constant_ids = nullptr;
	p_towchr_id->defn.routine.locals.p_type_ids = nullptr;
	p_towchr_id->defn.routine.locals.p_variable_ids = nullptr;
	p_towchr_id->defn.routine.locals.p_function_ids = nullptr;

    cx_symtab_node *p_tobyte_id = std_members->enter("to_byte", dc_function);
	p_tobyte_id->defn.routine.std_member = &cx_std_members::to_byte;
	p_tobyte_id->defn.routine.parm_count = 0;
	p_tobyte_id->defn.routine.total_parm_size = 0;
	p_tobyte_id->defn.routine.locals.p_parms_ids = nullptr;
	p_tobyte_id->defn.routine.locals.p_constant_ids = nullptr;
	p_tobyte_id->defn.routine.locals.p_type_ids = nullptr;
	p_tobyte_id->defn.routine.locals.p_variable_ids = nullptr;
	p_tobyte_id->defn.routine.locals.p_function_ids = nullptr;

	///////
	p_each_id->defn.routine.which = func_std_iterator;
    p_size_id->defn.routine.which = func_std_member;
	p_length_id->defn.routine.which = func_std_member;
    p_tostr_id->defn.routine.which = func_std_member;
    p_towstr_id->defn.routine.which = func_std_member;
    p_toint_id->defn.routine.which = func_std_member;
    p_tochr_id->defn.routine.which = func_std_member;
    p_toflt_id->defn.routine.which = func_std_member;
    p_tobool_id->defn.routine.which = func_std_member;
    p_towchr_id->defn.routine.which = func_std_member;
    p_tobyte_id->defn.routine.which = func_std_member;
    
    // set std member return types
	set_type(p_each_id->p_type, p_integer_type);
    set_type(p_size_id->p_type, p_integer_type);
	set_type(p_length_id->p_type, p_integer_type);

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

cx_type *cx_std_members::length(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	cx->pop();
	cx_stack_item *p_length_val = new cx_stack_item;

	p_type->form == fc_array ?
		p_length_val->basic_types.int__ = p_type->array.element_count :
		p_length_val->basic_types.int__ = 1;

	cx->push((void *) p_length_val);

	return cx_function_id->p_type;
}


/** @TODO - needs to be fixed for wide char
 */
cx_type *cx_std_members::to_str(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	std::stringstream ss;
	ss.clear();
	cx_stack_item *mem = (cx_stack_item *)cx->top()->basic_types.addr__;

	switch (p_type->type_code){
	case cx_int:
		ss << mem->basic_types.int__ << '\0';
		break;
	case cx_char:
		ss << mem->basic_types.char__ << '\0';
		break;
	case cx_wchar:
		ss << mem->basic_types.wchar__ << '\0';
		break;
	case cx_float:
		ss << mem->basic_types.float__ << '\0';
		break;
	case cx_bool:
		ss << (mem->basic_types.bool__ ? "true" : "false") << '\0';
		break;
	case cx_uint8:
		ss << (int)mem->basic_types.uint8__ << '\0';
		break;
	default:break;
	}

	cx->pop();
	
	const int size = ss.str().size() - 1;
	const int length = ss.str().length() - 1;

	cx_type *p_str = new cx_type(fc_array, size, nullptr);
	set_type(p_str->array.p_element_type, p_char_type);
	p_str->array.element_count = length;
	p_str->array.max_index = length;

	char *p_to_str = (char *)malloc(size + 1);
	memcpy(p_to_str, ss.str().c_str(), size + 1);

	cx->push((void *)p_to_str);

	return p_str;
}

cx_type *cx_std_members::to_wstr(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

cx_type *cx_std_members::to_int(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

cx_type *cx_std_members::to_chr(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

cx_type *cx_std_members::to_flt(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

cx_type *cx_std_members::to_bool(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

cx_type *cx_std_members::to_wchr(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

cx_type *cx_std_members::to_byte(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

cx_type *cx_std_members::each(cx_executor *cx,
	const cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}