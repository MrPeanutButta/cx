#ifndef CX_API_H
#define CX_API_H

#include "types.h"
#include "symtable.h"
#include "exec.h"

#include <string>

/* commented enums below are only for reference.
enum cx_type_form_code {
	fc_none,
	fc_scalar,
	fc_enum,
	fc_subrange,
	fc_array,
	fc_complex,
	fc_stream
};

enum cx_type_code {
	cx_int,
	cx_char,
	cx_wchar,
	cx_float,
	cx_bool,
	cx_uint8,
	cx_address,
	cx_complex,
	cx_file,
	cx_void
};

enum cx_define_code {
	dc_undefined, dc_constant, dc_type, dc_variable, dc_member,
	dc_value_parm, dc_reference, dc_pointer,
	dc_program, dc_function
};

enum cx_function_code {
	func_declared, func_forward, func_standard, func_std_member
};*/

struct params{
public:
	params(){
		next = nullptr;
	}

	// name of variable
	std::string name;
	// data type
	cx_type_code type;
	// defined as reference or value parameter
	cx_define_code ref_or_val;

	// if an array, what element type
	struct {
		cx_type_code element_type;
	}array;

	// next parameter
	params *next;
};

typedef cx_type *(*m_call)(cx_executor *, cx_symtab_node *, const cx_type *);

struct cx_symbol{
public:

	// name of symbol
	std::string name;

	int param_count;

	// function params
	params *parameters;
	
	/* define code
		dc_undefined, dc_constant, dc_type, dc_variable,
		dc_value_parm, dc_reference, dc_program, dc_function
	*/
	cx_define_code define;

	/* return type
		    cx_int, cx_char, cx_wchar, cx_float, cx_bool, 
			cx_uint8, cx_address, cx_complex, cx_file, cx_void
	*/
	cx_type_code return_type;

	/* declared as
			func_declared, func_forward, func_standard, func_std_member
	*/
	cx_function_code which;

	// cx_type *(cx_runtime_stack *, cx_symtab_node *, const cx_type *);
	ext_call f_ptr;

	// symbol name as it appears in the library
	std::string symbol_name;
};

#include <vector>
typedef std::vector<cx_symbol> cx_symbols;

#endif
