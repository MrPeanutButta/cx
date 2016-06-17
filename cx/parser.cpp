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

#include <iostream>
#include <cstdio>
#include "buffer.h"
#include "error.h"
#include "parser.h"
#include "cxvm.h"



namespace cx{
	//symbol_table_node_ptr p_program_ptr_id;

	/** parse       parse the source file.  After listing each
	 *              source line, extract and list its tokens.
	 *
	 * @return ptr to '__cx_global__' program Id.
	 */
	symbol_table_node_ptr parser::parse(void) {
		symbol_table_node_ptr p_program_id = nullptr;

		if (!is_module) {
			p_program_id = std::make_shared<symbol_table_node>(L"__main__", DC_PROGRAM);
			p_program_id->defined.routine.function_type = FUNC_DECLARED;
			p_program_id->p_type = p_integer_type;
		}

		scoping::current_nesting_level = 0;
		get_token();
		parse_statement_list(p_program_id, TC_END_OF_FILE);
		get_token();

		if (!is_module) {
			p_program_id->defined.routine.p_symtab = p_global_symbol_table;

			resync(tokenlist_program_end);
			conditional_get_token_append(TC_END_OF_FILE, ERR_MISSING_RIGHT_BRACKET);

			if (vm_settings::dev_debug_flag) {
				_swprintf(buffer::list.text, L"%20d source lines.", buffer::current_line_number);
				buffer::list.put_line();
				_swprintf(buffer::list.text, L"%20d syntax errors.", error::error_count);
				buffer::list.put_line();
			}
		}
		return p_program_id;
	}

	/** resync          Resynchronize the parser.  If the current
	 *                  token is not in one of the token lists,
	 *                  flag it as an error and then skip tokens
	 *                  up to one that is in a list or end of file.
	 *
	 * @param p_list1 : token list.
	 * @param p_list2 : token list.
	 * @param p_list3 : token list.
	 */
	void parser::resync(const token_code* p_list1,
		const token_code *p_list2,
		const token_code *p_list3) {

		bool errorFlag = (!token_in(token, p_list1)) &&
			(!token_in(token, p_list2)) &&
			(!token_in(token, p_list3));

		if (errorFlag) {
			error_code errorCode = token == TC_END_OF_FILE
				? ERR_UNEXPECTED_EOF
				: ERR_UNEXPECTED_TOKEN;

			cx_error(errorCode);

			while ((!token_in(token, p_list1)) &&
				(!token_in(token, p_list2)) &&
				(!token_in(token, p_list3)) &&
				(token != TC_RETURN) &&
				(token != TC_END_OF_FILE)) {
				get_token();
			}

			if ((token == TC_END_OF_FILE) &&
				(errorCode != ERR_UNEXPECTED_EOF)) {
				cx_error(ERR_UNEXPECTED_EOF);
			}
		}
	}

