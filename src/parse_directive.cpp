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

/** parse_execute_directive      Opens an external script module
 *                      for parsing.
 * 
 *      #include <identifier>
 * 
 * @param p_function_id : ptr to the routine owning this directive call.
 */
void cx_parser::parse_execute_directive(cx_symtab_node *p_function_id) {

    extern cx_symtab_node *p_program_ptr_id;

    switch (token) {
        case tc_INCLUDE:
        {
            std::string lib_path;
			char *env_path = getenv(__CX_STDLIB__);
            
			if(env_path != nullptr) {
				lib_path = env_path;
				lib_path += "/";
			}

            get_token();

            lib_path += p_token->string__();
            p_program_ptr_id->found_global_end = true;
            
            cx_parser *parser = new cx_parser
                    (new cx_source_buffer(lib_path.c_str()));
            
            /* true : stdlib module
             * returns nullptr */
            parser->parse(true);

            delete parser;
                        
            icode.reset();
            icode.put(tc_left_bracket);
            p_program_ptr_id->found_global_end = false;
            get_token_append();
        }
            break;
    }
}
