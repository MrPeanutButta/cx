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

#if defined _WIN32
#include <windows.h>
std::vector<HINSTANCE> windows_libs;
#elif defined __linux__
#include <dlfcn.h>
std::vector<void *> linux_libs;
#elif defined __APPLE__

#endif

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

				//p_new_id = search_local(p_token->string);

				// if not nullptr, it's already defined.
				// check if forwarded
				/*if (p_new_id != nullptr) {
					if (p_new_id->defined.defined_how == DC_FUNCTION &&
						p_new_id->defined.routine.function_type == FUNC_FORWARD) {
						get_token();
						parse_function_header(p_new_id);
					}
					else cx_error(ERR_REDEFINED_IDENTIFIER);
				}
				else {*/
					//p_new_id = enter_new_local(p_token->string);
					p_new_id = std::make_shared<symbol_table_node>(p_token->string, DC_UNDEFINED);
				//}

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
					if (p_function_id != nullptr) {

						//p_new_id->node_name = p_new_id->p_type->p_type_id->node_name + p_new_id->node_name;

						/*for (auto &param : p_new_id->defined.routine.p_parameter_ids) {
							p_new_id->node_name += param->p_type->p_type_id->node_name;
						}*/

						enter_new_function(p_new_id);

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

	// env variable that holds the path to stdlib
#define __CX_STDLIB__   L"CX_STDLIB"

	void load_lib(const wchar_t *lib, symbol_table *p_symtab) {

		static const cx_type * cx_types_[] = {
			p_integer_type.get(),
			p_char_type.get(),
			p_double_type.get(),
			p_boolean_type.get(),
			p_byte_type.get(),
			p_void_type.get()
		};

		typedef void(*lib_init)(symbol_table *, const cx_type **);
		lib_init init_call = nullptr;
		bool f_runtime_linksuccess = false;

#ifdef _WIN32
		HINSTANCE lib_handle = nullptr;
#elif defined __linux__
		void *lib_handle = nullptr;
#endif

		std::wstring lib_path;
		wchar_t *env_path = _wgetenv(__CX_STDLIB__);

		if (env_path != nullptr) {
			lib_path = env_path;
#ifdef _WIN32
			lib_path += L"\\";
#elif defined __linux__
			lib_path += L"/";
#endif
		}

#if defined _WIN32
		std::wstring dll = std::wstring(lib) + L".dll";
		lib_path += dll;
		// Get a handle to the DLL module.
		lib_handle = LoadLibrary((LPCSTR)lib_path.c_str());
#elif defined __linux__
		std::string so = std::string(lib) + ".so";
		lib_path += so;
		// Get a handle to the SO module
		lib_handle = dlopen(lib_path.c_str(), RTLD_NOW);
#endif

		// If the handle is valid, try to get the function address.
		if (lib_handle != nullptr) {

#if defined _WIN32
			init_call = (lib_init)GetProcAddress(lib_handle, "cx_lib_init");
			windows_libs.push_back(lib_handle);
#elif defined __linux__
			init_call = (lib_init)dlsym(lib_handle, "cx_lib_init");
			linux_libs.push_back(lib_handle);
#endif

			// If the function address is valid, call the function.
			if (nullptr != init_call) {
				f_runtime_linksuccess = true;
				//pass our symbol table to the library with a array of allowed data types.
				(init_call)(p_symtab, cx_types_);
			}
			else {
#if defined __linux__
				std::cout << dlerror() << std::endl;
#endif
				cx_error(ERR_LIBRARY_NO_INIT);
			}

		}
		else {
#if defined __linux__
			std::cout << dlerror() << std::endl;
#endif
			cx_error(ERR_LOADING_LIBRARY);
		}
	}

	/** parse_execute_directive      Opens an external script module
	*                      for parsing.
	*
	*      #include <identifier>
	*
	* @param p_function_id : ptr to the routine owning this directive call.
	*/
//	void parser::parse_execute_directive(symbol_table_node_ptr &p_function_id) {
//
//		//	extern symbol_table_node_ptr p_program_ptr_id;
//
//		switch (token) {
//		case TC_INCLUDE:
//		{
//			std::wstring lib_path;
//			wchar_t *env_path = _wgetenv(__CX_STDLIB__);
//
//			if (env_path != nullptr) {
//				lib_path = env_path;
//#ifdef _WIN32
//				lib_path += L"\\";
//#else
//				lib_path += L"/";
//#endif
//			}
//
//			get_token();
//
//			lib_path += p_token->string;
//			p_program_ptr_id->found_global_end = true;
//
//			// save current line number
//			int old_line_num = current_line_number;
//			current_line_number = 0;
//
//			/* true : stdlib module
//			* returns nullptr */
//			{
//				std::unique_ptr<parser> p_parser;// = nullptr;
//
//				try {
//					p_parser = std::make_unique<parser>(new source_buffer(lib_path.c_str()), true);
//
//					p_parser->parse();
//				}
//				catch (...) {
//
//				}
//			}
//
//			current_line_number = old_line_num;
//			p_program_ptr_id->found_global_end = false;
//			get_token();
//		}
//		break;
//		case TC_WARN:
//		{
//			get_token();
//
//			if (token != TC_STRING) {
//				cx_error(ERR_UNEXPECTED_TOKEN);
//			}
//
//			{
//				std::wstring msg = p_token->string;
//				msg[0] = ' ';
//				msg[msg.length() - 1] = L'\0';
//				std::wcerr << "warning:" << msg << std::endl;
//			}
//
//			get_token();
//		}
//		break;
//		case TC_IMPORT:
//		{
//			get_token();
//
//			load_lib(p_token->string, symtab_stack.get_current_symtab());
//
//			get_token();
//		}
//		break;
//		default:
//			break;
//		}
//	}

	/** parse_expression     parse an expression (binary relational
	*                      operators == < > <> <= and >= ).
	*
	* @return ptr to the expression's type object.
	*/
	type_ptr parser::parse_expression(symbol_table_node_ptr &p_function_id) {

		type_ptr p_result_type;
		type_ptr p_operand_type;
		token_code op;

		p_result_type = parse_simple_expression(p_function_id);

		if (token_in(token, tokenlist_relation_ops)) {
			op = token;
			get_token();
			p_operand_type = parse_simple_expression(p_function_id);

			check_relational_op_operands(p_function_id, p_result_type, p_operand_type);
			//check_assignment_type_compatible(p_function_id, p_result_type, p_operand_type, error_code::ERR_INCOMPATIBLE_TYPES);

			switch (op){
			case TC_LESSTHAN:
				this->emit_lt(p_function_id, p_result_type);
				break;
			case TC_GREATERTHAN:
				this->emit_gt(p_function_id, p_result_type);
				break;
			case TC_LESSTHAN_EQUAL:
				this->emit_lt_eq(p_function_id, p_result_type);
				break;
			case TC_GREATERTHAN_EQUAL:
				this->emit_gt_eq(p_function_id, p_result_type);
				break;
			case TC_NOT_EQUAL:
				this->emit_not_eq(p_function_id, p_result_type);
				break;
			case TC_EQUAL_EQUAL:
				this->emit_eq_eq(p_function_id, p_result_type);
				break;
			}

			p_result_type = p_boolean_type;
		}

		resync(tokenlist_expression_follow, tokenlist_statement_follow, tokenlist_statement_start);

		return p_result_type;
	}

	/** parse_simple_expression       parse a simple expression
	*                              (unary operators + or - , and
	*                              binary operators + - and ||).
	*
	* @return ptr to the simple expression's type object.
	*/
	type_ptr parser::parse_simple_expression(symbol_table_node_ptr &p_function_id) {

		type_ptr p_result_type;
		type_ptr p_operand_type;
		token_code op = TC_DUMMY;
		token_code unary_op = TC_DUMMY;

		bool unary_op_flag = false;

		if (token_in(token, tokenlist_unary_ops)) {
			unary_op_flag = true;
			unary_op = token;
			get_token();
		}

		p_result_type = parse_term(p_function_id);

		if (unary_op_flag) {
			switch (unary_op){
			case TC_LOGIC_NOT:
				this->emit_lnot(p_function_id, p_result_type);
				p_result_type = p_boolean_type;
				break;
			case TC_BIT_NOT:
				check_bitwise_integer(p_result_type);
				this->emit(p_function_id, opcode::INOT);
				break;
			case TC_PLUS:
				check_integer_or_real(p_result_type, nullptr);
				if (p_result_type->typecode == T_INT) {
					this->emit(p_function_id, opcode::IPOS);
				}
				else {
					this->emit(p_function_id, opcode::DPOS);
				}
				break;
			case TC_MINUS:
				check_integer_or_real(p_result_type, nullptr);
				if (p_result_type->typecode == T_INT) {
					this->emit(p_function_id, opcode::INEG);
				}
				else {
					this->emit(p_function_id, opcode::DNEG);
				}
				break;
			default:
				break;
			}
		}

		while (token_in(token, tokenlist_add_ops)) {
			op = token;
			get_token();

			p_operand_type = parse_term(p_function_id);

			switch (op) {
			case TC_PLUS:
				check_assignment_type_compatible(p_function_id, p_result_type, p_operand_type,
					ERR_INCOMPATIBLE_TYPES);

				this->emit_add(p_function_id, p_operand_type);
				break;
			case TC_MINUS:
				check_assignment_type_compatible(p_function_id, p_result_type, p_operand_type,
					ERR_INCOMPATIBLE_TYPES);

				this->emit_sub(p_function_id, p_operand_type);
				break;
			case TC_BIT_LEFTSHIFT:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);
				this->emit(p_function_id, opcode::ISHL);
				break;
			case TC_BIT_RIGHTSHIFT:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);
				this->emit(p_function_id, opcode::ISHR);
				break;
			case TC_BIT_AND:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);
				this->emit(p_function_id, opcode::IAND);
				break;
			case TC_BIT_XOR:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);
				this->emit(p_function_id, opcode::IXOR);
				break;
			case TC_BIT_OR:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);
				this->emit(p_function_id, opcode::IOR);
				break;
			case TC_LOGIC_OR:
				check_boolean(p_result_type, p_operand_type);
				this->emit(p_function_id, opcode::LOGIC_OR);
				break;
			default:
				break;
			}
		}

		return p_result_type;
	}

	/** parse_term           parse a term (binary operators * /
	*                      % and &&).
	*
	* @return ptr to the term's type object.
	*/
	type_ptr parser::parse_term(symbol_table_node_ptr &p_function_id) {

		type_ptr p_result_type;
		type_ptr p_operand_type;
		token_code op;

		p_result_type = parse_factor(p_function_id);

		while (token_in(token, tokenlist_mul_ops)) {

			op = token;
			get_token();
			p_operand_type = parse_factor(p_function_id);

			switch (op) {
			case TC_STAR:
				check_assignment_type_compatible(p_function_id, p_result_type, p_operand_type,
					ERR_INCOMPATIBLE_TYPES);

				this->emit_mul(p_function_id, p_operand_type);
				break;
			case TC_DIVIDE:
				check_assignment_type_compatible(p_function_id, p_result_type, p_operand_type,
					ERR_INCOMPATIBLE_TYPES);

				this->emit_div(p_function_id, p_operand_type);
				break;
			case TC_MODULAS:
				check_assignment_type_compatible(p_function_id, p_result_type, p_operand_type,
					ERR_INCOMPATIBLE_TYPES);

				this->emit_mod(p_function_id, p_operand_type);
				break;
			case TC_LOGIC_AND:
				check_boolean(p_result_type, p_operand_type);

				this->emit(p_function_id, { LOGIC_AND });
				break;
			default:
				break;
			}
		}

		return p_result_type;
	}

	/** parse_factor         parse a factor (identifier, number,
	*                      string, ! <factor>, or parenthesized
	*                      subexpression).
	*
	* @return ptr to the factor's type object.
	*/
	type_ptr parser::parse_factor(symbol_table_node_ptr &p_function_id) {

		type_ptr p_result_type = nullptr;
		token_code op = TC_DUMMY;

		switch (this->token){
		case TC_PLUS_PLUS:
			op = TC_PLUS_PLUS;
			this->get_token();
			break;
		case TC_MINUS_MINUS:
			op = TC_MINUS_MINUS;
			this->get_token();
			break;
		default: break;
		}

		switch (this->token) {
		case TC_IDENTIFIER:
		{
			symbol_table_node_ptr p_node = search_all(p_token->string);

			if (p_node == nullptr){
				cx_error(ERR_UNDEFINED_IDENTIFIER);
			}

			p_result_type = p_node->p_type;

			switch (p_node->defined.defined_how) {
			case DC_FUNCTION: {
				get_token();

				// Find all functions that share this name.
				auto function_list = this->find_functions(p_node->node_name);
				p_node = parse_subroutine_call(p_function_id, function_list);
				p_result_type = p_node->p_type;

				if (token == TC_DOT) {
					p_result_type = parse_variable(p_function_id, p_node);
				}
				else if (token == TC_LEFT_SUBSCRIPT) {
					p_result_type = parse_variable(p_function_id, p_node);
				}
			}break;
			case DC_CONSTANT:
				get_token();
				p_result_type = p_node->p_type;
				this->emit_const(p_function_id, p_node);
				break;

			case DC_TYPE:
				get_token();

				if (p_node->p_type->typeform == F_ENUM) {
					p_result_type = parse_variable(p_function_id, p_node);
				}

				//p_result_type = p_node->p_type;
				break;
			case DC_VARIABLE:
			case DC_VALUE_PARM:
			case DC_REFERENCE:
			case DC_MEMBER:
			case DC_NAMESPACE:

				switch (op)
				{
				case TC_PLUS_PLUS:
					switch (p_result_type->typecode){
					case T_DOUBLE:
						this->emit(p_function_id, opcode::DINC, p_node.get(), 1.0L);
						break;
					default:
						this->emit(p_function_id, opcode::IINC, p_node.get(), 1);
						break;
					}
					break;
				case TC_MINUS_MINUS:
					switch (p_result_type->typecode){
					case T_DOUBLE:
						this->emit(p_function_id, opcode::DINC, p_node.get(), -1.0L);
						break;
					default:
						this->emit(p_function_id, opcode::IINC, p_node.get(), -1);
						break;
					}
					break;
				default:
					break;
				}

				get_token();
				p_result_type = parse_variable(p_function_id, p_node, false);

				break;
			default:
				cx_error(ERR_UNDEFINED_IDENTIFIER);
				break;
			}
		}
		break;
		case TC_NUMBER:{
			symbol_table_node_ptr p_node = search_all(p_token->string);

			if (p_node == nullptr) {
				p_node = enter_local(p_token->string, DC_CONSTANT);
			}

			switch (p_token->type()){
			case T_INT:
				p_node->p_type = p_integer_type;
				p_node->defined.constant_value.i_ = p_token->value().i_;
				this->emit(p_function_id, opcode::ICONST, p_node->defined.constant_value.i_);
				break;
			case T_DOUBLE:
				p_node->p_type = p_double_type;
				p_node->defined.constant_value.d_ = p_token->value().d_;
				this->emit(p_function_id, opcode::DCONST, p_node->defined.constant_value.d_);
				break;
			default:
				cx_error(ERR_INCOMPATIBLE_ASSIGNMENT);
			}

			p_result_type = p_node->p_type;
			get_token();
		}break;
		case TC_CHAR:
		{
			symbol_table_node_ptr p_id = search_all(p_token->string);

			if (p_id == nullptr) {
				p_id = enter_local(p_token->string);
				p_id->p_type = p_char_type;
				p_id->defined.constant_value.c_ = p_token->string[1];
			}

			p_result_type = p_char_type;

			this->emit(p_function_id, ICONST, p_id->defined.constant_value.c_);
			get_token();
		}break;
		case TC_STRING:
		{
			// TODO fix string constants
			symbol_table_node_ptr p_id = search_all(p_token->string);

			if (p_id == nullptr) {
				p_id = enter_local(p_token->string);
			}

			if (p_token->type() == T_CHAR){
				p_id->p_type = p_char_type;
				p_id->defined.constant_value.c_ = (wchar_t)p_token->string[1];

				this->emit(p_function_id, ICONST, p_id->defined.constant_value);
				get_token();

				return p_char_type;
			}

			//	char *p_string = p_token->string__();
			//	symbol_table_node_ptr &p_node = search_all(p_token->string__());
			//	const int length = strlen(p_string) - 2;

			//	if (!p_node) {
			//		p_node = enter_local(p_token->string__());
			//		p_node->p_type = new cx_type(F_ARRAY, length, nullptr);
			//		set_type(p_node->p_type->array.p_element_type, p_char_type);
			//		p_node->p_type->type_code = cx_char;
			//		const int size = sizeof(char) * (length + 1);
			//		p_node->defined.constant.value.addr__ = new char[size];
			//		memset(p_node->defined.constant.value.addr__, '\0', size);
			//		memcpy(p_node->defined.constant.value.addr__,
			//			&p_string[1], size);

			//		// remove the quote
			//		char *t = (char *)p_node->defined.constant.value.addr__;
			//		t[length] = '\0';

			//		p_node->p_type->array.element_count = length;
			//		p_node->p_type->array.max_index = length;

			//	}

			//	p_result_type = p_node->p_type;
			//	icode.put(p_node);

			//	get_token();
		}break;

		case TC_LEFT_PAREN:
			get_token();
			p_result_type = parse_expression(p_function_id);
			conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);
			break;
			/*		*/
			//case tc_left_bracket:
			//{
			//	get_token_append();
			//	int total_size = 0;
			//	int elem_count = 0;
			//	bool comma = false;
			//	cx_type *p_prev_type = nullptr;

			//	do {
			//		p_result_type = parse_expression();
			//		p_prev_type = p_result_type;

			//		total_size += p_result_type->size;

			//		if (p_prev_type != nullptr) {
			//			// make sure we init all of the same type
			//			check_assignment_type_compatible(p_prev_type, p_result_type,
			//				err_incompatible_types);
			//		}

			//		if (token == tc_comma) {
			//			comma = true;
			//			get_token_append();
			//		}
			//		else comma = false;

			//		++elem_count;

			//	} while (comma);

			//	conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);

			//	cx_type *p_array_type = new cx_type(F_ARRAY, total_size, nullptr);
			//	set_type(p_array_type->array.p_element_type, p_result_type);
			//	p_array_type->array.element_count = elem_count;
			//	p_array_type->array.max_index = elem_count;

			//	p_result_type = p_array_type;
			//}
			//	break;
			//case TC_SEMICOLON:
			//	break;
		case TC_NEW: {
			get_token();
			if (token != TC_IDENTIFIER) {
				cx_error(error_code::ERR_MISSING_IDENTIFIER);
			}

			symbol_table_node_ptr p_node = search_all(p_token->string);
			if (p_node->defined.defined_how != DC_TYPE) {
				cx_error(error_code::ERR_NOT_A_TYPE_IDENTIFIER);
			}

			get_token();
			p_result_type = std::make_shared<cx_type>(F_ARRAY, T_REFERENCE);
			cx_type *p_next = p_result_type.get();

			// Array declaration
			if (token == TC_LEFT_SUBSCRIPT) {
				do {
					get_token();

					if ((token != TC_IDENTIFIER) && (token != TC_NUMBER) && (token != TC_CHAR)) {
						cx_error(error_code::ERR_INVALID_INDEX_TYPE);
					}	
					std::wstring index_id = p_token->string;	// Save node name to lookup later
					type_ptr p_expr_type = parse_expression(p_function_id);
					symbol_table_node_ptr p_const_node = search_all(index_id);

					if (p_const_node->defined.defined_how != DC_CONSTANT) {
						cx_error(error_code::ERR_NOT_A_CONSTANT_IDENTIFIER);
					}

					//cx_type::type_ptr p_expr_type = p_const_node->p_type;
					
					conditional_get_token(TC_RIGHT_SUBSCRIPT, error_code::ERR_MISSING_RIGHT_SUBSCRIPT);

					p_next->array.p_element_type = p_node->p_type;
					p_next->array.p_index_type = p_expr_type;
					p_next->array.element_count = static_cast<size_t>(p_const_node->defined.constant_value.i_);
					p_next->array.max_index = static_cast<size_t>(p_const_node->defined.constant_value.i_) - 1;
					p_next->array.min_index = 0;
					p_next->size = static_cast<size_t>(p_const_node->defined.constant_value.i_) * p_node->p_type->size;

					if (token == TC_LEFT_SUBSCRIPT) {
						p_next->array.next = std::make_shared<cx_type>(F_ARRAY, T_REFERENCE);
						p_next = p_next->array.next.get();
					}

				} while (token == TC_LEFT_SUBSCRIPT);

				this->emit(p_function_id, opcode::NEWARRAY, p_result_type.get());
			}
			// Constructor call
			else if (token == TC_LEFT_PAREN) {

			}
			else {
				cx_error(error_code::ERR_UNEXPECTED_TOKEN);
			}

			return p_result_type;
		}break;
		default:
			cx_error(ERR_INVALID_EXPRESSION);/// err_invalid_expression);
			p_result_type = p_dummy_type;
			break;
		}

		return p_result_type;
	}

	/** parse_variable       parse variable type, and assignment operators (= -- ++
	*                      += -= *= /= %= <<= >>= &= ^= |=).
	*                      Also parsed ([] and .).
	*
	* @param p_id : variable node id.
	* @return variables type object ptr.
	*/
	type_ptr parser::parse_variable(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, bool reference) {
		type_ptr p_result_type = p_id->p_type;

		switch (p_id->defined.defined_how) {
		case DC_VARIABLE:
		case DC_VALUE_PARM:
		case DC_REFERENCE:
		case DC_POINTER:
			if (!reference) {
				this->emit_load(p_function_id, p_id);
			}
			break;
		case DC_FUNCTION:
		case DC_UNDEFINED:
		case DC_NAMESPACE:
		case DC_TYPE:
			break;
	
		default:
			p_result_type = p_dummy_type;
			cx_error(ERR_INVALID_IDENTIFIER_USAGE);
			break;
		}

		//  [ or . : Loop to parse any subscripts and fields.
		bool done_flag = false;
		type_ptr p_prev_type = p_result_type;
		do {
			switch (token) {

			case TC_LEFT_SUBSCRIPT:
				p_result_type = parse_subscripts(p_function_id, p_result_type);
				if (!reference) {
					emit_ax_load(p_function_id, p_id, p_result_type);
				}
				break;

			case TC_DOT:
			case TC_COLON_COLON:
				if (p_id->p_type->typeform == F_ENUM) {
					get_token();
					symbol_table_node_ptr p_enum_id = p_id->p_type->p_enum_ids->search(p_token->string);
					if (p_enum_id == nullptr) cx_error(error_code::ERR_UNDEFINED_IDENTIFIER);

					this->emit_const(p_function_id, p_enum_id);
					p_result_type = p_enum_id->p_type;
					get_token();
				}
				// TODO: Else 
				else {
					//				p_result_type = parse_field(p_function_id, p_id, p_prev_type);
				}
				p_prev_type = p_result_type;
				break;

			case TC_PLUS_PLUS:
				this->get_token();
				switch (p_result_type->typecode){
				case T_DOUBLE:
					this->emit(p_function_id, opcode::DINC, p_id.get(), 1.0L);
					break;
				default:
					this->emit(p_function_id, opcode::IINC, p_id.get(), 1);
					break;
				}
				break;
			case TC_MINUS_MINUS:
				this->get_token();
				switch (p_result_type->typecode){
				case T_DOUBLE:
					this->emit(p_function_id, opcode::DINC, p_id.get(), -1.0L);
					break;
				default:
					this->emit(p_function_id, opcode::IINC, p_id.get(), -1);
					break;
				}
				break;


			default: done_flag = true;
			}
		} while (!done_flag);

		if (token_in(token, tokenlist_assign_ops)) {
			type_ptr p_expr_type = nullptr;

			switch (token) {
			case TC_EQUAL:
			{
				get_token();
				p_expr_type = parse_expression(p_function_id);

				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				if (p_id->p_type->typecode == T_REFERENCE) {
					if (p_expr_type->typecode == T_REFERENCE) {
						this->emit(p_function_id, opcode::ASTORE, p_id.get());
					}
					else {
						this->emit_ax_store(p_function_id, p_id);
					}
				}
				else {
					this->emit_store(p_function_id, p_id);
				}
				

				p_result_type = p_expr_type;
			}
			break;
			case TC_PLUS_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit_add(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_MINUS_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit_sub(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_STAR_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit_mul(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_DIVIDE_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit_div(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_MODULAS_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit_mod(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_BIT_LEFTSHIFT_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit(p_function_id, opcode::ISHL);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_BIT_RIGHTSHIFT_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit(p_function_id, opcode::ISHR);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_BIT_AND_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit(p_function_id, opcode::IAND);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_BIT_XOR_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit(p_function_id, opcode::IXOR);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_BIT_OR_EQUAL:
			{
				get_token();
				this->emit_load(p_function_id, p_id);
				p_expr_type = parse_expression(p_function_id);
				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);
				this->emit(p_function_id, opcode::IOR);
				this->emit_store(p_function_id, p_id);

				p_result_type = p_expr_type;
			}
			break;
			case TC_COMMA:
			case TC_SEMICOLON:
			case TC_RETURN:
				break;
			case TC_IDENTIFIER:
				get_token();
				p_expr_type = p_result_type;

			default:
				cx_error(ERR_INVALID_ASSIGNMENT);
				break;
			}
		}

		//while (token_in(token, tokenlist_subscript_or_field_start)) {
			/*p_result_type = token == TC_LEFT_SUBSCRIPT ? parse_subscripts(p_function_id, p_result_type)
				: parse_field(p_function_id, p_id, p_result_type);*/
		//}

		return p_result_type;
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

		//  )
		conditional_get_token(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		if (token == TC_SEMICOLON) {
			p_function_id->defined.routine.function_type = FUNC_FORWARD;

			//for()
		}
		else {
			p_function_id->defined.routine.function_type = FUNC_DECLARED;
			parse_statement(p_function_id);
			p_function_id->defined.routine.p_symtab.reset(symtab_stack.exit_scope());
		}

		return p_function_id;
	}

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

	void parser::parse_iterator_block(symbol_table_node_ptr &p_function_id) {
		bool is_block = (token == TC_LEFT_BRACKET);
		resync(tokenlist_statement_start);
		parse_statement(p_function_id);
		p_function_id->defined.routine.p_symtab.reset(symtab_stack.exit_scope());

		if (is_block) {
			conditional_get_token_append(TC_RIGHT_BRACKET, ERR_MISSING_RIGHT_BRACKET);
		}
		else {
			conditional_get_token_append(TC_SEMICOLON, ERR_MISSING_SEMICOLON);
		}
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
				if (it->second->p_type == this->p_target_type) {
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
				//ERR_INCOMPATIBLE_TYPES);

			//if (p_formal_id != p_node_id->defined.routine.p_parameter_ids.end()){
			//	++p_formal_id;
			//}
		} while (token == TC_COMMA);

		//  )
		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		bool found_match = false;

		for (auto it = p_node_ids.first; it != p_node_ids.second; ++it) {
			if (it->second->p_type == this->p_target_type) {
				if (it->second->defined.routine.p_parameter_ids.size() == param_results.size()) {
					int count = param_results.size();
					for (int index = 0; index < count; ++index) {
						if (it->second->defined.routine.p_parameter_ids[index]->p_type != param_results[index]) {
							found_match = false;
							break;
						}
						else {
							found_match = true;
						}
					}
				}
			}

			if (found_match) {
				p_result_node = it->second;
				break;
			}
		}

		return p_result_node;
	}

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
			//	// not a type but a cv-qualifier
			//case TC_CONST:
			//	get_token_append();
			//	parse_constant_declaration(p_function_id);
			//	break;
		case TC_ENUM: parse_ENUM(p_function_id); break;
		case TC_DO: parse_DO(p_function_id); break;
		case TC_WHILE: parse_WHILE(p_function_id); break;
		case TC_IF: parse_IF(p_function_id); break;
		case TC_FOR: parse_FOR(p_function_id); break;
			//case TC_SWITCH: parse_SWITCH(p_function_id);
			//	break;
			//	//case tc_CASE:
			//	//case tc_DEFAULT:parse_case_label(p_function_id);
			//	//  break;
		case TC_BREAK: get_token(); this->emit(p_function_id, opcode::BREAK_MARKER); break;
		case TC_LEFT_BRACKET: parse_compound(p_function_id); break;
		case TC_RETURN: parse_RETURN(p_function_id); break;
			//case TC_POUND:
			//	get_token();
				//parse_execute_directive(p_function_id);
			//	break;
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

	void parser::parse_ENUM(symbol_table_node_ptr &p_function_id) {
		get_token();
		if (token != TC_IDENTIFIER) {
			cx_error(ERR_MISSING_IDENTIFIER);
		}

		if (p_function_id == nullptr) {}

		symbol_table_node_ptr p_enum_id = this->enter_new_local(p_token->string, DC_TYPE);
		get_token();

		type_ptr p_result_type = p_integer_type;
		symbol_table_node_ptr p_node;
		symbol_table_node_ptr p_id;

		switch (token)
		{
		case TC_COLON:
			get_token();
			if (token != TC_IDENTIFIER) cx_error(error_code::ERR_NOT_A_TYPE_IDENTIFIER);
		case TC_IDENTIFIER: {
			p_node = search_all(p_token->string);
			if (p_node->defined.defined_how != DC_TYPE) cx_error(error_code::ERR_INVALID_TYPE);

			if (p_node->p_type->typecode != T_BYTE && p_node->p_type->typecode != T_INT)
				cx_error(error_code::ERR_INCOMPATIBLE_TYPES);

			p_result_type = p_node->p_type;
			get_token();
		}
		case TC_LEFT_BRACKET: {
			p_enum_id->p_type = std::make_shared<cx_type>(F_ENUM, p_result_type->typecode);
			p_enum_id->p_type->p_enum_ids = std::make_shared<symbol_table>();
			p_enum_id->p_type->p_type_id = p_enum_id;
			conditional_get_token(TC_LEFT_BRACKET, error_code::ERR_MISSING_LEFT_BRACKET);
			cx_int tc_number = 0;

			do {
				while (token == TC_COMMA) get_token();

				if (token != TC_IDENTIFIER) cx_error(error_code::ERR_MISSING_IDENTIFIER);

				p_id = p_enum_id->p_type->p_enum_ids->enter(p_token->string, DC_CONSTANT);
				p_id->p_type = p_result_type;
				get_token();

				if (token == TC_EQUAL) {
					get_token();
					// TODO: Add constant check for enum
					if (token != TC_NUMBER) cx_error(error_code::ERR_INVALID_CONSTANT);

					switch (p_result_type->typecode)
					{
					case T_INT:
						tc_number = p_token->value().i_;
						break;
					case T_BYTE:
						tc_number = p_token->value().b_;
						break;
					default:
						cx_error(error_code::ERR_INCOMPATIBLE_TYPES);
						break;
					}
					get_token();
				}
				
				p_id->defined.constant_value.i_ = tc_number++;
			} while (token == TC_COMMA);

			conditional_get_token(TC_RIGHT_BRACKET, error_code::ERR_MISSING_RIGHT_BRACKET);
		}break;
		default:
			cx_error(error_code::ERR_UNEXPECTED_TOKEN);
			break;
		}
	}

	/** parse_DO     parse do/while statement.
	*
	*      do
	*        <statement>;
	*      while(<expression>);
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_DO(symbol_table_node_ptr &p_function_id) {
		// Enter new scoped block
		symtab_stack.enter_scope();
		get_token();

		int do_start = current_location(p_function_id);//put_location_marker(p_function_id);

		parse_statement(p_function_id);

		conditional_get_token(TC_WHILE, ERR_MISSING_WHILE);
		conditional_get_token(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);
		check_boolean(parse_expression(p_function_id), nullptr);
		conditional_get_token(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		this->emit(p_function_id, opcode::IF_FALSE, 0); // Push 0 for now, come back and fix location jump.
		int break_marker = put_location_marker(p_function_id);

		this->emit(p_function_id, opcode::GOTO, { do_start });
		this->emit(p_function_id, opcode::NOP);

		fixup_location_marker(p_function_id, break_marker);
		set_break_jump(p_function_id, do_start);

		symtab_stack.exit_scope();
	}

	/** parse_WHILE          parse while statement.
	*
	*      while(<expression>)
	*            <statement>;
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_WHILE(symbol_table_node_ptr &p_function_id) {
		// Enter new scoped block
		symtab_stack.enter_scope();
		get_token();

		int while_start = current_location(p_function_id);

		conditional_get_token(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);
		check_boolean(parse_expression(p_function_id), nullptr);
		conditional_get_token(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);
		
		this->emit(p_function_id, opcode::IF_FALSE, 0); // Push 0 for now, come back and fix location jump.
		int break_marker = put_location_marker(p_function_id);

		parse_statement(p_function_id);

		this->emit(p_function_id, opcode::GOTO, { while_start });
		this->emit(p_function_id, opcode::NOP);
		fixup_location_marker(p_function_id, break_marker);
		set_break_jump(p_function_id, while_start);
		symtab_stack.exit_scope();
	}

	/** parse_IF             parse if/else statements.
	*
	*      if(<expression>)
	*         <statement>;
	*      else if (<expression>)
	*         <statement>;
	*      else
	*         <statement>;
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_IF(symbol_table_node_ptr &p_function_id) {
		// Enter new scoped block
		symtab_stack.enter_scope();
		get_token();

		conditional_get_token(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);
		check_boolean(parse_expression(p_function_id), nullptr);
		conditional_get_token(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		// Append a placeholder location marker for where to go to if
		// <expr> is false.  Remember the location of this placeholder
		// so it can be fixed up below.
		this->emit(p_function_id, opcode::IF_FALSE, 0); // Push 0 for now, come back and fix location jump.
		int at_false_location_marker = put_location_marker(p_function_id);
		
		parse_statement(p_function_id);


		int if_end = current_location(p_function_id);
		this->emit(p_function_id, opcode::NOP);
		fixup_location_marker(p_function_id, at_false_location_marker);

		if(token == TC_SEMICOLON) get_token();
		symtab_stack.exit_scope();
		if (token == TC_ELSE) {
			// Enter new scoped block
			symtab_stack.enter_scope();
			get_token();
			parse_statement(p_function_id);

			this->emit(p_function_id, opcode::NOP);
			p_function_id->defined.routine.program_code.insert(
				p_function_id->defined.routine.program_code.begin() + if_end, 
				{ opcode::GOTO, current_location(p_function_id) });

			symtab_stack.exit_scope();
		}
	}

	/** parse_FOR            parse for statements.
	*
	*      for(<statement>; <expression>; <expression>)
	*              <statement>;
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_FOR(symbol_table_node_ptr &p_function_id) {
		// Enter new scoped block
		symtab_stack.enter_scope();
		get_token();
		conditional_get_token(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		if (token != TC_SEMICOLON) {
			// Initialization
			parse_declarations_or_assignment(p_function_id);
		}

		conditional_get_token(TC_SEMICOLON, ERR_MISSING_SEMICOLON);

		int for_start = current_location(p_function_id);
		int break_marker = 0;

		if (token != TC_SEMICOLON) {
			// Condition
			check_boolean(parse_expression(p_function_id), nullptr);
			// IF_FALSE emit GOTO end of loop
			this->emit(p_function_id, opcode::IF_FALSE, 0); // Push 0 for now, come back and fix location jump.
			break_marker = put_location_marker(p_function_id);
		}

		conditional_get_token(TC_SEMICOLON, ERR_MISSING_SEMICOLON);

		if (token != TC_RIGHT_PAREN) {
			//parse_expression(p_function_id);
			parse_declarations_or_assignment(p_function_id);
		}

		conditional_get_token(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);
		parse_statement(p_function_id);
		this->emit(p_function_id, opcode::GOTO, for_start);
		this->emit(p_function_id, opcode::NOP);

		fixup_location_marker(p_function_id, break_marker);
		set_break_jump(p_function_id, for_start);
		symtab_stack.exit_scope();
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

		//    if ((p_function_id->defined.routine.function_type == func_std_iterator)) return;


		conditional_get_token_append(TC_RIGHT_BRACKET, ERR_MISSING_RIGHT_BRACKET);
	}

	/** parse_RETURN         parse return statements.
	*
	*      return;
	*      or
	*      return <expression>;
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_RETURN(symbol_table_node_ptr &p_function_id) {
		get_token();

		// TODO check for expressions

		if (p_function_id->p_type->typecode == T_VOID){
			// if token != ; (void function returning a value)
			this->emit(p_function_id, RETURN);
			return;
		}

		// expr 1
		check_assignment_type_compatible(p_function_id, p_function_id->p_type, parse_expression(p_function_id),
			ERR_INCOMPATIBLE_TYPES);

		this->emit_store(p_function_id, p_function_id);
		this->emit(p_function_id, RETURN);
	}

	/** parse_identifier_type     In a type defintion of the form
	*
	*                               <id-1> = <id-2>
	*
	*                          parse <id-2>.
	*
	* @param p_id2 : ptr to symbol table node of <id-2>.
	* @return ptr to type object of <id-2>.
	*/
	/*cx_type::type_ptr parser::parse_identifier_type(symbol_table_node_ptr &p_id2) {
		get_token();
		return p_id2->p_type;
	}*/

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

	/** parse_complex_type
	*
	* NOTE:
	*      Not implemented.
	*
	* @param p_function_id
	* @param p_node
	* @return
	*/
	type_ptr parser::parse_complex_type(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node) {

//	get_token();
//
//	// <id>
//	cx_type *p_type = new cx_type(F_REFERENCE, 0, nullptr);
//
	symbol_table_node_ptr next_type = p_node;
//
//	//while (next_type->next__)
//	//next_type = next_type->next__;
//
//	symbol_table_node_ptr p_id = enter_new_local(p_token->string__());
//	icode.put(p_id);
//
//	//next_type->next__ = p_id;
//
//	// Link the routine's local type id nodes together.
//	if (!p_function_id->defined.routine.locals.p_type_ids) {
//	p_function_id->defined.routine.locals.p_type_ids = p_id;
//	}
//	else {
//	symbol_table_node_ptr p_var_id = p_function_id->defined.routine.locals.p_type_ids;
//
//	while (p_var_id->next__)
//	p_var_id = p_var_id->next__;
//
//	p_var_id->next__ = p_id;
//	}
//
//	set_type(p_id->p_type, p_type);
//	p_id->defined.defined_how = DC_TYPE;
//
//	parse_member_decls(p_function_id, p_id->p_type, 0);

		return p_function_id->p_type;
	}

	/** parse_member_decls
	*
	* NOTE:
	*      Not implemented.
	*
	* @param p_function_id
	* @param p_complex_type
	* @param offset
	*/
	/*	void parser::parse_member_decls(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_complex_type, int offset) {
	// copy of base class would go here

	// if no '{' this must be a forward

	get_token();

	if (token == TC_SEMICOLON) {
	get_token_append();
	return;
	}

	// if '{' this is a class body

	conditional_get_token_append(TC_LEFT_BRACKET, ERR_MISSING_LEFT_BRACKET);

	// scope
	token_code scope;
	symbol_table_node_ptr member = nullptr;

	// pointer to scoped table
	cx_symtab *member_table = nullptr;

	symbol_table_node_ptr p_node = nullptr;

	symbol_table_node_ptr p_last_id = nullptr; // ptrs to symtab nodes
	symbol_table_node_ptr p_first_id = nullptr; // ptr to last node of previous sublist

	//    p_complex_type->complex.MemberTable.insert(pair<cx_token_code, cx_symtab *>(tc_PUBLIC, new cx_symtab));
	//   p_complex_type->complex.MemberTable.insert(pair<cx_token_code, cx_symtab *>(tc_PRIVATE, new cx_symtab));
	//   p_complex_type->complex.MemberTable.insert(pair<cx_token_code, cx_symtab *>(tc_PROTECTED, new cx_symtab));

	//default to public
	scope = TC_PUBLIC;

	do {

	// check scope and fast forward list to the end
	switch (this->token) {
	case tc_PUBLIC:
	scope = tc_PUBLIC;
	get_token();
	conditional_get_token(tc_colon, err_missing_colon);
	///p_first_id = p_complex_type->complex.MemberTable[scope]->root();

	offset = 0;
	while (p_first_id) {
	offset += p_first_id->p_type->size;
	p_first_id = p_first_id->next__;
	}

	break;
	case tc_PRIVATE:
	scope = tc_PRIVATE;
	get_token();
	conditional_get_token(tc_colon, err_missing_colon);
	//p_first_id = p_complex_type->complex.MemberTable[scope]->root();

	offset = 0;
	while (p_first_id) {
	offset += p_first_id->p_type->size;
	p_first_id = p_first_id->next__;
	}

	break;
	case tc_PROTECTED:
	scope = tc_PROTECTED;
	get_token();
	conditional_get_token(tc_colon, err_missing_colon);
	// p_first_id = p_complex_type->complex.MemberTable[scope]->root();

	offset = 0;
	while (p_first_id) {
	offset += p_first_id->p_type->size;
	p_first_id = p_first_id->next__;
	}

	break;
	default:
	break;
	}

	//member_table = p_complex_type->complex.MemberTable[scope];

	// find our declared type
	p_node = find(p_token->string__());

	// if complex then this is an object
	if (p_node->p_type->typeform == F_REFERENCE) {
	parse_complex_type(p_function_id, p_node);
	// predefined type name found
	}
	else if ((p_node->defined.defined_how == DC_TYPE) && (p_node->p_type->typeform != F_REFERENCE)) {
	do {
	get_token();

	// enter new local
	member = member_table->enter_new(p_token->string__());
	member->defined.defined_how = dc_member;

	// set type
	set_type(member->p_type, p_node->p_type);

	// Record fields
	//                member->defined.data.offset = offset;
	offset += p_node->p_type->size;

	get_token_append();

	// check for array type
	// check for array type
	if (token == TC_LEFT_SUBSCRIPT) {
	//parse_array_type(member);
	member->defined.defined_how = dc_variable;
	}
	else if (token == tc_left_paren) {
	parse_function_header(member);
	}
	else if (token != tc_comma) {
	// check for assignment
	//parse_assignment(p_new_id);
	member->defined.defined_how = dc_variable;
	}

	if (!p_first_id) p_first_id = p_last_id = member;
	else {
	p_last_id->next__ = member;
	p_last_id = member;
	}

	} while (token == tc_comma);

	conditional_get_token(TC_SEMICOLON, err_missing_semicolon);
	}



	} while (token != tc_right_bracket);

	// connect all symtabs for use within the class
	p_complex_type->complex.p_class_scope = new cx_symtab;
	// p_complex_type->complex.p_class_scope_symtab->connect_tables(p_complex_type->complex.MemberTable);

	conditional_get_token_append(tc_right_bracket, err_missing_right_bracket);
	conditional_get_token_append(TC_SEMICOLON, err_missing_semicolon);
	}*/

	void parser::emit_const(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id){
		opcode op;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_INT:
			op = ICONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.i_ });
			break;
		case T_CHAR:
			op = ICONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.c_ });
			break;
		case T_DOUBLE:
			op = DCONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.d_ });
			break;
		case T_REFERENCE:
			op = ACONST_NULL;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant_value.a_ });
			break;
		default:
			break;
		}
	}

	void parser::emit_not_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = INOT_EQ;
			break;
		case T_DOUBLE:
			op = DNOT_EQ;
			break;
		case T_REFERENCE: // TODO fix
			//op = ANOT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_lt_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ILT_EQ;
			break;
		case T_DOUBLE:
			op = DLT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}


	void parser::emit_eq_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BYTE:
			op = opcode::BEQ;
			break;
		case T_BOOLEAN:
			op = opcode::ZEQ;
			break;
		case T_CHAR:
		case T_INT:
			op = opcode::IEQ;
			break;
		case T_DOUBLE:
			op = opcode::DEQ;
			break;
		case T_REFERENCE: // TODO fix
			//op = ANOT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_gt(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IGT;
			break;
		case T_DOUBLE:
			op = DGT;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_gt_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IGT_EQ;
			break;
		case T_DOUBLE:
			op = DGT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_lt(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ILT;
			break;
		case T_DOUBLE:
			op = DLT;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_add(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IADD;
			break;
		case T_DOUBLE:
			op = DADD;
			break;
		case T_REFERENCE: // TODO fix
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_div(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IDIV;
			break;
		case T_DOUBLE:
			op = DDIV;
			break;
		case T_REFERENCE: // TODO fix
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_sub(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ISUB;
			break;
		case T_DOUBLE:
			op = DSUB;
			break;
		case T_REFERENCE: // TODO fix
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_mod(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IREM;
			break;
		case T_DOUBLE:
			op = DREM;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_mul(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = IMUL;
			break;
		case T_DOUBLE:
			op = DMUL;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_lnot(symbol_table_node_ptr &p_function_id, type_ptr &p_type){
		cx::opcode op = LOGIC_NOT;

		switch (p_type->typecode)
		{
		case T_DOUBLE:
			p_function_id->defined.routine.program_code.push_back(D2I);
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit(symbol_table_node_ptr &p_function_id, cx::opcode op1){
		p_function_id->defined.routine.program_code.push_back(op1);
	}

	void parser::emit(symbol_table_node_ptr &p_function_id, cx::opcode op1, value arg1){
		p_function_id->defined.routine.program_code.push_back({ op1, arg1 });
	}

	void parser::emit(symbol_table_node_ptr &p_function_id, cx::opcode op1, value arg1, value arg2){
		p_function_id->defined.routine.program_code.push_back({ op1, arg1, arg2 });
	}

	void parser::emit_store_no_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id){
		opcode op = opcode::NOP;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ISTORE;
			break;
		case T_DOUBLE:
			op = DSTORE;
			break;
		case T_REFERENCE:
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_store(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id){
		opcode op = opcode::NOP;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ISTORE;
			break;
		case T_DOUBLE:
			op = DSTORE;
			break;
		case T_REFERENCE:
			op = ASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id){
		opcode op = opcode::NOP;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:

		case T_BYTE:
		case T_CHAR:
		case T_INT:
			op = ILOAD;
			break;
		case T_DOUBLE:
			op = DLOAD;
			break;
		case T_REFERENCE:
			op = ALOAD;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_ax_load(symbol_table_node_ptr &p_function_id, 
		symbol_table_node_ptr &p_id,
		type_ptr &p_type) {
		opcode op = opcode::NOP;

		switch (p_type->typecode)
		{
		case T_BYTE:
			op = BALOAD;
			break;
		case T_CHAR:
			op = CALOAD;
			break;
		case T_BOOLEAN:
		case T_INT:
			op = IALOAD;
			break;
		case T_DOUBLE:
			op = DALOAD;
			break;
		case T_REFERENCE:
			op = AALOAD;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get(), p_type.get() });
	}

	void parser::emit_ax_store(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id) {
		opcode op = opcode::NOP;

		switch (p_id->p_type->base_type()->typecode)
		{
		case T_BYTE:
			op = BASTORE;
			break;
		case T_CHAR:
			op = CASTORE;
			break;
		case T_BOOLEAN:
		case T_INT:
			op = IASTORE;
			break;
		case T_DOUBLE:
			op = DASTORE;
			break;
		case T_REFERENCE:
			op = AASTORE;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back({ op, p_id.get() });
	}

	void parser::emit_inc(symbol_table_node_ptr &p_function_id, type_ptr &p_type, value v_){

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
		case T_INT:
			this->emit(p_function_id, IINC, { v_.i_ });
			break;
		case T_DOUBLE:
			this->emit(p_function_id, DCONST, { v_.d_ });
			this->emit(p_function_id, DADD);
			break;
		default:
			break;
		}
	}

	void parser::parse_ASM(symbol_table_node_ptr &p_function_id) {
		symtab_stack.enter_scope();

		get_token();

		if (p_function_id != nullptr) {}

		std::pair<std::wstring, cx::opcode> opcode_map_data[] = {
			std::make_pair(L"aaload",          cx::opcode::AALOAD),
			std::make_pair(L"aastore",         cx::opcode::AASTORE),
			std::make_pair(L"aconst_null",     cx::opcode::ACONST_NULL),
			std::make_pair(L"aload",           cx::opcode::ALOAD),
			std::make_pair(L"anewarray",       cx::opcode::ANEWARRAY),
			std::make_pair(L"arraylength",     cx::opcode::ARRAYLENGTH),
			std::make_pair(L"astore",          cx::opcode::ASTORE),
			std::make_pair(L"vm_throw",          cx::opcode::VM_THROW),
			std::make_pair(L"baload",          cx::opcode::BALOAD),
			std::make_pair(L"bastore",         cx::opcode::BASTORE),
			std::make_pair(L"beq",             cx::opcode::BEQ),
			std::make_pair(L"bipush",          cx::opcode::BIPUSH),
			std::make_pair(L"c2i",             cx::opcode::C2I),
			std::make_pair(L"call",            cx::opcode::CALL),
			std::make_pair(L"caload",          cx::opcode::CALOAD),
			std::make_pair(L"castore",         cx::opcode::CASTORE),
			std::make_pair(L"checkcast",       cx::opcode::CHECKCAST),
			std::make_pair(L"d2f",             cx::opcode::D2F),
			std::make_pair(L"d2i",             cx::opcode::D2I),
			std::make_pair(L"d2l",             cx::opcode::D2L),
			std::make_pair(L"dadd",            cx::opcode::DADD),
			std::make_pair(L"daload",          cx::opcode::DALOAD),
			std::make_pair(L"dastore",         cx::opcode::DASTORE),
			std::make_pair(L"dcmp",            cx::opcode::DCMP),
			std::make_pair(L"dconst",          cx::opcode::DCONST),
			std::make_pair(L"ddiv",            cx::opcode::DDIV),
			std::make_pair(L"deq_eq",          cx::opcode::DEQ),
			std::make_pair(L"dgt",             cx::opcode::DGT),
			std::make_pair(L"dgt_eq",          cx::opcode::DGT_EQ),
			std::make_pair(L"dinc",            cx::opcode::DINC),
			std::make_pair(L"dload",           cx::opcode::DLOAD),
			std::make_pair(L"dlt",             cx::opcode::DLT),
			std::make_pair(L"dlt_eq",          cx::opcode::DLT_EQ),
			std::make_pair(L"dmul",            cx::opcode::DMUL),
			std::make_pair(L"dneg",            cx::opcode::DNEG),
			std::make_pair(L"dnot_eq",         cx::opcode::DNOT_EQ),
			std::make_pair(L"drem",            cx::opcode::DREM),
			std::make_pair(L"dstore",          cx::opcode::DSTORE),
			std::make_pair(L"dsub",            cx::opcode::DSUB),
			std::make_pair(L"dup",             cx::opcode::DUP),
			std::make_pair(L"dup2",            cx::opcode::DUP2),
			std::make_pair(L"dup2_x1",         cx::opcode::DUP2_X1),
			std::make_pair(L"dup2_x2",         cx::opcode::DUP2_X2),
			std::make_pair(L"dup_x1",          cx::opcode::DUP_X1),
			std::make_pair(L"dup_x2",          cx::opcode::DUP_X2),
			std::make_pair(L"getfield",        cx::opcode::GETFIELD),
			std::make_pair(L"getstatic",       cx::opcode::GETSTATIC),
			std::make_pair(L"goto",            cx::opcode::GOTO),
			std::make_pair(L"i2b",             cx::opcode::I2B),
			std::make_pair(L"i2c",             cx::opcode::I2C),
			std::make_pair(L"i2d",             cx::opcode::I2D),
			std::make_pair(L"iadd",            cx::opcode::IADD),
			std::make_pair(L"iaload",          cx::opcode::IALOAD),
			std::make_pair(L"iand",            cx::opcode::IAND),
			std::make_pair(L"iastore",         cx::opcode::IASTORE),
			std::make_pair(L"icmp",            cx::opcode::ICMP),
			std::make_pair(L"iconst",          cx::opcode::ICONST),
			std::make_pair(L"idiv",            cx::opcode::IDIV),
			std::make_pair(L"ieq_eq",          cx::opcode::IEQ),
			std::make_pair(L"if_false",        cx::opcode::IF_FALSE),
			std::make_pair(L"ifne",            cx::opcode::IFNE),
			std::make_pair(L"iflt",            cx::opcode::IFLT),
			std::make_pair(L"ifge",            cx::opcode::IFGE),
			std::make_pair(L"ifgt",            cx::opcode::IFGT),
			std::make_pair(L"ifle",            cx::opcode::IFLE),
			std::make_pair(L"if_acmpeq",       cx::opcode::IF_ACMPEQ),
			std::make_pair(L"if_acmpne",       cx::opcode::IF_ACMPNE),
			std::make_pair(L"if_icmpeq",       cx::opcode::IF_ICMPEQ),
			std::make_pair(L"if_icmpne",       cx::opcode::IF_ICMPNE),
			std::make_pair(L"if_icmplt",       cx::opcode::IF_ICMPLT),
			std::make_pair(L"if_icmpge",       cx::opcode::IF_ICMPGE),
			std::make_pair(L"if_icmpgt",       cx::opcode::IF_ICMPGT),
			std::make_pair(L"if_icmple",       cx::opcode::IF_ICMPLE),
			std::make_pair(L"ifnonnull",       cx::opcode::IFNONNULL),
			std::make_pair(L"ifnull",          cx::opcode::IFNULL),
			std::make_pair(L"igt",             cx::opcode::IGT),
			std::make_pair(L"igt_eq",          cx::opcode::IGT_EQ),
			std::make_pair(L"iinc",            cx::opcode::IINC),
			std::make_pair(L"iload",           cx::opcode::ILOAD),
			std::make_pair(L"ilt",             cx::opcode::ILT),
			std::make_pair(L"ilt_eq",          cx::opcode::ILT_EQ),
			std::make_pair(L"imul",            cx::opcode::IMUL),
			std::make_pair(L"ineg",            cx::opcode::INEG),
			std::make_pair(L"inot",            cx::opcode::INOT),
			std::make_pair(L"inot_eq",         cx::opcode::INOT_EQ),
			std::make_pair(L"instanceof",      cx::opcode::INSTANCEOF),
			std::make_pair(L"invokedynamic",   cx::opcode::INVOKEDYNAMIC),
			std::make_pair(L"invokefunct",     cx::opcode::INVOKEFUNCT),
			std::make_pair(L"invokeinterface", cx::opcode::INVOKEINTERFACE),
			std::make_pair(L"invokespecial",   cx::opcode::INVOKESPECIAL),
			std::make_pair(L"invokestatic",    cx::opcode::INVOKESTATIC),
			std::make_pair(L"invokevirtual",   cx::opcode::INVOKEVIRTUAL),
			std::make_pair(L"ior",             cx::opcode::IOR),
			std::make_pair(L"irem",            cx::opcode::IREM),
			std::make_pair(L"ishl",            cx::opcode::ISHL),
			std::make_pair(L"ishr",            cx::opcode::ISHR),
			std::make_pair(L"istore",          cx::opcode::ISTORE),
			std::make_pair(L"isub",            cx::opcode::ISUB),
			std::make_pair(L"ixor",            cx::opcode::IXOR),
			std::make_pair(L"jsr",             cx::opcode::JSR),
			std::make_pair(L"jsr_w",           cx::opcode::JSR_W),
			std::make_pair(L"ldc",             cx::opcode::LDC),
			std::make_pair(L"ldc2_w",          cx::opcode::LDC2_W),
			std::make_pair(L"ldc_w",           cx::opcode::LDC_W),
			std::make_pair(L"lookupswitch",    cx::opcode::LOOKUPSWITCH),
			std::make_pair(L"logic_or",        cx::opcode::LOGIC_OR),
			std::make_pair(L"logic_and",       cx::opcode::LOGIC_AND),
			std::make_pair(L"logic_not",       cx::opcode::LOGIC_NOT),
			std::make_pair(L"monitorenter",    cx::opcode::MONITORENTER),
			std::make_pair(L"monitorexit",     cx::opcode::MONITOREXIT),
			std::make_pair(L"multianewarray",  cx::opcode::MULTIANEWARRAY),
			std::make_pair(L"new",             cx::opcode::NEW),
			std::make_pair(L"newarray",        cx::opcode::NEWARRAY),
			std::make_pair(L"nop",             cx::opcode::NOP),
			std::make_pair(L"pload",           cx::opcode::PLOAD),
			std::make_pair(L"postop",          cx::opcode::POSTOP),
			std::make_pair(L"pop",             cx::opcode::POP),
			std::make_pair(L"pop2",            cx::opcode::POP2),
			std::make_pair(L"preop",           cx::opcode::PREOP),
			std::make_pair(L"putfield",        cx::opcode::PUTFIELD),
			std::make_pair(L"putstatic",       cx::opcode::PUTSTATIC),
			std::make_pair(L"return",          cx::opcode::RETURN),
			std::make_pair(L"ret",			   cx::opcode::RETURN),
			std::make_pair(L"swap",            cx::opcode::SWAP),
			std::make_pair(L"tableswitch",     cx::opcode::TABLESWITCH),
			std::make_pair(L"zeq",			   cx::opcode::ZEQ)
		};

		typedef std::map<std::wstring, cx::opcode> opcode_map;
		opcode_map cxvm_opcodes(opcode_map_data, opcode_map_data + sizeof opcode_map_data / sizeof opcode_map_data[0]);

		bool is_block = (token == TC_LEFT_BRACKET);
		if (is_block) get_token();

		do {

			opcode op = opcode::NOP;

			if (token != TC_IDENTIFIER) {
				cx_error(ERR_INVALID_OPCODE);
			}
			else {
				op = cxvm_opcodes[this->p_token->string];
			}

			switch (op) {
			case opcode::AALOAD: get_token(); break;
			case opcode::AASTORE: get_token(); break;
			case opcode::ACONST_NULL: get_token(); break;
			case opcode::ALOAD: get_token(); break;
			case opcode::ANEWARRAY: get_token(); break;
			case opcode::ARRAYLENGTH: get_token(); break;
			case opcode::ASTORE: get_token(); break;
			case opcode::VM_THROW: get_token(); break;
			case opcode::BALOAD: get_token(); break;
			case opcode::BASTORE: get_token(); break;
			case opcode::BEQ: get_token(); break;
			case opcode::BIPUSH: get_token(); break;
			case opcode::C2I: get_token(); break;
			case opcode::CALL: get_token(); break;
			case opcode::CALOAD: get_token(); break;
			case opcode::CASTORE: get_token(); break;
			case opcode::CHECKCAST: get_token(); break;
			case opcode::D2F: get_token(); break;
			case opcode::D2I: get_token(); break;
			case opcode::D2L: get_token(); break;
			case opcode::DADD: get_token(); break;
			case opcode::DALOAD: get_token(); break;
			case opcode::DASTORE: get_token(); break;
			case opcode::DCMP: get_token(); break;
			case opcode::DCONST: get_token(); break;
			case opcode::DDIV: get_token(); break;
			case opcode::DEQ: get_token(); break;
			case opcode::DGT: get_token(); break;
			case opcode::DGT_EQ: get_token(); break;
			case opcode::DINC: get_token(); break;
			case opcode::DLOAD: get_token(); break;
			case opcode::DLT: get_token(); break;
			case opcode::DLT_EQ: get_token(); break;
			case opcode::DMUL: get_token(); break;
			case opcode::DNEG: get_token(); break;
			case opcode::DNOT_EQ: get_token(); break;
			case opcode::DREM: get_token(); break;
			case opcode::DSTORE: get_token(); break;
			case opcode::DSUB: get_token(); break;
			case opcode::DUP: get_token(); break;
			case opcode::DUP2: get_token(); break;
			case opcode::DUP2_X1: get_token(); break;
			case opcode::DUP2_X2: get_token(); break;
			case opcode::DUP_X1: get_token(); break;
			case opcode::DUP_X2: get_token(); break;
			case opcode::GETFIELD: get_token(); break;
			case opcode::GETSTATIC: get_token(); break;
			case opcode::GOTO: get_token(); break;
			case opcode::I2B: get_token(); break;
			case opcode::I2C: get_token(); break;
			case opcode::I2D: get_token(); break;
			case opcode::IADD: 
				this->emit(p_function_id, opcode::IADD);
				break;
			case opcode::IALOAD: get_token(); break;
			case opcode::IAND: get_token(); break;
			case opcode::IASTORE: get_token(); break;
			case opcode::ICMP: get_token(); break;
			case opcode::ICONST: get_token(); 
				if (token != TC_NUMBER) cx_error(ERR_INVALID_NUMBER);
				this->emit(p_function_id, opcode::ICONST, { p_token->value().i_ });
				break;
			case opcode::IDIV: get_token(); break;
			case opcode::IEQ: get_token(); break;
			case opcode::IF_FALSE: get_token(); break;
			case opcode::IFNE: get_token(); break;
			case opcode::IFLT: get_token(); break;
			case opcode::IFGE: get_token(); break;
			case opcode::IFGT: get_token(); break;
			case opcode::IFLE: get_token(); break;
			case opcode::IF_ACMPEQ: get_token(); break;
			case opcode::IF_ACMPNE: get_token(); break;
			case opcode::IF_ICMPEQ: get_token(); break;
			case opcode::IF_ICMPNE: get_token(); break;
			case opcode::IF_ICMPLT: get_token(); break;
			case opcode::IF_ICMPGE: get_token(); break;
			case opcode::IF_ICMPGT: get_token(); break;
			case opcode::IF_ICMPLE: get_token(); break;
			case opcode::IFNONNULL: get_token(); break;
			case opcode::IFNULL: get_token(); break;
			case opcode::IGT: get_token(); break;
			case opcode::IGT_EQ: get_token(); break;
			case opcode::IINC: get_token(); break;
			case opcode::ILOAD: get_token(); {
				if (token != TC_IDENTIFIER) cx_error(ERR_MISSING_IDENTIFIER);
				symbol_table_node_ptr p_node = search_all(p_token->string);
				if (p_node == nullptr) cx_error(ERR_UNDEFINED_IDENTIFIER);
				this->emit(p_function_id, opcode::ILOAD, p_node.get());
			}
			break;
			case opcode::ILT: get_token(); break;
			case opcode::ILT_EQ: get_token(); break;
			case opcode::IMUL: get_token(); break;
			case opcode::INEG: get_token(); break;
			case opcode::INOT: get_token(); break;
			case opcode::INOT_EQ: get_token(); break;
			case opcode::INSTANCEOF: get_token(); break;
			case opcode::INVOKEDYNAMIC: get_token(); break;
			case opcode::INVOKEFUNCT: get_token(); break;
			case opcode::INVOKEINTERFACE: get_token(); break;
			case opcode::INVOKESPECIAL: get_token(); break;
			case opcode::INVOKESTATIC: get_token(); break;
			case opcode::INVOKEVIRTUAL: get_token(); break;
			case opcode::IOR: get_token(); break;
			case opcode::IREM: get_token(); break;
			case opcode::ISHL: get_token(); break;
			case opcode::ISHR: get_token(); break;
			case opcode::ISTORE: get_token(); {
				if (token != TC_IDENTIFIER) cx_error(ERR_MISSING_IDENTIFIER);
				symbol_table_node_ptr p_node = search_all(p_token->string);
				if(p_node == nullptr) cx_error(ERR_UNDEFINED_IDENTIFIER);
				this->emit(p_function_id, opcode::ISTORE, p_node.get());
			}
				break;
			case opcode::ISUB: get_token(); break;
			case opcode::IXOR: get_token(); break;
			case opcode::JSR: get_token(); break;
			case opcode::JSR_W: get_token(); break;
			case opcode::LDC: get_token(); break;
			case opcode::LDC2_W: get_token(); break;
			case opcode::LDC_W: get_token(); break;
			case opcode::LOOKUPSWITCH: get_token(); break;
			case opcode::LOGIC_OR: get_token(); break;
			case opcode::LOGIC_AND: get_token(); break;
			case opcode::LOGIC_NOT: get_token(); break;
			case opcode::MONITORENTER: get_token(); break;
			case opcode::MONITOREXIT: get_token(); break;
			case opcode::MULTIANEWARRAY: get_token(); break;
			case opcode::NEW: get_token(); break;
			case opcode::NEWARRAY: get_token(); break;
			case opcode::NOP: get_token(); break;
			case opcode::PLOAD: get_token(); break;
			case opcode::POSTOP: get_token(); break;
			case opcode::POP: get_token(); break;
			case opcode::POP2: get_token(); break;
			case opcode::PREOP: get_token(); break;
			case opcode::PUTFIELD: get_token(); break;
			case opcode::PUTSTATIC: get_token(); break;
			case opcode::RETURN: 
				this->emit(p_function_id, opcode::RETURN);
				break;
			case opcode::SWAP: get_token(); break;
			case opcode::TABLESWITCH: get_token(); break;
			default:
				cx_error(ERR_INVALID_OPCODE);
				break;
			}

			get_token();
		} while (is_block && token != TC_RIGHT_BRACKET);

		if (is_block) conditional_get_token(TC_RIGHT_BRACKET, ERR_MISSING_RIGHT_BRACKET);
		else conditional_get_token(TC_SEMICOLON, ERR_MISSING_SEMICOLON);

		symtab_stack.exit_scope();
	}
}