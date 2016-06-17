/*
The MIT License (MIT)

Copyright (c) 2015 Aaron Hebert <aaron.hebert@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "parser.h"

namespace cx {
	/** parse_block      parse a function's block:
	*
	*                      {
	*                              <compound-statement>
	*                      }
	*
	* @param p_function_id : ptr to symbol table node of function's id.
	*/
	void parser::parse_block(symbol_table_node_ptr &p_function_id) {
		// <compound-statement> : reset the icode and append BEGIN to it,
		//                        and then parse the compound statement.
		resync(tokenlist_statement_start);
		if (token != TC_LEFT_BRACKET) cx_error(ERR_MISSING_LEFT_BRACKET);

		parse_compound(p_function_id);
	}

	/** parse_formal_parm_list     parse a formal parameter list:
	*
	*                              ( <type-id> <id-list> ); or {
	*
	//* @param count
	* @param total_size
	* @return
	*///s//ymbol_table_node_ptr p_parm_id;// = nullptr; // ptrs to parm symtab nodes

	void parser::parse_formal_parm_list(symbol_table_node_ptr &p_function_id) {
		symbol_table_node_ptr p_node = nullptr;

		//// Loop to parse the comma-separated sublist of parameter ids.
		while (token == TC_IDENTIFIER) {
			bool is_array = false;

			// find param type
			p_node = find(p_token->string);

			if (p_node->defined.defined_how != DC_TYPE) {
				cx_error(ERR_INVALID_TYPE);
			}

			// get param name
			get_token();

			// Reference or array parameter?
			if (token == TC_STAR) {
				get_token();
				is_array = true;
			}

			symbol_table_node_ptr p_param = nullptr;


			p_param = enter_new_local(p_token->string, DC_VARIABLE);

			if (is_array) {
				p_param->p_type = std::make_shared<cx_type>(F_ARRAY, T_REFERENCE);
				p_param->p_type->array.p_element_type = p_node->p_type;
				p_param->p_type->p_type_id = p_param;
			}
			else {
				p_param->p_type = p_node->p_type;
			}

			get_token();
			p_function_id->defined.routine.p_parameter_ids.push_back(p_param);

			resync(tokenlist_identifier_follow);
			if (token == TC_COMMA) {

				/* Saw comma.
				* Skip extra commas and look for an identifier. */
				do {
					get_token();
					resync(tokenlist_identifier_start, tokenlist_identifier_follow);
					if (token == TC_COMMA) {
						cx_error(ERR_MISSING_IDENTIFIER);
					}
				} while (token == TC_COMMA);
				if (token != TC_IDENTIFIER) {
					cx_error(ERR_MISSING_IDENTIFIER);
				}
			}
			else if (token == TC_IDENTIFIER) cx_error(ERR_MISSING_COMMA);

			//  ; or )
			// resync(tlFormalParmsFollow, tokenlist_declaration_follow);

		}

		if (token == TC_IDENTIFIER) {
			cx_error(ERR_MISSING_SEMICOLON);
		}
		else while (token == TC_SEMICOLON) get_token();
	}

	/** parse_subroutine_call     parse a call to a declared or a
	*                          standard procedure or function.
	*
	* @param p_function_id    : ptr to routine id's symbol table node.
	* @param parm_check_flag : true to check parameter, false not to
	* @return ptr to the subroutine's type object
	*/
	symbol_table_node_ptr parser::parse_subroutine_call(
		symbol_table_node_ptr &p_function_id,
		std::pair<local::iterator, local::iterator> p_node_ids) {
		symbol_table_node_ptr p_result_node = parse_declared_subroutine_call(p_function_id, p_node_ids);
		p_function_id->defined.routine.program_code.push_back({ CALL, p_result_node.get() });
		return p_result_node;
	}

	/** parse_declared_subroutine_call parse a call to a declared
	*                              procedure or function.
	*
	* @param p_function_id    : ptr to subroutine id's symbol table node.
	* @param parm_check_flag : true to check parameter, false not to.
	* @return ptr to the subroutine's type object.
	*/
	symbol_table_node_ptr parser::parse_declared_subroutine_call(
		symbol_table_node_ptr &p_function_id,
		std::pair<local::iterator, local::iterator> p_node_ids) {

		symbol_table_node_ptr p_result_node;
		if (token == TC_LEFT_PAREN) {
			p_result_node = parse_actual_parm_list(p_function_id, p_node_ids);
		}
		else {
			for (auto it = p_node_ids.first; it != p_node_ids.second; ++it) {
				if (it->second->defined.defined_how != define_code::DC_FUNCTION) continue;

				if (assignment_is_compatible(it->second->p_type, this->p_target_type)) {
					p_result_node = it->second;
					break;
				}
			}
		}

		return p_result_node;
	}

	/** parse_actual_parm_list     parse an actual parameter list:
	*
	*                              ( <expr-list> )
	*
	* @param p_function_id    : ptr to routine id's symbol table node.
	* @param parm_check_flag : true to check parameter, false not to.
	*/
	symbol_table_node_ptr parser::parse_actual_parm_list(
		symbol_table_node_ptr &p_function_id,
		std::pair<local::iterator, local::iterator> p_node_ids) {

		/* If there are no actual parameters, there better not be
		* any formal parameters either. */

		if (token != TC_LEFT_PAREN) {
			symbol_table_node_ptr p_result;

			for (auto it = p_node_ids.first; it != p_node_ids.second; ++it) {
				if (it->second->defined.defined_how != define_code::DC_FUNCTION) continue;

				if (it->second->p_type == this->p_target_type) {
					return it->second;
				}
			}
		}

		//std::vector <std::shared_ptr<symbol_table_node>>::iterator p_formal_id;
		//p_formal_id = p_node_id->defined.routine.p_parameter_ids.begin();

		std::vector<type_ptr> param_results;
		type_ptr p_expr_type;
		symbol_table_node_ptr p_result_node;

		/* Loop to parse actual parameter expressions
		* separated by commas. */
		do {
			//  ( or ,
			get_token();

			//if (token == TC_RIGHT_PAREN && (p_node_id->defined.routine.p_parameter_ids.size()) == 0) {
			//	break;
			//}

			// TODO parse_expression and check type compatability
			//p_expr_type = parse_expression(p_function_id);
			param_results.emplace_back(parse_expression(p_function_id));
			//check_assignment_type_compatible(p_function_id, p_formal_id->get()->p_type, p_expr_type,
			//	ERR_INCOMPATIBLE_TYPES);

			//if (p_formal_id != p_node_id->defined.routine.p_parameter_ids.end()){
			//	++p_formal_id;
			//}
		} while (token == TC_COMMA);

		//  )
		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		bool found_match = false;

		for (auto it = p_node_ids.first; it != p_node_ids.second; ++it) {
			if (it->second->defined.defined_how != define_code::DC_FUNCTION) continue;

			//			if (assignment_is_compatible(it->second->p_type, this->p_target_type)) {
			if (it->second->defined.routine.p_parameter_ids.size() == param_results.size()) {
				int count = param_results.size();
				for (int index = 0; index < count; ++index) {
					if (!assignment_is_compatible(it->second->defined.routine.p_parameter_ids[index]->p_type, param_results[index])) {
						found_match = false;
						break;
					}
					else {
						found_match = true;
					}
				}
			}
			//	}

			if (found_match) {
				p_result_node = it->second;
				break;
			}
		}

		return p_result_node;
	}

	/** parse_function_header         parse a function header:
	*
	*                              <type-id> <id> (<parm-list>);
	*
	*                          or:
	*
	*                              <type-id> <id> (<parm-list>){}
	*
	* NOTE:
	*      If scope == 0 and p_program_ptr_id->found_global_end == false;
	*      Set main's location in icode only when function body is found.
	*
	* @param p_function_id : ptr to the function id's symbol table node.
	* @return ptr to function id's symbol table node.
	*/
	symbol_table_node_ptr parser::parse_function_header(symbol_table_node_ptr &p_function_id) {
		// enter the next__ nesting level and open a new scope
		// for the function.
		symtab_stack.enter_scope();
		//  (
		conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		parse_formal_parm_list(p_function_id);
		p_function_id->defined.defined_how = DC_FUNCTION;

		// For recursive calls.
		//symtab_stack.enter_new_function(p_function_id);
		//  )
		conditional_get_token(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		if (token == TC_SEMICOLON) {
			p_function_id->defined.routine.function_type = FUNC_FORWARD;

			//for()
		}
		else {
			p_function_id->defined.routine.function_type = FUNC_DECLARED;
			parse_statement(p_function_id);
			p_function_id->defined.routine.p_symtab = symtab_stack.exit_scope();
		}

		return p_function_id;
	}
}