	/** parse_declarations_or_assignment       Parses new declarations or
	*                                      assignment statements.
	*
	* NOTE:
	*      This should be broken up a bit. Function, complex, and type declaraions
	*      should be seperated.
	*
	* @param p_function_id : ptr to the routine which owns the type being declared or
	*                     assigned a value.
	*/
	void parser::parse_declarations_or_assignment(symbol_table_node_ptr &p_function_id) {
		access_scope scope = access_scope::PUBLIC;

		switch (token) {
		case TC_PUBLIC:
			get_token();
			break;
		case TC_PRIVATE:
			scope = access_scope::PRIVATE;
			get_token();
			break;
		case TC_PROTECTED:
			scope = access_scope::PROTECTED;
			get_token();
			break;
		default:
			break;
		}

		bool is_array = false;
		symbol_table_node_ptr p_node = find(p_token->string);
		type_ptr assignment_expression_ptr = nullptr;
		
		if ((p_node->defined.defined_how == DC_TYPE) &&
			(p_node->defined.defined_how != DC_FUNCTION)) {

			this->p_target_type = p_node->p_type;
			get_token();

			if (token == TC_STAR) {
				get_token();
				is_array = true;
			}

			do {
				while (token == TC_COMMA)get_token();

				symbol_table_node_ptr p_new_id;

				p_new_id = search_local(p_token->string);

				// if not nullptr, it's already defined.
				// check if forwarded
				if (p_new_id != nullptr) {
					if (p_new_id->defined.defined_how == DC_FUNCTION &&
						p_new_id->defined.routine.function_type == FUNC_FORWARD) {
						get_token();
						parse_function_header(p_new_id);
					}
					else cx_error(ERR_REDEFINED_IDENTIFIER);
				}
				else {
					p_new_id = std::make_shared<symbol_table_node>(p_token->string, DC_UNDEFINED);
				}

				// set type
				p_new_id->p_type = p_node->p_type;
				get_token();

				// check for array type
				if (token == TC_LEFT_SUBSCRIPT) {
//					parse_array_type(p_function_id, p_new_id);
				}
				else if (is_array) {
					// TODO parse unkown array size
					p_new_id->p_type = parse_array_type(p_function_id, p_new_id);
				}
				else if (token == TC_LEFT_PAREN) {
					enter_new_function(p_new_id);
					parse_function_header(p_new_id);
				}
				else if ((token != TC_COMMA) && (token != TC_END_OF_FILE)) {

					// check for assignment
					assignment_expression_ptr = parse_assignment(p_function_id, p_new_id);
					p_new_id->defined.defined_how = DC_VARIABLE;
				}

				p_new_id->defined.member_scope = scope;

				if (p_new_id->defined.defined_how == DC_VARIABLE) {
					enter_new_local(p_new_id);

					// add variable to variable list
					if (p_function_id != nullptr) {
						p_function_id->defined.routine.p_variable_ids.emplace_back(p_new_id);
					}
					// add function to routine list
				}
				else if (p_new_id->defined.defined_how == DC_FUNCTION) {
					enter_new_function(p_new_id);

					if (p_function_id != nullptr) {
						p_function_id->defined.routine.p_function_ids.emplace_back(p_new_id);
					}
				}
			} while (token == TC_COMMA);
		}
		else if (p_node->defined.defined_how == DC_FUNCTION) {
			get_token();
			parse_subroutine_call(p_function_id, find_functions(p_node->node_name));
		}
		else {
			get_token();
			parse_assignment(p_function_id, p_node);
		}
	}

	/** parse_subscripts     parse a bracketed list of subscripts
	*                      following an array variable:
	*
	*                          [ <expr> ]
	*
	* @param p_type : ptr to the array's type object.
	* @return ptr to the array element's type object.
	*/
	type_ptr parser::parse_subscripts(symbol_table_node_ptr &p_function_id, type_ptr &p_type) {

//		int column = 0;
		type_ptr p_result_type = nullptr;
		cx_type *p_next = p_type.get();

		do {
			get_token();

			if (p_type->typeform == F_ARRAY) {
				check_assignment_type_compatible(p_function_id, p_type->array.p_index_type,
					parse_expression(p_function_id),
					ERR_INCOMPATIBLE_TYPES);

				p_result_type = p_next->array.p_element_type;

				// TODO: emit iaload xxx
//				this->emit_ax_load(p_function_id, p_result_type);
			}
			else {
				cx_error(ERR_TOO_MANY_SUBSCRIPTS);
			}

			conditional_get_token_append(TC_RIGHT_SUBSCRIPT, ERR_MISSING_RIGHT_SUBSCRIPT);
			if (token == TC_LEFT_SUBSCRIPT) {
				p_next = p_next->array.next.get();
			}
		} while (token == TC_LEFT_SUBSCRIPT);

		return p_result_type;
	}

