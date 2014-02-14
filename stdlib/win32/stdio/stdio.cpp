// stdio.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
#include "cx_api.h"
#include <string>
#include <cstdio>

STDIO_API cx_type *cx_remove(cx_runtime_stack *p_stack,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	const char *filename = (const char *)p_stack->top()->basic_types.addr__;
	p_stack->pop();
	
	p_stack->push((bool)(std::remove(filename) == 0));

	return cx_function_id->p_type;
}

STDIO_API cx_symbols cx_lib_init(void){

	params *p_remove_params = new params;
	p_remove_params->name = "filename";
	p_remove_params->type = cx_address;
	p_remove_params->ref_or_val = dc_value_parm;
	p_remove_params->array.element_type = cx_char;

	return {
		{ "remove", 1, p_remove_params, dc_function, cx_bool, func_standard, nullptr, "cx_remove" }
	};
}