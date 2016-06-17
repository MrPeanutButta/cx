#include "parser.h"

namespace cx {
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

			switch (op) {
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
			switch (unary_op) {
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

		switch (this->token) {
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

			if (p_node == nullptr) {
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
					switch (p_result_type->typecode) {
					case T_DOUBLE:
						this->emit(p_function_id, opcode::DINC, p_node.get(), 1.0L);
						break;
					default:
						this->emit(p_function_id, opcode::IINC, p_node.get(), 1);
						break;
					}
					break;
				case TC_MINUS_MINUS:
					switch (p_result_type->typecode) {
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
		case TC_NUMBER: {
			symbol_table_node_ptr p_node = search_all(p_token->string);

			if (p_node == nullptr) {
				p_node = enter_local(p_token->string, DC_CONSTANT);
			}

			switch (p_token->type()) {
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

			if (p_token->type() == T_CHAR) {
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
				switch (p_result_type->typecode) {
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
				switch (p_result_type->typecode) {
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
}