	/*	std::string unique_name(const std::string &prefix, const int &postfix) {
			std::stringstream ss;
			ss.clear();

			ss << prefix << "_" << postfix << '\0';

			return ss.str();
			}
			*/
	/** parse_field          parse a field following a record
	*                      variable:
	*
	*                          . <id>
	*
	* @param p_type : ptr to the record's type object
	* @return ptr to the field's type object.
	*/
/*	cx_type::type_ptr parser::parse_field(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node, cx_type::type_ptr &p_type) {
		get_token();

		if (token == TC_IDENTIFIER) {
			symbol_table_node_ptr p_field_id = p_type->complex.p_class_scope->search(p_token->string);
			if (p_field_id == nullptr) cx_error(ERR_INVALID_FIELD);

			//icode.put(p_field_id);
			get_token();

			if (p_field_id->defined.defined_how == DC_FUNCTION) {
				parse_subroutine_call(p_function_id, p_field_id);
			}

			return p_field_id != nullptr ?
				p_field_id->p_type :
				p_dummy_type;

		}
		else {
			cx_error(ERR_INVALID_FIELD);
			get_token();
			return p_dummy_type;
		}

		return p_dummy_type;
	}*/

	//	extern symbol_table_node_ptr &p_program_ptr_id;

	/** parse_statement          parse a statement.
	*
	* @param p_function_id : function in which this statement is executed.
	*/
	void parser::parse_statement(symbol_table_node_ptr &p_function_id) {

		switch (this->token) {
		case TC_PUBLIC:
		case TC_PRIVATE:
		case TC_PROTECTED:
		case TC_IDENTIFIER: parse_declarations_or_assignment(p_function_id);break;
		case TC_ENUM: parse_ENUM(p_function_id); break;
		case TC_DO: parse_DO(p_function_id); break;
		case TC_WHILE: parse_WHILE(p_function_id); break;
		case TC_IF: parse_IF(p_function_id); break;
		case TC_FOR: parse_FOR(p_function_id); break;
		case TC_BREAK: get_token(); this->emit(p_function_id, opcode::BREAK_MARKER); break;
		case TC_LEFT_BRACKET: parse_compound(p_function_id); break;
		case TC_RETURN: parse_RETURN(p_function_id); break;
			//case TC_NAMESPACE:{
			//	get_token();
			//	symbol_table_node_ptr p_namespace_id = search_local(p_token->string__()); //p_function_id->defined.routine.p_symtab->enter(p_token->string__(), dc_namespace);

			//	if (p_namespace_id == nullptr){
			//		p_namespace_id = enter_local(p_token->string__(), DC_NAMESPACE);
			//		p_namespace_id->p_type = std::make_shared<cx_type>(*(new cx_type()));
			//		p_namespace_id->p_type->complex.p_class_scope = std::make_shared<symbol_table>(*(new symbol_table()));
			//	}

			//	//icode.put(p_namespace_id);
			//	symbol_table_ptr p_old_symtab = symtab_stack.get_current_symtab();

			//	symtab_stack.set_scope(++current_nesting_level);
			//	symtab_stack.set_current_symtab(p_namespace_id->p_type->complex.p_class_scope);

			//	get_token();	// namespace ID
			//	conditional_get_token_append(TC_LEFT_BRACKET, ERR_MISSING_LEFT_BRACKET);	// open bracket

			//	parse_statement_list(p_namespace_id, TC_RIGHT_BRACKET);

			//	conditional_get_token_append(TC_RIGHT_BRACKET, ERR_MISSING_RIGHT_BRACKET);
			//	symtab_stack.set_scope(--current_nesting_level);
			//	symtab_stack.set_curr
			//}
			//	break;ent_symtab(p_old_symtab);
		case TC_ASM: parse_ASM(p_function_id); break;
		case TC_DELETE: {
			get_token();
			if (token != TC_IDENTIFIER) cx_error(error_code::ERR_MISSING_IDENTIFIER);
			symbol_table_node_ptr p_node = search_all(p_token->string);
			if (p_node->p_type->typecode != T_REFERENCE) cx_error(error_code::ERR_INVALID_REFERENCE);

			this->emit(p_function_id, opcode::DEL, p_node.get());

			get_token();
		}break;
		case TC_CLASS:
			get_token();
			parse_class_declaration(p_function_id);
			break;
		case TC_INIT:
			get_token();
			parse_init(p_function_id);
			break;
		case TC_DISPOSE:
			get_token();
			parse_dispose(p_function_id);
			break;
		default: parse_expression(p_function_id); break;
		}
	}

