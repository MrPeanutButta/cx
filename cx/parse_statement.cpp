#include <cstdio>
#include "parser.h"
#include "common.h"

namespace cx{
	/** parse_statement          parse a statement.
	 *
	 * @param p_function_id : function in which this statement is executed.
	 */
	void parser::parse_statement(symbol_table_node_ptr &p_function_id) {
		insert_line_marker();

		switch (token) {
		case TC_IDENTIFIER: parse_declarations_or_assignment(p_function_id);

			break;
			// not a type but a cv-qualifier
		case TC_CONST:
			get_token_append();
			parse_constant_declaration(p_function_id);
			break;
			//case tcEnum:
			//get_token_append();
			//            parse_enum_header(p_function_id);
			//break;
		case TC_DO: parse_DO(p_function_id);
			break;
		case TC_WHILE: parse_WHILE(p_function_id);
			break;
		case TC_IF: parse_IF(p_function_id);
			break;
		case TC_FOR: parse_FOR(p_function_id);
			break;
		case TC_SWITCH: parse_SWITCH(p_function_id);
			break;
			//case tc_CASE:
			//case tc_DEFAULT:parse_case_label(p_function_id);
			//  break;
		case TC_BREAK: get_token_append();
			break;
		case TC_LEFT_BRACKET: parse_compound(p_function_id);
			break;
		case TC_RETURN: parse_RETURN(p_function_id);
			break;
		case TC_POUND:
			get_token();
			parse_execute_directive(p_function_id);
			break;
		case TC_NAMESPACE:{
			get_token();
			symbol_table_node_ptr p_namespace_id = search_local(p_token->string__()); //p_function_id->defn.routine.p_symtab->enter(p_token->string__(), dc_namespace);

			if (p_namespace_id == nullptr){
				p_namespace_id = enter_local(p_token->string__(), DC_NAMESPACE);
				p_namespace_id->p_type = std::make_shared<cx_type>(*(new cx_type()));
				p_namespace_id->p_type->complex.p_class_scope = std::make_shared<symbol_table>(*(new symbol_table()));
			}

			//icode.put(p_namespace_id);
			symbol_table_ptr p_old_symtab = symtab_stack.get_current_symtab();

			symtab_stack.set_scope(++current_nesting_level);
			symtab_stack.set_current_symtab(p_namespace_id->p_type->complex.p_class_scope);

			get_token();	// namespace ID
			conditional_get_token_append(TC_LEFT_BRACKET, ERR_MISSING_LEFT_BRACKET);	// open bracket

			parse_statement_list(p_namespace_id, TC_RIGHT_BRACKET);

			conditional_get_token_append(TC_RIGHT_BRACKET, ERR_MISSING_RIGHT_BRACKET);
			symtab_stack.set_scope(--current_nesting_level);
			symtab_stack.set_current_symtab(p_old_symtab);

		}
			break;
		default:
			break;
		}

		if (token != TC_END_OF_FILE) {
			resync(tokenlist_statement_follow, tokenlist_statement_start);
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
	cx_type::type_ptr parser::parse_assignment(const symbol_table_node_ptr &p_target_id) {

		cx_type::type_ptr p_target_type = parse_variable(p_target_id);

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
		get_token_append(); //do

		parse_statement_list(p_function_id, TC_WHILE);

		conditional_get_token_append(TC_WHILE, ERR_MISSING_WHILE);
		conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		check_boolean(parse_expression(), nullptr);

		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

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

		//int break_point = put_location_marker();

		get_token_append(); // while
		conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		check_boolean(parse_expression(), nullptr);

		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		parse_statement(p_function_id);

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

		// Append a placeholder location marker for where to go to if
		// <expr> is false.  Remember the location of this placeholder
		// so it can be fixed up below.
		//int at_false_location_marker = put_location_marker();

		get_token_append();
		conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		check_boolean(parse_expression(), nullptr);

		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);

		parse_statement(p_function_id);
		while (token == TC_SEMICOLON) get_token_append();

		//fixup_location_marker(at_false_location_marker);
		if (token == TC_ELSE) {
			// Append a placeholder location marker for the token that
			// follows the IF statement.  Remember the location of this
			// placeholder so it can be fixed up below.
			//int at_follow_location_marker = put_location_marker();

			get_token_append();
			parse_statement(p_function_id);
			while (token == TC_SEMICOLON) get_token_append();

			//fixup_location_marker(at_follow_location_marker);
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

		/*int break_point = put_location_marker();
		int statementMarker = put_location_marker();
		int condition_marker = put_location_marker();
		int increment_marker = put_location_marker();*/

		get_token_append(); // for

		conditional_get_token_append(TC_LEFT_PAREN, ERR_MISSING_LEFT_PAREN);

		if (token != TC_SEMICOLON) {
			// declaration would go here //
			parse_declarations_or_assignment(p_function_id);
			conditional_get_token_append(TC_SEMICOLON, ERR_MISSING_SEMICOLON);
		}
		else get_token_append();

		//fixup_location_marker(condition_marker);
		if (token != TC_SEMICOLON) {

			// expr 2
			check_boolean(parse_expression(), nullptr);
			conditional_get_token_append(TC_SEMICOLON, ERR_MISSING_SEMICOLON);
		}
		else get_token_append();

	//	fixup_location_marker(increment_marker);
		if (token != TC_RIGHT_PAREN) {
			// expr 3
			parse_expression();
		}

		conditional_get_token_append(TC_RIGHT_PAREN, ERR_MISSING_RIGHT_PAREN);
	//	fixup_location_marker(statementMarker);
		parse_statement(p_function_id);
		//fixup_location_marker(break_point);

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

	/*void cx_parser::parse_case_label(cx_symtab_node* p_function_id, const cx_type *p_expr_type) {
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

		//    if ((p_function_id->defn.routine.which == func_std_iterator)) return;


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
		get_token_append();

		// expr 1
		check_assignment_type_compatible(p_function_id->p_type, parse_expression(),
			ERR_INCOMPATIBLE_TYPES);
	}
}