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

		if (token == TC_SEMICOLON) get_token();
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

		if (p_function_id->p_type->typecode == T_VOID) {
			// if token != ; (void function returning a value)
			this->emit(p_function_id, RETURN);
			return;
		}

		this->p_target_type = p_function_id->p_type;
		// expr 1
		check_assignment_type_compatible(p_function_id, p_function_id->p_type, parse_expression(p_function_id),
			ERR_INCOMPATIBLE_TYPES);

		this->emit_store(p_function_id, p_function_id);
		this->emit(p_function_id, RETURN);
	}
}