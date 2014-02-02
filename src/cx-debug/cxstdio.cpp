#include <sstream>
#include "cxstdio.h"
#include "types.h"
#include "symtable.h"
#include <cstdio>

cx_type *cx_stdio::puts(cx_executor *cx,
	cx_symtab_node *cx_function_id,
	const cx_type *p_type){

	// first param as string
	const char *put_string = (const char *)cx->top()->basic_types.addr__;
	cx->pop();

	// output stream
	FILE *stream = (FILE *)cx->top()->basic_types.addr__;
	cx->pop();

	// call function and push return value
	cx->push((int)fputs(put_string, stream));

	return cx_function_id->p_type;
}