// stdio.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
#include "cx_api.h"
#include <string>
#include <cstdio>

STDIO_API cx_type *cx_rename(cx_runtime_stack *p_stack,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	const char *new_filename = (const char *)p_stack->top()->basic_types.addr__;
	p_stack->pop();

	const char *old_filename = (const char *)p_stack->top()->basic_types.addr__;
	p_stack->pop();

	p_stack->push((bool)(std::rename(old_filename, new_filename) == 0));

	return cx_function_id->p_type;
}

STDIO_API cx_type *cx_remove(cx_runtime_stack *p_stack,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	const char *filename = (const char *)p_stack->top()->basic_types.addr__;
	p_stack->pop();
	
	p_stack->push((bool)(std::remove(filename) == 0));

	return cx_function_id->p_type;
}

STDIO_API cx_type *cx_perror(cx_runtime_stack *p_stack,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	const char *str = (const char *)p_stack->top()->basic_types.addr__;
	p_stack->pop();

	std::perror(str);

	return cx_function_id->p_type;
}

STDIO_API cx_symbols cx_lib_init(void){

	// std::remove
	params *p_remove_params = new params;
	p_remove_params->name = "filename";
	p_remove_params->type = cx_address;
	p_remove_params->ref_or_val = dc_value_parm;
	p_remove_params->array.element_type = cx_char;

	// std::perror
	params *p_perror_params = new params;
	p_perror_params->name = "str";
	p_perror_params->type = cx_address;
	p_perror_params->ref_or_val = dc_value_parm;
	p_perror_params->array.element_type = cx_char;

	// std::rename
	// char *old_filename
	params *p_rename_params = new params;
	p_rename_params->name = "old_filename";
	p_rename_params->type = cx_address;
	p_rename_params->ref_or_val = dc_value_parm;
	p_rename_params->array.element_type = cx_char;
	// char *new_filename
	p_rename_params->next = new params;
	p_rename_params->next->name = "new_filename";
	p_rename_params->next->type = cx_address;
	p_rename_params->next->ref_or_val = dc_value_parm;
	p_rename_params->next->array.element_type = cx_char;

	return {
		{ "remove", 1, p_remove_params, dc_function, cx_bool, func_standard, nullptr, "cx_remove" },
		{ "perror", 1, p_perror_params, dc_function, cx_void, func_standard, nullptr, "cx_perror" },
		{ "rename", 2, p_rename_params, dc_function, cx_bool, func_standard, nullptr, "cx_rename" }
	};
}