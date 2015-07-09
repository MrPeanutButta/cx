#include "common.h"
#include "parser.h"

namespace cx{
	/** parse_formal_parm_list     parse a formal parameter list:
	 *
	 *                              ( <type-id> <id-list> ); or {
	 *
	 * @param count
	 * @param total_size
	 * @return
	 */
	void parser::parse_formal_parm_list(symbol_table_node_ptr &p_function_id, int &count, int &total_size) {

		//s//ymbol_table_node_ptr p_parm_id;// = nullptr; // ptrs to parm symtab nodes
		//cx_symtab_node *p_first_id, *p_last_id = nullptr;
		//cx_symtab_node *p_prev_sublist_last_id = nullptr;
		//cx_symtab_node *p_parm_list = nullptr; // ptr to list of parm nodes
		//define_code parm_defined_as; // how a parm is defined

		symbol_table_node_ptr p_node;// = nullptr;

		// Loop to parse the comma-separated sublist of parameter ids.
		///cx_type::type_ptr p_parm_type; // ptr to parm's type object
		while (token == TC_IDENTIFIER) {
			bool is_array = false;

			// find param type
			p_node = find(p_token->string__());

			if (p_node->defined.defined_how != DC_TYPE) {
				cx_error(ERR_INVALID_TYPE);
			}

			///p_parm_type = p_node->p_type;

			// get param name
			get_token();

			//p_first_id = nullptr;


			// Reference or array parameter?
			if (token == TC_STAR) {
				get_token();
				is_array = true;
			}

			//parm_defined_as = DC_REFERENCE;
			symbol_table_node_ptr p_param = enter_new_local(p_token->string__(), DC_REFERENCE);
			p_param->p_type->typeform = F_ARRAY;

			p_function_id->defined.routine.p_parameters->symbols.emplace(p_param);


			//  , or )
			get_token_append();

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
					get_token_append();
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
		else while (token == TC_SEMICOLON) get_token_append();

		//return p_parm_list;
	}

	/** parse_subroutine_call     parse a call to a declared or a
	 *                          standard procedure or function.
	 *
	 * @param p_function_id    : ptr to routine id's symbol table node.
	 * @param parm_check_flag : true to check parameter, false not to
	 * @return ptr to the subroutine's type object
	 */
	cx_type::type_ptr parser::parse_subroutine_call(symbol_table_node_ptr &p_function_id,
		bool parm_check_flag) {
		//get_token_append();

		return (p_function_id->defined.routine.function_type == FUNC_DECLARED) ||
			(p_function_id->defined.routine.function_type == FUNC_FORWARD)
			||
			!parm_check_flag
			? parse_declared_subroutine_call(p_function_id, parm_check_flag)
			: parse_declared_subroutine_call(p_function_id, parm_check_flag);
	}

	/** parse_declared_subroutine_call parse a call to a declared
	 *                              procedure or function.
	 *
	 * @param p_function_id    : ptr to subroutine id's symbol table node.
	 * @param parm_check_flag : true to check parameter, false not to.
	 * @return ptr to the subroutine's type object.
	 */
	cx_type::type_ptr parser::parse_declared_subroutine_call
		(symbol_table_node_ptr &p_function_id,
		int parm_check_flag) {

		if (token == TC_LEFT_PAREN) {
			parse_actual_parm_list(p_function_id, parm_check_flag);
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
	void parser::parse_actual_parm_list(symbol_table_node_ptr &p_function_id,
		int parm_check_flag) {
//		cx_symtab_node *p_formal_id = p_function_id ? p_function_id->defined.routine.p_parameters : nullptr;

		/* If there are no actual parameters, there better not be
		 * any formal parameters either. */
		if (token != TC_LEFT_PAREN) {
			if (parm_check_flag && p_formal_id) cx_error(ERR_WRONG_NUMBER_OF_PARMS);
			return;
		}

		/* Loop to parse actual parameter expressions
		 * separated by commas. */
		do {
			//  ( or ,
			get_token_append();

			if (token == TC_RIGHT_PAREN && (p_function_id->defined.routine.p_parameters->symbols.size() == 0)) {
				break;
			}

			parse_actual_parm(p_formal_id, parm_check_flag);
			if (p_formal_id) p_formal_id = p_formal_id->next__;
		} while (token == TC_COMMA);

		//  )
		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		// There better not be any more formal parameters.
		if (parm_check_flag && p_formal_id) cx_error(ERR_WRONG_NUMBER_OF_PARMS);
	}

	/** parse_actual_parm     parse an actual parameter.  Make sure it
	 *                      matches the corresponding formal parm.
	 *
	 * @param p_formal_id     : ptr to the corresponding formal parm
	 *                        id's symbol table node
	 * @param parm_check_flag : true to check parameter, false not to.
	 */
	void parser::parse_actual_parm(symbol_table_node_ptr &p_formal_id,
		int parm_check_flag) {
		/* If we're not checking the actual parameters against
		 * the corresponding formal parameters (as during error
		 * recovery), just parse the actual parameter. */
		if (!parm_check_flag) {
			parse_expression();
			return;
		}

		/* If we've already run out of formal parameter,
		 * we have an error.  Go into error recovery mode and
		 * parse the actual parameter anyway. */
		if (p_formal_id == nullptr) {
			parse_expression();
			cx_error(ERR_WRONG_NUMBER_OF_PARMS);
			return;
		}

		/* Formal value parameter: The actual parameter can be an
		 *                         arbitrary expression that is
		 *                         assignment type compatible with
		 *                         the formal parameter. */
		if (p_formal_id->defined.defined_how == DC_VALUE_PARM) {
			check_assignment_type_compatible(p_formal_id->p_type,
				parse_expression(),
				ERR_INCOMPATIBLE_TYPES);
		}/* Formal VAR parameter: The actual parameter must be a
			 *                       variable of the same type as the
			 *                       formal parameter. */
		else if (token == TC_IDENTIFIER) {
			symbol_table_node_ptr p_actual_id = find(p_token->string__());

			// skip type declaration
			if (p_actual_id->defined.defined_how == DC_TYPE) {
				get_token();

				if (token == TC_BIT_AND)get_token();

				p_actual_id = find(p_token->string__());
			}

			//icode.put(p_actual_id);

			get_token_append();
			if (p_formal_id->p_type->base_type()
				!= parse_variable(p_actual_id)->base_type()) {
				cx_error(ERR_INCOMPATIBLE_TYPES);
			}
			resync(tokenlist_expression_follow, tokenlist_statement_follow, tokenlist_statement_start);
		}// cx_error: parse the actual parameter anyway for error recovery.
		else {
			parse_expression();
			cx_error(ERR_INVALID_REFERENCE);
		}
	}
}
