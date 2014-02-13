#ifndef CX_API_H
#define CX_API_H

#include "types.h"
#include "symtable.h"
#include "exec.h"

struct cx_lib{
	cx_symtab_node *p_node;
	std::string name;
	cx_function_code func_code;
	m_call member_call;
	cx_type_code type;
};

#include <vector>
typedef std::vector<cx_lib> cx_symbols;

#endif
