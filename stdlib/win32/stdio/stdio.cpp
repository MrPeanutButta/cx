// stdio.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "stdio.h"
#include "cx_api.h"
#include <string>

class STDIO_API cxstdio{
public:
	static cx_type *remove(cx_executor *cx,
		cx_symtab_node *cx_function_id,
		const cx_type *p_type);
};

cx_type *cxstdio::remove(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	return cx_function_id->p_type;
}

STDIO_API cx_symbols cx_lib_init(void){

	return{
		{ nullptr, "remove", func_standard, &cxstdio::remove, cx_bool } 
	};
}