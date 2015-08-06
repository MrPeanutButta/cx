#include <iostream>
#include <cstdio>
#//include "common.h"
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
	extern bool cx_dev_debug_flag;
	// might not always be true in some cases.
	bool exec_flag(true);

	symbol_table_node_ptr p_program_ptr_id;// = nullptr;
	/** parse       parse the source file.  After listing each
	 *              source line, extract and list its tokens.
	 *
	 * @return ptr to '__cx_global__' program Id.
	 */
	symbol_table_node_ptr parser::parse(void) {

		extern bool cx_dev_debug_flag;
		symbol_table_node_ptr p_program_id;// = nullptr;

		if (!is_module) {
			p_program_id = std::make_shared<symbol_table_node>(L"__main__", DC_PROGRAM);
			p_program_id->defined.routine.function_type = FUNC_DECLARED;
			p_program_id->p_type = p_integer_type;

			p_program_ptr_id = p_program_id;
		}
		current_nesting_level = 0;
		// enter the nesting level 0 and open a new scope for the program.
		//symtab_stack.set_current_symtab(p_global_symbol_table);

		//if (!is_module) icode.put(TC_LEFT_BRACKET);
		get_token();

		parse_statement_list(p_program_id, TC_END_OF_FILE);

		get_token();

		if (!is_module) {
			p_program_id->defined.routine.p_symtab = p_global_symbol_table;

			resync(tokenlist_program_end);
			conditional_get_token_append(TC_END_OF_FILE, ERR_MISSING_RIGHT_BRACKET);

			if (cx_dev_debug_flag) {
				//list.put_line();
				swprintf(list.text, L"%20d source lines.", current_line_number);
				list.put_line();
				swprintf(list.text, L"%20d syntax errors.", error_count);
				list.put_line();
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
				get_token_append();
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

		// track if we seen '*'
		bool is_unk_array_size = false;
		symbol_table_node_ptr p_node = find(p_token->string);
		cx_type::type_ptr assignment_expression_ptr = nullptr;
		
		// if complex then this is an object
		if (p_node->p_type->typeform == F_REFERENCE) {
			//parse_complex_type(p_function_id, p_node);
			//			// predefined type name found
		}
		else if ((p_node->defined.defined_how == DC_TYPE) && (p_node->p_type->typeform != F_REFERENCE) &&
			(p_node->defined.defined_how != DC_FUNCTION)) {

			get_token();

			if (token == TC_STAR) {
				get_token();
				is_unk_array_size = true;
			}

			do {
				while (token == TC_COMMA)get_token();

				symbol_table_node_ptr p_new_id;

				p_new_id = search_local(p_token->string);

				/* if not nullptr, it's already defined.
				 * check if forwarded */
				if (p_new_id != nullptr) {
					if (p_new_id->defined.defined_how == DC_FUNCTION &&
						p_new_id->defined.routine.function_type == FUNC_FORWARD) {
						get_token();
						parse_function_header(p_new_id);
					}
					else cx_error(ERR_REDEFINED_IDENTIFIER);
				}
				else {
					p_new_id = enter_new_local(p_token->string);
				}

				// set type
				p_new_id->p_type = p_node->p_type;
				get_token();

				// check for array type
				if (token == TC_LEFT_SUBSCRIPT) {
					parse_array_type(p_function_id, p_new_id);
				}
				else if (is_unk_array_size) {
					// TODO parse unkown array size
					parse_unksize_array_type(p_function_id, p_new_id);
				}
				else if (token == TC_LEFT_PAREN) {
					parse_function_header(p_new_id);
				}
				else if ((token != TC_COMMA) && (token != TC_END_OF_FILE)) {

					// check for assignment
					assignment_expression_ptr = parse_assignment(p_function_id, p_new_id);
					p_new_id->defined.defined_how = DC_VARIABLE;
				}

				if (p_new_id->defined.defined_how == DC_VARIABLE) {
					// add variable to variable list
					if (p_function_id != nullptr) {
						p_function_id->defined.routine.p_variable_ids.emplace_back(p_new_id);
					}
					// add function to routine list
				}
				else if (p_new_id->defined.defined_how == DC_FUNCTION) {
					if (p_function_id != nullptr) {
						p_function_id->defined.routine.p_function_ids.emplace_back(p_new_id);
					}
				}

			} while (token == TC_COMMA);
		}
		else if (p_node->defined.defined_how == DC_FUNCTION) {

			get_token();

			if (token == TC_LEFT_PAREN) {
				parse_subroutine_call(p_function_id, p_node);
			}
			else {
				parse_assignment(p_function_id, p_node);
			}
		}
		else {
			get_token();
			parse_assignment(p_function_id, p_node);
		}
	}

	/** parse_constant_declaration    'const' will only set it's qualifier as
	*                              dc_constant all else is treated as a standard
	*                              declaration.
	*      const <type> <name>;
	*
	* @param p_function_id    ptr to the routine which owns the type being declared or
	*                      assigned a constant value.
	*/
	void parser::parse_constant_declaration(symbol_table_node_ptr &p_function_id) {
		//symbol_table_node_ptr &p_last_id = nullptr;
		//symbol_table_node_ptr &p_const_id = nullptr;
		//symbol_table_node_ptr &p_type_node = find(p_token->string__());

		//get_token_append();

		//p_const_id = enter_new_local(p_token->string__());

		//if (!p_function_id->defined.routine.locals.p_constant_ids) {
		//	p_function_id->defined.routine.locals.p_constant_ids = p_const_id;
		//}
		//else {

		//	p_last_id = p_function_id->defined.routine.locals.p_constant_ids;

		//	while (p_last_id->next__)
		//		p_last_id = p_last_id->next__;

		//	p_last_id->next__ = p_const_id;

		//}

		//get_token_append();
		//conditional_get_token(tc_equal, err_missing_equal);

		//set_type(p_const_id->p_type, p_type_node->p_type);
		//parse_constant(p_const_id);
		//p_const_id->defined.defined_how = dc_constant;

		//resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);

	}

	/** parse_constant       parse a constant.
	*
	* @param p_const_id : ptr to symbol table node of the identifier
	*                   being defined
	*/
	void parser::parse_constant(symbol_table_node_ptr &p_const_id) {
		//token_code sign = tc_dummy;

		//if (token_in(token, tokenlist_unary_ops)) {
		//	if (token == tc_minus) sign = tc_minus;
		//	get_token_append();
		//}

		//switch (token) {
		//case tc_number:
		//	if ((p_token->type() == ty_integer) && (p_const_id->p_type == p_integer_type)) {
		//		p_const_id->defined.constant.value.int__ = sign == tc_minus ?
		//			-p_token->value().int__ : p_token->value().int__;
		//	}
		//	else if ((p_token->type() == ty_real) &&
		//		(((p_const_id->p_type == p_float_type)))) {

		//		if (p_const_id->p_type == p_float_type) {
		//			p_const_id->defined.constant.value.float__ = sign == tc_minus ?
		//				-p_token->value().float__ : p_token->value().float__;
		//		}
		//		else {
		//			//                    p_const_id->defined.constant.value.double__ = sign == tc_minus ?
		//			//                            -p_token->value().float__ : p_token->value().float__;
		//		}
		//	}

		//	get_token_append();
		//	break;

		//case tc_identifier:
		//	parse_identifier_constant(p_const_id, sign);
		//	break;
		//case tc_char:
		//case tc_string:
		//	if (p_const_id->p_type == p_char_type) {
		//		int length = strlen(p_token->string__()) - 2;

		//		if (sign != tc_dummy) cx_error(err_invalid_constant);

		//		if (length == 1) {
		//			p_const_id->defined.constant.value.char__ = p_token->string__()[1];

		//			//set_type(p_const_id->p_type, p_char_type);
		//		}
		//		else {
		//			char *p_string = new char[length];
		//			copy_quoted_string(p_string, p_token->string__());

		//			p_const_id->defined.constant.value.addr__ = (void *)p_string;

		//			//set_type(p_const_id->p_type, new cx_type(length));
		//		}

		//		get_token_append();
		//	}
		//	else cx_error(err_invalid_type);
		//	break;
		//default:
		//	break;
		//}
	}

	/** parse_identifier_constant     In a constant definition of the
	*                              form
	*
	*                                      <id-1> = <id-2>
	*
	*                              parse <id-2>. The type can be
	*                              integer, real, character,
	*                              enumeration, or string
	*                              (character array).
	*
	* @param p_id1 : ptr to symbol table node of <id-1>.
	* @param sign : unary + or - sign, or none.
	*/
	void parser::parse_identifier_constant(symbol_table_node_ptr &p_id1, token_code sign) {
		//symbol_table_node_ptr &p_id2 = find(p_token->string__());

		//if (p_id2->defined.defined_how != dc_constant) {
		//	cx_error(err_not_a_constant_identifier);
		//	set_type(p_id1->p_type, p_dummy_type);
		//	get_token_append();
		//	return;
		//}

		//if (p_id2->p_type == p_integer_type) {
		//	p_id2->defined.constant.value.int__ = sign == tc_minus ?
		//		-p_id2->defined.constant.value.int__ :
		//		p_id2->defined.constant.value.int__;

		//	set_type(p_id1->p_type, p_integer_type);
		//}
		//else if (p_id2->p_type == p_float_type) {
		//	p_id1->defined.constant.value.float__ = sign == tc_minus ?
		//		-p_id2->defined.constant.value.float__ :
		//		p_id2->defined.constant.value.float__;
		//	set_type(p_id1->p_type, p_float_type);
		//}
		//else if (p_id2->p_type == p_char_type) {
		//	if (sign != tc_dummy) cx_error(err_invalid_constant);

		//	p_id1->defined.constant.value.char__ = p_id2->defined.constant.value.char__;

		//	set_type(p_id1->p_type, p_char_type);
		//}
		//else if (p_id2->p_type->typeform == fc_enum) {
		//	if (sign != tc_dummy)cx_error(err_invalid_constant);

		//	p_id1->defined.constant.value.int__ = p_id2->defined.constant.value.int__;

		//	set_type(p_id1->p_type, p_id2->p_type);
		//}
		//else if (p_id2->p_type->typeform == F_ARRAY) {
		//	if ((sign != tc_dummy) || (p_id2->p_type->array.p_element_type != p_char_type)) {
		//		cx_error(err_invalid_constant);
		//	}

		//	p_id1->defined.constant.value.addr__ = p_id2->defined.constant.value.addr__;

		//	set_type(p_id1->p_type, p_id2->p_type);
		//}

		//get_token_append();
	}

	// env variable that holds the path to stdlib
#define __CX_STDLIB__   L"CX_STDLIB"

	void load_lib(const wchar_t *lib, symbol_table *p_symtab) {

		static const cx_type * cx_types_[] = {
			p_integer_type.get(),
			p_char_type.get(),
			//p_wchar_type.get(),
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
	void parser::parse_execute_directive(symbol_table_node_ptr &p_function_id) {

		//	extern symbol_table_node_ptr p_program_ptr_id;

		switch (token) {
		case TC_INCLUDE:
		{
			std::wstring lib_path;
			wchar_t *env_path = _wgetenv(__CX_STDLIB__);

			if (env_path != nullptr) {
				lib_path = env_path;
#ifdef _WIN32
				lib_path += L"\\";
#else
				lib_path += L"/";
#endif
			}

			get_token();

			lib_path += p_token->string;
			p_program_ptr_id->found_global_end = true;

			// save current line number
			int old_line_num = current_line_number;
			current_line_number = 0;

			/* true : stdlib module
			* returns nullptr */
			{
				std::unique_ptr<parser> p_parser;// = nullptr;

				try {
					p_parser = std::make_unique<parser>(new source_buffer(lib_path.c_str()), true);

					p_parser->parse();
				}
				catch (...) {

				}

				//delete p_parser;
			}

			current_line_number = old_line_num;
			//icode.reset();
			//icode.put(tc_left_bracket);
			p_program_ptr_id->found_global_end = false;
			get_token_append();
		}
		break;
		case TC_WARN:
		{
			get_token();

			if (token != TC_STRING) {
				cx_error(ERR_UNEXPECTED_TOKEN);
			}

			{
				std::wstring msg = p_token->string;
				msg[0] = ' ';
				msg[msg.length() - 1] = L'\0';
				std::wcerr << "warning:" << msg << std::endl;
			}

			get_token_append();
		}
		break;
		case TC_IMPORT:
		{
			get_token();

			load_lib(p_token->string, symtab_stack.get_current_symtab());

			get_token_append();
		}
		break;
		default:
			break;
		}
	}

	/** parse_expression     parse an expression (binary relational
	*                      operators == < > <> <= and >= ).
	*
	* @return ptr to the expression's type object.
	*/
	cx_type::type_ptr parser::parse_expression(symbol_table_node_ptr &p_function_id) {

		cx_type::type_ptr p_result_type;
		cx_type::type_ptr p_operand_type;
		token_code op;

		p_result_type = parse_simple_expression(p_function_id);

		if (token_in(token, tokenlist_relation_ops)) {
			op = token;
			get_token();
			p_operand_type = parse_simple_expression(p_function_id);

			check_relational_op_operands(p_function_id, p_result_type, p_operand_type);

			p_result_type = p_boolean_type;

			switch (op){
			case TC_LESSTHAN:
				this->emit_lt(p_function_id, p_operand_type);
				break;
			case TC_GREATERTHAN:
				this->emit_gt(p_function_id, p_operand_type);
				break;
			case TC_LESSTHAN_EQUAL:
				this->emit_lt_eq(p_function_id, p_operand_type);
				break;
			case TC_GREATERTHAN_EQUAL:
				this->emit_gt_eq(p_function_id, p_operand_type);
				break;
			case TC_NOT_EQUAL:
				this->emit_not_eq(p_function_id, p_operand_type);
				break;
			case TC_EQUAL_EQUAL:
				this->emit_eq_eq(p_function_id, p_operand_type);
				break;
			}
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
	cx_type::type_ptr parser::parse_simple_expression(symbol_table_node_ptr &p_function_id) {

		cx_type::type_ptr p_result_type;
		cx_type::type_ptr p_operand_type;
		token_code op;
		token_code unary_op;

		bool unary_op_flag = false;

		if (token_in(token, tokenlist_unary_ops)) {
			unary_op_flag = true;
			unary_op = token;
			get_token();
		}

		p_result_type = parse_term(p_function_id);

		// TODO ++ -- should be in this section
		if (unary_op_flag) {
			switch (unary_op){
			case TC_LOGIC_NOT:
				this->emit_lnot(p_function_id, p_result_type);
				p_result_type = p_boolean_type;
				break;
			case TC_BIT_NOT:
				check_bitwise_integer(p_result_type);
				this->emit(p_function_id, { INOT });
				break;
			case TC_PLUS: // TODO use ABS?
				check_integer_or_real(p_result_type, nullptr);
				// TODO emit ABS
				break;
			case TC_MINUS:
				check_integer_or_real(p_result_type, nullptr);
				// TODO emit ABS
				// TODO emit NEG
				break;
				/*case TC_PLUS_PLUS:{
					check_integer_or_real(p_result_type, nullptr);
					this->emit_inc(p_function_id, p_result_type, { 1 });
					}break;
					case TC_MINUS_MINUS:{
					check_integer_or_real(p_result_type, nullptr);
					this->emit_inc(p_function_id, p_result_type, { -1 });
					}break;*/
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

				p_result_type = parse_rvalue(p_function_id, p_result_type, p_operand_type);
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

				this->emit(p_function_id, { ISHL });
				break;
			case TC_BIT_RIGHTSHIFT:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);

				this->emit(p_function_id, { ISHR });
				break;
			case TC_BIT_AND:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);

				this->emit(p_function_id, { IAND });
				break;
			case TC_BIT_XOR:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);

				this->emit(p_function_id, { IXOR });
				break;
			case TC_BIT_OR:
				check_bitwise_integer(p_result_type);
				check_bitwise_integer(p_operand_type);

				this->emit(p_function_id, { IOR });
				break;
			case TC_LOGIC_OR:
				check_boolean(p_result_type, p_operand_type);

				this->emit(p_function_id, { LOGIC_OR });
				break;
			default:
				break;
			}
		}

		return p_result_type;
	}

	cx_type::type_ptr parser::parse_rvalue(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &lhs, cx_type::type_ptr &rhs) {
		cx_type::type_ptr p_result_type = lhs;
		//cx_type::type_ptr p_tmp_type;// = nullptr;

		//if ((lhs->typeform == F_ARRAY) || (rhs->typeform == F_ARRAY)) {
		//	const int size = lhs->size + rhs->size;
		//	const int element_count = size / (lhs->typeform == F_ARRAY ?
		//		lhs->base_type()->size :
		//		rhs->base_type()->size);

		//	p_tmp_type = new cx_type(F_ARRAY, size, nullptr);
		//	//p_tmp_type->is_temp_value = true;
		//	p_tmp_type->array.element_count = element_count;
		//	p_tmp_type->array.max_index = element_count;
		//	p_tmp_type->type_code = rhs->base_type()->type_code;

		//	if (lhs->typeform == F_ARRAY) {
		//		set_type(p_tmp_type->array.p_element_type, lhs->array.p_element_type);
		//	}
		//	else {
		//		set_type(p_tmp_type->array.p_element_type, rhs->array.p_element_type);
		//	}

		//	p_result_type = p_tmp_type;

		//	if (rhs->is_temp_value) {
		//		remove_type(rhs);
		//	}

		//}

		return p_result_type;
	}

	/** parse_term           parse a term (binary operators * /
	*                      % and &&).
	*
	* @return ptr to the term's type object.
	*/
	cx_type::type_ptr parser::parse_term(symbol_table_node_ptr &p_function_id) {

		cx_type::type_ptr p_result_type;
		cx_type::type_ptr p_operand_type;
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
	cx_type::type_ptr parser::parse_factor(symbol_table_node_ptr &p_function_id) {

		cx_type::type_ptr p_result_type = nullptr;
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
			symbol_table_node_ptr &p_node = search_all(p_token->string);

			if (p_node == nullptr){
				cx_error(ERR_UNDEFINED_IDENTIFIER);
			}

			switch (p_node->defined.defined_how) {
			case DC_FUNCTION:
				get_token();
				p_result_type = parse_subroutine_call(p_function_id, p_node);

				if (token == TC_DOT) {
					p_result_type = parse_variable(p_function_id, p_node);
				}
				break;
			case DC_CONSTANT:
				get_token();
				p_result_type = p_node->p_type;

				this->emit_const(p_function_id, p_node);
				break;

			case DC_TYPE:
				get_token();
				p_result_type = p_node->p_type;
				break;
			case DC_VARIABLE:
			case DC_VALUE_PARM:
			case DC_REFERENCE:
			case DC_MEMBER:
			case DC_NAMESPACE:
				get_token();
				p_result_type = parse_variable(p_function_id, p_node, true);

				switch (op)
				{
				case TC_PLUS_PLUS:
					switch (p_result_type->typecode){
					case T_DOUBLE:
						this->emit(p_function_id, { DCONST }, { 1.0 });
						this->emit(p_function_id, { DADD });
						this->emit_store(p_function_id, p_node);
						break;
					default:
						this->emit(p_function_id, { IINC }, { 1 });
						this->emit_store(p_function_id, p_node);
						break;
					}
					break;
				case TC_MINUS_MINUS:
					switch (p_result_type->typecode){
					case T_DOUBLE:
						this->emit(p_function_id, { DCONST }, { -1.0 });
						this->emit(p_function_id, { DSUB });
						this->emit_store(p_function_id, p_node);
						break;
					default:
						this->emit(p_function_id, { IINC }, { -1 });
						this->emit_store(p_function_id, p_node);
						break;
					}
					break;
				default:
					break;
				}

				break;
			default:
				cx_error(ERR_UNDEFINED_IDENTIFIER);
				break;
			}
		}
		break;
		case TC_NUMBER:
		{
			symbol_table_node_ptr &p_node = search_all(p_token->string);
			opcode op;

			if (p_node == nullptr) {
				p_node = enter_local(p_token->string);
			}

			switch (p_token->type()){
			case T_INT:
				p_node->p_type = p_integer_type;
				p_node->defined.constant.value.i_ = p_token->value().i_;
				op = ICONST;
				break;
			case T_DOUBLE:
				p_node->p_type = p_double_type;
				p_node->defined.constant.value.d_ = p_token->value().d_;
				op = DCONST;
				break;
			default:
				cx_error(ERR_INCOMPATIBLE_ASSIGNMENT);
			}

			get_token();
			p_result_type = p_node->p_type;
			this->emit(p_function_id, op, p_node->defined.constant.value);

		}break;
		case TC_CHAR: // TODO TC_WCHAR
		{
			symbol_table_node_ptr &p_id = search_all(p_token->string);

			if (p_id == nullptr) {
				p_id = enter_local(p_token->string);
				p_id->p_type = p_char_type;
				p_id->defined.constant.value.c_ = p_token->string[1];
			}

			p_result_type = p_char_type;

			this->emit(p_function_id, ICONST, p_id->defined.constant.value.c_);
			get_token();
		}break;
		/*case TC_CHAR: 
		{
			symbol_table_node_ptr &p_id = search_all(p_token->string);

			if (p_id == nullptr) {
				p_id = enter_local(p_token->string);
				p_id->p_type = p_char_type;
				p_id->defined.constant.value.c_ = p_token->string[1];
			}

			p_result_type = p_char_type;

			this->emit(p_function_id, ICONST, p_id->defined.constant.value);
			get_token();
		}
		break;*/
		case TC_STRING:
		case TC_WSTRING: // TODO TC_WSTRING
		{
			// TODO fix string constants
			symbol_table_node_ptr &p_id = search_all(p_token->string);

			if (p_id == nullptr) {
				p_id = enter_local(p_token->string);
			}

			if (p_token->type() == T_CHAR){
				p_id->p_type = p_char_type;
				p_id->defined.constant.value.c_ = (wchar_t)p_token->string[1];

				this->emit(p_function_id, ICONST, p_id->defined.constant.value);
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

			//	get_token_append();
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
	cx_type::type_ptr parser::parse_variable(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, bool reference) {
		cx_type::type_ptr p_result_type = p_id->p_type;

		switch (p_id->defined.defined_how) {
		case DC_VARIABLE:
		case DC_VALUE_PARM:
		case DC_REFERENCE:
		case DC_POINTER:
			this->emit_load(p_function_id, p_id, reference);
			break;
		case DC_FUNCTION:
		case DC_UNDEFINED:
		case DC_NAMESPACE:
			break;

		default:
			p_result_type = p_dummy_type;
			cx_error(ERR_INVALID_IDENTIFIER_USAGE);
			break;
		}

		//  [ or . : Loop to parse any subscripts and fields.
		bool done_flag = false;
		cx_type::type_ptr p_prev_type = p_result_type;
		do {
			switch (token) {

			case TC_LEFT_SUBSCRIPT:
				p_result_type = parse_subscripts(p_function_id, p_result_type);
				break;

			case TC_DOT:
			case TC_COLON_COLON:
				p_result_type = parse_field(p_function_id, p_id, p_prev_type);
				p_prev_type = p_result_type;
				break;

			case TC_PLUS_PLUS:
				this->get_token();
				switch (p_result_type->typecode){
				case T_DOUBLE:
					this->emit(p_function_id, { DCONST }, { 1.0 });
					this->emit(p_function_id, { DADD });
					this->emit_store_no_load(p_function_id, p_id);
					break;
				default:
					this->emit(p_function_id, { IINC }, { 1 });
					this->emit_store(p_function_id, p_id);
					break;
				}
				break;
			case TC_MINUS_MINUS:
				this->get_token();
				switch (p_result_type->typecode){
				case T_DOUBLE:
					this->emit(p_function_id, { DCONST }, { -1.0 });
					this->emit(p_function_id, { DSUB });
					this->emit_store(p_function_id, p_id);
					break;
				default:
					this->emit(p_function_id, { IINC }, { -1 });
					this->emit_store(p_function_id, p_id);
					break;
				}
				break;


			default: done_flag = true;
			}
		} while (!done_flag);

		if (token_in(token, tokenlist_assign_ops)) {
			cx_type::type_ptr p_expr_type = nullptr;

			switch (token) {
			case TC_EQUAL:
			{
				get_token_append();
				p_expr_type = parse_expression(p_function_id);

				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_PLUS_EQUAL:
			{
				get_token();
				p_expr_type = parse_expression(p_function_id);

				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit_add(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_MINUS_EQUAL:
			{
				get_token();
				p_expr_type = parse_expression(p_function_id);

				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit_sub(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_STAR_EQUAL:
			{
				get_token();
				p_expr_type = parse_expression(p_function_id);

				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit_mul(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_DIVIDE_EQUAL:
			{
				get_token();
				p_expr_type = parse_expression(p_function_id);

				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit_div(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_MODULAS_EQUAL:
			{
				get_token();
				p_expr_type = parse_expression(p_function_id);

				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit_mod(p_function_id, p_result_type);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_BIT_LEFTSHIFT_EQUAL:
			{
				get_token();

				p_expr_type = parse_expression(p_function_id);

				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit(p_function_id, ISHL);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_BIT_RIGHTSHIFT_EQUAL:
			{
				get_token();

				p_expr_type = parse_expression(p_function_id);

				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit(p_function_id, ISHR);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_BIT_AND_EQUAL:
			{
				get_token();

				p_expr_type = parse_expression(p_function_id);

				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit(p_function_id, IAND);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_BIT_XOR_EQUAL:
			{
				get_token();

				p_expr_type = parse_expression(p_function_id);

				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit(p_function_id, IXOR);
				this->emit_store(p_function_id, p_id);
			}
			break;
			case TC_BIT_OR_EQUAL:
			{
				get_token();

				p_expr_type = parse_expression(p_function_id);

				check_bitwise_integer(p_result_type, p_expr_type);
				check_assignment_type_compatible(p_function_id, p_result_type, p_expr_type,
					ERR_INCOMPATIBLE_ASSIGNMENT);

				this->emit(p_function_id, IOR);
				this->emit_store(p_function_id, p_id);
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

		while (token_in(token, tokenlist_subscript_or_field_start)) {
			p_result_type = token == TC_LEFT_SUBSCRIPT ? parse_subscripts(p_function_id, p_result_type)
				: parse_field(p_function_id, p_id, p_result_type);
		}

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
	cx_type::type_ptr parser::parse_subscripts(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type) {

		//cx_type *p_result_type = (cx_type *) p_type;

		/*if (p_type->array.p_element_type->typeform == F_ARRAY){
		p_type = p_type->array.p_element_type;
		}*/

		int column = 0;

		do {
			get_token_append();

			if (p_type->typeform == F_ARRAY) {
				check_assignment_type_compatible(p_function_id, p_integer_type,
					parse_expression(p_function_id),
					ERR_INCOMPATIBLE_TYPES);
			}
			else {
				cx_error(ERR_TOO_MANY_SUBSCRIPTS);
				parse_expression(p_function_id);
			}

			conditional_get_token_append(TC_RIGHT_SUBSCRIPT, ERR_MISSING_RIGHT_SUBSCRIPT);
			if (token == TC_LEFT_SUBSCRIPT)++column;
		} while (token == TC_LEFT_SUBSCRIPT);

		cx_type::type_ptr elem = p_type;
		for (int i = 0; i < column; i++) {
			if (elem->typeform == F_ARRAY) {
				elem = elem->array.p_element_type;
			}
		}

		return elem->array.p_element_type;
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
	cx_type::type_ptr parser::parse_field(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node, cx_type::type_ptr &p_type) {
		get_token_append();

		if (token == TC_IDENTIFIER) {
			symbol_table_node_ptr &p_field_id = p_type->complex.p_class_scope->search(p_token->string);
			if (p_field_id == nullptr) cx_error(ERR_INVALID_FIELD);

			//icode.put(p_field_id);
			get_token_append();

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
	}

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
		}
		else if (token == TC_LEFT_BRACKET) {
			p_function_id->defined.routine.function_type = FUNC_DECLARED;
			parse_block(p_function_id);
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

			// TODO if token !== identifier, this must be a forward decaration

			symbol_table_node_ptr p_param = enter_new_local(p_token->string, DC_REFERENCE);
			p_param->p_type = p_node->p_type;
			p_function_id->defined.routine.p_parameter_ids.push_back(p_param);

			//  , or )
			get_token();

			/*if (is_array) {
			//set_type(p_parm_type->array.p_element_type, p_parm_type);
			set_type(p_parm_id->p_type, p_parm_type);
			cx_type *p_res = parse_unksize_array_type(p_function_id,
			p_parm_id);
			}
			else {
			set_type(p_parm_id->p_type, p_parm_type);
			}*/

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
	cx_type::type_ptr parser::parse_subroutine_call(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node_id) {
		//get_token_append();
		cx_type::type_ptr p_result_type = nullptr;
		p_result_type = parse_declared_subroutine_call(p_function_id, p_node_id);
		p_function_id->defined.routine.program_code.push_back({ CALL, p_node_id.get() });
		return p_result_type;
		/*return (p_function_id->defined.routine.function_type == FUNC_DECLARED) ||
			(p_function_id->defined.routine.function_type == FUNC_FORWARD)
			||
			!parm_check_flag
			? parse_declared_subroutine_call(p_function_id, parm_check_flag)
			: ;*/
	}

	/** parse_declared_subroutine_call parse a call to a declared
	*                              procedure or function.
	*
	* @param p_function_id    : ptr to subroutine id's symbol table node.
	* @param parm_check_flag : true to check parameter, false not to.
	* @return ptr to the subroutine's type object.
	*/
	cx_type::type_ptr parser::parse_declared_subroutine_call
		(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node_id) {

		if (token == TC_LEFT_PAREN) {
			parse_actual_parm_list(p_function_id, p_node_id);
			//get_token_append();
		}

		return p_function_id->p_type;
	}

	/** parse_actual_parm_list     parse an actual parameter list:
	*
	*                              ( <expr-list> )
	*
	* @param p_function_id    : ptr to routine id's symbol table node.
	* @param parm_check_flag : true to check parameter, false not to.
	*/
	void parser::parse_actual_parm_list(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node_id) {

		/* If there are no actual parameters, there better not be
		 * any formal parameters either. */

		if (token != TC_LEFT_PAREN) {
			//if (parm_check_flag && p_formal_id) cx_error(ERR_WRONG_NUMBER_OF_PARMS);
			return;
		}

		std::vector <std::shared_ptr<symbol_table_node>>::iterator p_formal_id;
		p_formal_id = p_node_id->defined.routine.p_parameter_ids.begin();

		cx::cx_type::type_ptr p_expr_type;

		/* Loop to parse actual parameter expressions
		* separated by commas. */
		do {
			//  ( or ,
			get_token();

			if (token == TC_RIGHT_PAREN && (p_node_id->defined.routine.p_parameter_ids.size()) == 0) {
				break;
			}

			// TODO parse_expression and check type compatability
			p_expr_type = parse_expression(p_function_id);

			check_assignment_type_compatible(p_function_id, p_formal_id->get()->p_type, p_expr_type,
				ERR_INCOMPATIBLE_TYPES);

			if (p_formal_id != p_node_id->defined.routine.p_parameter_ids.end()){
				++p_formal_id;
			}
		} while (token == TC_COMMA);

		//  )
		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		// There better not be any more formal parameters.
		//if (parm_check_flag && p_formal_id) cx_error(ERR_WRONG_NUMBER_OF_PARMS);
	}

	/** parse_statement          parse a statement.
	*
	* @param p_function_id : function in which this statement is executed.
	*/
	void parser::parse_statement(symbol_table_node_ptr &p_function_id) {
		//insert_line_marker();

		switch (this->token) {
		case TC_IDENTIFIER: parse_declarations_or_assignment(p_function_id);

			break;
			//	// not a type but a cv-qualifier
			//case TC_CONST:
			//	get_token_append();
			//	parse_constant_declaration(p_function_id);
			//	break;
			//	//case tcEnum:
			//	//get_token_append();
			//	//            parse_enum_header(p_function_id);
			//	//break;
			//case TC_DO: parse_DO(p_function_id);
			//	break;
			//case TC_WHILE: parse_WHILE(p_function_id);
			//	break;
			//case TC_IF: parse_IF(p_function_id);
			//	break;
			//case TC_FOR: parse_FOR(p_function_id);
			//	break;
			//case TC_SWITCH: parse_SWITCH(p_function_id);
			//	break;
			//	//case tc_CASE:
			//	//case tc_DEFAULT:parse_case_label(p_function_id);
			//	//  break;
			//case TC_BREAK: get_token_append();
			//	break;
			//case TC_LEFT_BRACKET: parse_compound(p_function_id);
			//	break;
		case TC_RETURN: parse_RETURN(p_function_id);
			break;
			//case TC_POUND:
			//	get_token();
			//	parse_execute_directive(p_function_id);
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

		default:
			parse_simple_expression(p_function_id);
			break;
		}

		//if (token != TC_END_OF_FILE) {
		//	resync(tokenlist_statement_follow, tokenlist_statement_start);
		//}
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
			while (token == TC_SEMICOLON)get_token_append();
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
	cx_type::type_ptr parser::parse_assignment(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_target_id) {

		cx_type::type_ptr p_target_type = parse_variable(p_function_id, p_target_id);

		return p_target_type;
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


		//int break_point = put_location_marker();
		//get_token_append(); //do

		//parse_statement_list(p_function_id, TC_WHILE);

		//conditional_get_token_append(TC_WHILE, ERR_MISSING_WHILE);
		//conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		//check_boolean(parse_expression(), nullptr);

		//conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		//fixup_location_marker(break_point);
	}

	/** parse_WHILE          parse while statement.
	*
	*      while(<expression>)
	*            <statement>;
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_WHILE(symbol_table_node_ptr &p_function_id) {

		////int break_point = put_location_marker();

		//get_token_append(); // while
		//conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		//check_boolean(parse_expression(), nullptr);

		//conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		//parse_statement(p_function_id);

		//fixup_location_marker(break_point);
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

		//// Append a placeholder location marker for where to go to if
		//// <expr> is false.  Remember the location of this placeholder
		//// so it can be fixed up below.
		////int at_false_location_marker = put_location_marker();

		//get_token_append();
		//conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		//check_boolean(parse_expression(), nullptr);

		//conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		//parse_statement(p_function_id);
		//while (token == TC_SEMICOLON) get_token_append();

		////fixup_location_marker(at_false_location_marker);
		//if (token == TC_ELSE) {
		//	// Append a placeholder location marker for the token that
		//	// follows the IF statement.  Remember the location of this
		//	// placeholder so it can be fixed up below.
		//	//int at_follow_location_marker = put_location_marker();

		//	get_token_append();
		//	parse_statement(p_function_id);
		//	while (token == TC_SEMICOLON) get_token_append();

		//	//fixup_location_marker(at_follow_location_marker);
		//}
	}

	/** parse_FOR            parse for statements.
	*
	*      for(<statement>; <expression>; <expression>)
	*              <statement>;
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_FOR(symbol_table_node_ptr &p_function_id) {

		///*int break_point = put_location_marker();
		//int statementMarker = put_location_marker();
		//int condition_marker = put_location_marker();
		//int increment_marker = put_location_marker();*/

		//get_token_append(); // for

		//conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		//if (token != TC_SEMICOLON) {
		//	// declaration would go here //
		//	parse_declarations_or_assignment(p_function_id);
		//	conditional_get_token_append(TC_SEMICOLON, ERR_MISSING_SEMICOLON);
		//}
		//else get_token_append();

		////fixup_location_marker(condition_marker);
		//if (token != TC_SEMICOLON) {

		//	// expr 2
		//	check_boolean(parse_expression(), nullptr);
		//	conditional_get_token_append(TC_SEMICOLON, ERR_MISSING_SEMICOLON);
		//}
		//else get_token_append();

		////	fixup_location_marker(increment_marker);
		//if (token != TC_RIGHT_PAREN) {
		//	// expr 3
		//	parse_expression();
		//}

		//conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);
		////	fixup_location_marker(statementMarker);
		//parse_statement(p_function_id);
		////fixup_location_marker(break_point);

	}

	/** parse_SWITCH         parse switch statements.
	*
	*      switch(<expression>){
	*              case <const-expression>:
	*              default:
	*      }
	*
	* NOTE:
	*      Broken/not implemented yet.
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	/*void parser::parse_SWITCH(symbol_table_node_ptr &p_function_id) {

	get_token_append();
	conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

	cx_type::type_ptr p_expr_type = parse_expression();

	conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

	if ((p_expr_type != p_integer_type)
	&& (p_expr_type != p_char_type)
	&& (p_expr_type->typeform != F_ENUM)) {
	cx_error(ERR_INCOMPATIBLE_TYPES);
	}

	parse_statement(p_function_id);

	}*/

	/// XXX fixme

	/*void parser::parse_case_branch(symbol_table_node_ptr &p_function_id, const cx_type::type_ptr &p_expr_type) {
	// c switch easier to parse that Pascal???
	}*/

	/// XXX fixme

	/*void parser::parse_case_label(cx_symtab_node* p_function_id, const cx_type *p_expr_type) {
	get_token_append();

	bool sign_flag(false);

	if (token_in(token, tokenlist_unary_ops)) {
	sign_flag = true;
	get_token_append();
	}

	switch (token) {
	case tc_identifier:
	if (!search_all(p_token->string__())) {
	cx_error(err_undefined_identifier);
	}

	get_token_append();
	break;
	case tc_number:
	if (p_token->type() != ty_integer) cx_error(err_invalid_constant);
	get_token_append();
	break;
	case tc_string:
	if (sign_flag || (strlen(p_token->string__()) != 3)) {
	cx_error(err_invalid_constant);
	}
	break;
	default:
	break;
	}

	conditional_get_token_append(tc_colon, err_missing_colon);

	parse_statement_list(p_function_id, tc_BREAK);
	}*/

	/** parse_compound       parse compounded statements.
	*
	*      {
	*         <statements>;
	*      }
	*
	* @param p_function_id : ptr to this statements function Id.
	*/
	void parser::parse_compound(symbol_table_node_ptr &p_function_id) {
		get_token_append();

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
		}

		// expr 1
		check_assignment_type_compatible(p_function_id, p_function_id->p_type, parse_expression(p_function_id),
			ERR_INCOMPATIBLE_TYPES);

		this->emit_store(p_function_id, p_function_id);
		this->emit(p_function_id, RETURN);
	}

	/** parse_type_definitions    parse a list of type definitions
	*                          separated by commas:
	*
	*                              <type> <id>, ... ;
	*
	* @param p_function_id : ptr to symbol table node of program,
	*                     procedure, or function identifier.
	*/
	void parser::parse_type_definitions(symbol_table_node_ptr &p_function_id) {
		//symbol_table_node_ptr p_last_id;// = nullptr;

		//while (token == TC_IDENTIFIER) {
		//	symbol_table_node_ptr p_type_id = enter_new_local(p_token->string__());

		//	/*if (p_function_id->defined.routine.locals.types == nullptr) {
		//	p_function_id->defined.routine.locals.types.symbols.emplace(p_type_id);
		//	}
		//	else {
		//	//p_last_id->next__ = p_type_id;
		//	}*/

		//	p_last_id = p_type_id;

		//	get_token_append();
		//	conditional_get_token(TC_EQUAL, ERR_MISSING_EQUAL);

		//	//        set_type(p_type_id->p_type, parse_type_spec());
		//	p_type_id->defined.defined_how = DC_TYPE;

		//	if (!p_type_id->p_type->p_type_id) {
		//		p_type_id->p_type->p_type_id = p_type_id;
		//	}

		//	resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);
		//	conditional_get_token(TC_SEMICOLON, ERR_MISSING_SEMICOLON);

		//	while (token == TC_SEMICOLON)get_token_append();

		//	resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);

		//}
	}

	/** parse_type_spec       parse a type specification.
	*
	* @param p_node : ptr to object node.
	* @return ptr to type object.
	*/
	cx_type::type_ptr parser::parse_type_spec(symbol_table_node_ptr &p_node) {
		//switch (token) {
		//case TC_IDENTIFIER:
		//{
		//	symbol_table_node_ptr p_id = find(p_token->string__());

		//	switch (p_id->defined.defined_how) {
		//	case DC_TYPE: return parse_identifier_type(p_id);
		//	case DC_CONSTANT: return parse_subrange_type(p_id);
		//	default:
		//		cx_error(ERR_NOT_A_TYPE_IDENTIFIER);
		//		get_token_append();
		//		return (p_dummy_type);
		//	}
		//}

		//case TC_LEFT_BRACKET: return parse_enumeration_type();

		//	/* found empty subscript.
		//	* array must have initializer */
		//case TC_RIGHT_SUBSCRIPT: return parse_assignment(p_node);
		//	//        case tcRecord: return parse_complex_type();
		//case TC_PLUS:
		//case TC_MINUS:
		//case TC_NUMBER:
		//case TC_CHAR:
		//case TC_STRING:// return parse_subrange_type(nullptr);

		//default:
		//	cx_error(ERR_INVALID_TYPE);
		//	return (p_dummy_type);

		//}

		return p_dummy_type;
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
	cx_type::type_ptr parser::parse_identifier_type(symbol_table_node_ptr &p_id2) {
		get_token_append();
		return p_id2->p_type;
	}

	/** parse_enumeration_type    parse a enumeration type
	*                          specification:
	*      enum <id> { <enum-list> };
	*
	* @return
	*/
	/*cx_type *parser::parse_enumeration_type(void) {
	cx_type *p_type = new cx_type(fc_enum, sizeof(int), nullptr);
	symbol_table_node_ptr &p_last_id = nullptr;

	int const_value = -1;

	get_token_append();
	resync(tokenlist_enum_const_start);

	while (token == tc_identifier) {
	symbol_table_node_ptr &p_const_id = enter_new_local(p_token->string__());
	++const_value;

	if (p_const_id->defined.defined_how == dc_undefined) {
	p_const_id->defined.defined_how = dc_constant;

	p_const_id->defined.constant.value.int__ = const_value;
	set_type(p_const_id->p_type, p_type);

	if (!p_last_id) {
	p_type->enumeration.p_const_ids = p_last_id = p_const_id;
	}
	else {
	p_last_id->next__ = p_const_id;
	p_last_id = p_const_id;
	}
	}

	get_token_append();
	resync(tokenlist_enum_const_follow);

	if (token == tc_comma) {

	do {
	get_token_append();
	resync(tokenlist_enum_const_start, tokenlist_enum_const_follow);
	if (token == tc_comma) cx_error(err_missing_identifier);
	} while (token == tc_comma);
	if (token != tc_identifier) cx_error(err_missing_identifier);
	}
	else if (token == tc_identifier) cx_error(err_missing_comma);
	}

	conditional_get_token(tc_right_bracket, err_missing_right_bracket);

	p_type->enumeration.max = const_value;
	return p_type;
	}*/

	/** parse_array_type      parse an array type specification.
	*
	*      <type-id> <id> [ <const-index-size> ];
	*
	* @param p_function_id : ptr to fuction which owns this array.
	* @param p_array_node : ptr to array symtab node.
	* @return ptr to array type object.
	*/
	cx_type::type_ptr parser::parse_array_type(symbol_table_node_ptr &p_function_id,
		symbol_table_node_ptr &p_array_node) {

		cx_type::type_ptr p_array_type;
		//cx_type::type_ptr p_array_type = std::make_shared<cx_type>(*(new cx_type()));
		//p_array_type->typeform = F_ARRAY;
		//p_array_type->size = 0;
		//cx_type::type_ptr p_element_type = p_array_type;

		//// Final element type.
		////set_type(p_element_type->array.p_element_type, p_array_node->p_type);

		//conditional_get_token(TC_LEFT_SUBSCRIPT, ERR_MISSING_LEFT_SUBSCRIPT);

		//int min_index = 0;
		//int max_index = 0;

		//if (token == TC_NUMBER) {
		//	max_index = p_token->value().i_;
		//	get_token();

		//}
		//else {
		//	cx_type::type_ptr p_index_type = parse_expression();

		//	check_assignment_type_compatible(p_integer_type, p_index_type,
		//		ERR_INVALID_INDEX_TYPE);

		//}

		//p_array_type->array.element_count = max_index;
		//p_array_type->array.max_index = max_index;
		//p_array_type->size = min_index * p_array_type->array.p_element_type->size;

		//conditional_get_token(TC_RIGHT_SUBSCRIPT, ERR_MISSING_RIGHT_SUBSCRIPT);

		//if (token_in(token, tokenlist_assign_ops))parse_assignment(p_array_node);
		//p_array_node->p_type = p_array_type;
		////set_type(p_array_node->p_type, p_array_type);

		//if (p_array_node->defined.defined_how == DC_UNDEFINED) {
		//	p_array_node->defined.defined_how = DC_VARIABLE;
		//}

		//// If the type object doesn't have a name yet,
		//// point it to the type id.
		//if (!p_array_node->p_type->p_type_id) {
		//	p_array_node->p_type->p_type_id = p_array_node;
		//}

		return p_array_type;
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
	cx_type::type_ptr parser::parse_unksize_array_type(symbol_table_node_ptr &p_function_id,
		symbol_table_node_ptr &p_array_node) {

		cx_type::type_ptr p_array_type = std::make_shared<cx_type>();
		p_array_type->typeform = F_ARRAY;

		cx_type::type_ptr p_expr_type;

		bool is_function = false;
		const bool is_expression = token_in(this->token, tokenlist_assign_ops);

		if ((this->token != TC_LEFT_PAREN) && (token != TC_RIGHT_PAREN) &&
			(!is_expression)) get_token_append();
		else if ((this->token != TC_RIGHT_PAREN) && (!is_expression)) is_function = true;

		p_array_type->array.element_count = 0;
		p_array_type->array.max_index = 0;

		// element type.
		//set_type(p_array_type->array.p_element_type, p_array_node->p_type);
		p_array_type->typecode = p_array_node->p_type->typecode;

		if (is_function) {
			//set_type(p_array_node->p_type, p_array_type);
			p_array_node->p_type->p_type_id = p_array_node;
			parse_function_header(p_array_node);

			return p_array_type;
		}

		if (is_expression) {

			p_expr_type = parse_assignment(p_function_id, p_array_node);
			//remove_type(p_array_type);

			//set_type(p_array_node->p_type, p_expr_type);
			p_array_node->p_type->p_type_id = p_array_node;
			p_array_type = p_array_node->p_type;
		}
		else {
			//set_type(p_array_node->p_type, p_array_type);
		}

		if (p_array_node->defined.defined_how == DC_UNDEFINED) {
			p_array_node->defined.defined_how = DC_VARIABLE;
		}

		/* If the type object doesn't have a name yet,
		* point it to the type id. */
		if (!p_array_node->p_type->p_type_id) {
			p_array_node->p_type->p_type_id = p_array_node;
		}

		return p_array_type;
	}

	/** array_size           Calculate the total byte size of an
	*                      array type by recursively calculating
	*                      the size of each dimension.
	*
	* @param p_array_type : ptr to array type object.
	* @return byte size.
	*/
	size_t parser::array_size(cx_type::type_ptr &p_array_type) {
		if (p_array_type->array.p_element_type->size == 0) {
			p_array_type->array.p_element_type->size = array_size(p_array_type->array.p_element_type);
		}

		return (p_array_type->array.element_count * p_array_type->array.p_element_type->size);
	}

	///** parse_string_type      parse an string type specification.
	// *
	// *      <type-id> <id> [ <const-index-size> ];
	// *
	// * @param p_function_id : ptr to fuction which owns this array.
	// * @param p_array_node : ptr to string symtab node.
	// * @return ptr to array type object.
	// */
	//cx_type *parser::parse_string_type(cx_symtab_node* p_function_id,
	//        cx_symtab_node* p_string_node) {
	//
	//    cx_type *p_array_type = new cx_type(F_ARRAY, 0, nullptr);
	//    cx_type *p_element_type = p_array_type;
	//
	//    // Final element type.
	//    set_type(p_element_type->array.p_element_type, p_char_type);
	//
	//    int min_index = 0;
	//    int max_index = 0;
	//
	//    //set_type(p_element_type->array.p_index_type, p_integer_type);
	//    p_array_type->array.element_count = max_index;
	//    p_array_type->array.min_index = min_index;
	//    p_array_type->array.max_index = max_index - 1;
	//
	//    set_type(p_string_node->p_type, p_array_type);
	//
	//    get_token_append();
	//
	//    if (token_in(token, tokenlist_assign_ops))parse_assignment(p_string_node);
	//
	//    p_string_node->defined.defined_how = dc_variable;
	//
	//    //add to routines variable list
	//    /*if (p_function_id != nullptr) {
	//        symbol_table_node_ptr array = p_function_id->defined.routine.locals.p_type_ids;
	//        if (!array) {
	//            p_function_id->defined.routine.locals.p_type_ids = p_string_node;
	//        } else {
	//            while (array->next__)array = array->next__;
	//
	//            array->next__ = p_string_node;
	//        }
	//    }*/
	//
	//    // If the type object doesn't have a name yet,
	//    // point it to the type id.
	//    if (!p_string_node->p_type->p_type_id) {
	//        p_string_node->p_type->p_type_id = p_string_node;
	//    }
	//
	//    return p_array_type;
	//
	//}

	/** parse_complex_type
	*
	* NOTE:
	*      Not implemented.
	*
	* @param p_function_id
	* @param p_node
	* @return
	*/
	/*	cx_type * parser::parse_complex_type(symbol_table_node_ptr p_function_id, symbol_table_node_ptr  p_node) {

	get_token();

	// <id>
	cx_type *p_type = new cx_type(F_REFERENCE, 0, nullptr);

	//symbol_table_node_ptr next_type = p_node;

	//while (next_type->next__)
	//next_type = next_type->next__;

	symbol_table_node_ptr p_id = enter_new_local(p_token->string__());
	icode.put(p_id);

	//next_type->next__ = p_id;

	// Link the routine's local type id nodes together.
	if (!p_function_id->defined.routine.locals.p_type_ids) {
	p_function_id->defined.routine.locals.p_type_ids = p_id;
	}
	else {
	symbol_table_node_ptr p_var_id = p_function_id->defined.routine.locals.p_type_ids;

	while (p_var_id->next__)
	p_var_id = p_var_id->next__;

	p_var_id->next__ = p_id;
	}

	set_type(p_id->p_type, p_type);
	p_id->defined.defined_how = DC_TYPE;

	parse_member_decls(p_function_id, p_id->p_type, 0);

	return p_id->p_type;
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
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant.value.i_ });
			break;
		case T_CHAR:
			op = ICONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant.value.c_ });
			break;
		case T_DOUBLE:
			op = DCONST;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant.value.d_ });
			break;
		case T_REFERENCE:
			op = ACONST_NULL;
			p_function_id->defined.routine.program_code.push_back({ op, p_id.get()->defined.constant.value.a_ });
			break;
		default:
			break;
		}
	}

	void parser::emit_not_eq(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_lt_eq(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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


	void parser::emit_eq_eq(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
		case T_INT:
			op = IEQ_EQ;
			break;
		case T_DOUBLE:
			op = DEQ_EQ;
			break;
		case T_REFERENCE: // TODO fix
			//op = ANOT_EQ;
			break;
		default:
			break;
		}

		p_function_id->defined.routine.program_code.push_back(op);
	}

	void parser::emit_gt(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_gt_eq(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_lt(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_add(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_div(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_sub(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_mod(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_mul(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
		cx::opcode op;

		switch (p_type->typecode)
		{
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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

	void parser::emit_lnot(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type){
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
		opcode op;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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
		opcode op;

		switch (p_id->p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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
		this->emit_load(p_function_id, p_id, false);
	}

	void parser::emit_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, bool reference = false){
		opcode op;

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

	void parser::emit_inc(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type, value v_){

		switch (p_type->typecode)
		{
		case T_BOOLEAN:
		case T_BYTE:
		case T_CHAR:
//		case T_WCHAR:
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
}