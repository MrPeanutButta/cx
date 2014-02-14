/** parse Directives
 * parse_directive.cpp
 *
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"
#include "cx_api.h"

#ifdef _WIN32
#include <windows.h>

std::vector<HINSTANCE> windows_libs;

cx_symbols *load_windows_lib(const char *lib, cx_symtab *p_symtab){
	typedef cx_symbols(*lib_init)(void);

	cx_symbols *loaded_symbols = nullptr;

	HINSTANCE hinst_lib = nullptr;
	lib_init init_call = nullptr;
	bool f_runtime_linksuccess = false;

	std::string lib_path;
	char *env_path = getenv(__CX_STDLIB__);

	if (env_path != nullptr) {
		lib_path = env_path;
		lib_path += "\\";
	}

	std::string dll = std::string(lib) + ".dll";

	lib_path += dll;

	// Get a handle to the DLL module.
	hinst_lib = LoadLibrary(lib_path.c_str());

	// If the handle is valid, try to get the function address.
	if (hinst_lib != nullptr){
		init_call = (lib_init)GetProcAddress(hinst_lib, "cx_lib_init");

		windows_libs.push_back(hinst_lib);

		// If the function address is valid, call the function.
		if (nullptr != init_call){
			f_runtime_linksuccess = true;
			//pass our symbol table to the DLL with a map of allowed data types.
			loaded_symbols = new cx_symbols((init_call)());
		}
		else {
			cx_error(err_library_no_init);
		}

	}
	else {
		cx_error(err_loading_library);
	}

	if (loaded_symbols != nullptr){
		for (unsigned int i = 0; i < loaded_symbols->size(); ++i){
			loaded_symbols->at(i).f_ptr = (ext_call)GetProcAddress(hinst_lib, loaded_symbols->at(i).symbol_name.c_str());
			std::cout << loaded_symbols->at(i).name << std::endl;
		}
	}

	return loaded_symbols;
}

#endif

void load_symbols(cx_symtab *p_symtable, cx_symbols *loaded_symbols){
	extern cx_symtab_node *p_program_ptr_id;
	extern cx_datatype cx_dt;

	cx_symtab_node *p_node = nullptr;

	for (cx_symbols::iterator it = loaded_symbols->begin(); it != loaded_symbols->end(); ++it){
		p_node = p_symtable->enter(it->name.c_str(), it->define);
		p_node->defn.routine.which = it->which;

		if (it->return_type == cx_address){
			/*set_type(p_node->p_type, new cx_type(fc_array, 0, p_node));
			set_type(p_node->p_type->array.p_element_type, it->)*/
		}
		else {
			set_type(p_node->p_type, cx_dt[it->return_type]);
		}

		if (it->parameters != nullptr){
			
			p_node->defn.routine.parm_count = it->param_count;
			
			cx_symtab_node *p_pnode = nullptr;

			// for each parameter
			for (params *p = it->parameters; p != nullptr; p = p->next){
				if (p_node->defn.routine.locals.p_parms_ids == nullptr){
					p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node(p->name.c_str(), p->ref_or_val);
					p_pnode = p_node->defn.routine.locals.p_parms_ids;
				}
				else {
					p_pnode->next__ = new cx_symtab_node(p->name.c_str(), p->ref_or_val);
					p_pnode = p_pnode->next__;
				}

				if (p->type == cx_address){
					set_type(p_pnode->p_type, new cx_type(fc_array, 0, p_pnode));
					set_type(p_pnode->p_type->array.p_element_type, cx_dt[p->array.element_type]);
				}
				else {
					set_type(p_pnode->p_type, cx_dt[p->type]);
				}
			}

			p_node->defn.routine.ext_function = it->f_ptr;
		}
	}

	//cx_type *t = (loaded_symbols->at(0).f_ptr)(nullptr, p_program_ptr_id, p_integer_type);

	delete loaded_symbols;
}

/** parse_execute_directive      Opens an external script module
 *                      for parsing.
 *
 *      #include <identifier>
 *
 * @param p_function_id : ptr to the routine owning this directive call.
 */
void cx_parser::parse_execute_directive (cx_symtab_node *p_function_id) {
	
    extern cx_symtab_node *p_program_ptr_id;

    switch (token) {
	case tc_INCLUDE:{
						std::string lib_path;
						char *env_path = getenv(__CX_STDLIB__);

						if (env_path != nullptr) {
							lib_path = env_path;
							lib_path += "/";
						}

						get_token();

						lib_path += p_token->string__();
						p_program_ptr_id->found_global_end = true;

						// save current line number
						int old_line_num = current_line_number;
						current_line_number = 0;

						/* true : stdlib module
						 * returns nullptr */
						cx_parser *parser = new cx_parser
							(new cx_source_buffer(lib_path.c_str()), true);

						parser->parse();

						current_line_number = old_line_num;

						delete parser;

						icode.reset();
						icode.put(tc_left_bracket);
						p_program_ptr_id->found_global_end = false;
						get_token_append();
	}
            break;	
	case tc_WARN:{
						 get_token();

						 if (token != tc_string){
							 cx_error(err_unexpected_token);
						 }

						 {
							 std::string msg = p_token->string__();
							 msg[0] = ' '; msg[msg.length() - 1] = '\0';
							 std::cerr << "warning:" << msg << std::endl;
						 }

						 get_token_append();
		}
			break;	
	case tc_IMPORT:{
						   get_token();
						   cx_symbols *p_symbols = nullptr;
#ifdef _WIN32
						   p_symbols = load_windows_lib(p_token->string__(), symtab_stack.get_current_symtab());
#else

#endif
						   if (p_symbols != nullptr){
							   load_symbols(symtab_stack.get_current_symtab(), p_symbols);
						   }

						   get_token_append();
		}
			break;
        default:
            break;
    }
}
