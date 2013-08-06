/** parse Directives
 * parse_directive.cpp
 * 
 */

#include <cstdio>
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

    switch (token) {
        case tc_INCLUDE:
        {
            get_token_append();
            cx_parser *parser = new cx_parser(new cx_source_buffer(p_token->string__()));

            cx_symtab_node *pModule = parser->parse();
            delete parser;
            get_token_append();
        }
            break;
    }

}