	/** parse_statement_list      parse a statement list until the
	*                          terminator token.
	*
	* @param p_function_id : function in which these statements are executed.
	* @param terminator : the token that terminates the list.
	*/
	void parser::parse_statement_list(symbol_table_node_ptr &p_function_id, token_code terminator) {
		do {
			parse_statement(p_function_id);
			while (token == TC_SEMICOLON)get_token();
		} while ((token != terminator) && (token != TC_END_OF_FILE));
	}

	/** parse_assignment         parse an assignment statement.
	*
	* NOTE:
	*      Just calls parse_variable; This is because expressions are fully
	*      recursive.
	*
	* @param p_target_id : ptr to target id's symbol table node
	* @return ptr to the p_target_id type object.
	*/
	type_ptr parser::parse_assignment(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_target_id) {

		type_ptr p_result_target_type = parse_variable(p_function_id, p_target_id, true);

		return p_result_target_type;
	}

	/** parse_compound       parse compounded statements.
	*
	*      {
	*         <statements>;
	*      }
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_compound(symbol_table_node_ptr &p_function_id) {
		get_token();	
		parse_statement_list(p_function_id, TC_RIGHT_BRACKET);
		conditional_get_token_append(TC_RIGHT_BRACKET, ERR_MISSING_RIGHT_BRACKET);
	}

	/** parse_unksize_array_type      parse an array type specification
	*                                of unknown size.
	*
	*      <type-id> *<id>;
	*
	* @param p_function_id : ptr to fuction which owns this array.
	* @param p_array_node : ptr to array symtab node.
	* @return ptr to array type object.
	*/
	type_ptr parser::parse_array_type(symbol_table_node_ptr &p_function_id,
		symbol_table_node_ptr &p_array_node) {

		type_ptr p_array_type = std::make_shared<cx_type>(F_ARRAY, T_REFERENCE);
		p_array_type->array.element_count = 0;
		p_array_type->array.max_index = 0;
		p_array_type->array.min_index = 0;
		p_array_type->array.p_element_type = p_array_node->p_type;
		p_array_type->array.p_index_type = p_integer_type;
		p_array_type->size = 0;
		p_array_type->p_type_id = p_array_node;

		type_ptr p_expr_type = nullptr;

		bool is_function = false;
		const bool is_expression = token_in(this->token, tokenlist_assign_ops);

		if ((this->token != TC_LEFT_PAREN) && (token != TC_RIGHT_PAREN) &&
			(!is_expression)) get_token();
		else	
		if ((this->token == TC_LEFT_PAREN) && (!is_expression)) is_function = true;

		if (is_function) {
			p_array_node->p_type = p_array_type;
			parse_function_header(p_array_node);
			return p_array_node->p_type;
		}

		p_array_node->p_type = p_array_type;
		p_array_node->defined.defined_how = DC_VARIABLE;

		if (is_expression) {	
			p_expr_type = parse_assignment(p_function_id, p_array_node);
			check_assignment_type_compatible(p_function_id, p_array_node->p_type, p_expr_type, error_code::ERR_INCOMPATIBLE_TYPES);

			p_array_node->p_type = p_expr_type;
			p_array_node->p_type->p_type_id = p_array_node;
		}

		return p_array_node->p_type;
	}

	/** array_size           Calculate the total byte size of an
	*                      array type by recursively calculating
	*                      the size of each dimension.
	*
	* @param p_array_type : ptr to array type object.
	* @return byte size.
	*/
	size_t parser::array_size(type_ptr &p_array_type) {
		if (p_array_type->array.p_element_type->size == 0) {
			p_array_type->array.p_element_type->size = array_size(p_array_type->array.p_element_type);
		}

		return (p_array_type->array.element_count * p_array_type->array.p_element_type->size);
	}
}