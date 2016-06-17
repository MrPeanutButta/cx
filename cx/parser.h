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

#ifndef PARSER_H
#define PARSER_H
#include <string>
#include <cwchar>
#include "buffer.h"
#include "error.h"
#include "token.h"
#include "scanner.h"
#include "types.h"
#include "symtab.h"

namespace cx{

	extern symbol_table_ptr p_global_symtab;

	class parser {
	public:
		const std::wstring code_filename(void) const {
			return this->file_name;
		}

	private:
		text_scanner *const p_scanner; // ptr to the scanner
		token *p_token; // ptr to the current token
		token_code token; // code of current token
		symbol_table_stack symtab_stack;
		type_ptr p_target_type;
		bool is_module;
		std::wstring file_name;

		symbol_table_node_ptr parse_function_header(symbol_table_node_ptr &p_function_id);

		void parse_block(symbol_table_node_ptr &p_function_id);
		void parse_formal_parm_list(symbol_table_node_ptr &p_function_id);

		symbol_table_node_ptr parse_subroutine_call(symbol_table_node_ptr &p_function_id, std::pair<local::iterator, local::iterator> p_node_ids);
		symbol_table_node_ptr parse_declared_subroutine_call(symbol_table_node_ptr &p_function_id, std::pair<local::iterator, local::iterator> p_node_ids);

		symbol_table_node_ptr parse_actual_parm_list(symbol_table_node_ptr &p_function_id, std::pair<local::iterator, local::iterator> p_node_ids);
		void parse_declarations_or_assignment(symbol_table_node_ptr &p_function_id);
		type_ptr parse_array_type(symbol_table_node_ptr &p_function_id,
			symbol_table_node_ptr &p_array_node);

		size_t array_size(type_ptr &p_array_type);
		void parse_class_declaration(symbol_table_node_ptr &p_function_id);
		void parse_init(symbol_table_node_ptr &p_function_id);
		void parse_dispose(symbol_table_node_ptr &p_function_id);

		// expressions
		type_ptr parse_expression(symbol_table_node_ptr &p_function_id);
		type_ptr parse_simple_expression(symbol_table_node_ptr &p_function_id);
		type_ptr parse_term(symbol_table_node_ptr &p_function_id);
		type_ptr parse_factor(symbol_table_node_ptr &p_function_id);
		type_ptr parse_variable(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, bool reference = false);
		type_ptr parse_subscripts(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		//cx_type::type_ptr parse_field(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node, cx_type::type_ptr &p_type);

		// statements
		void parse_statement(symbol_table_node_ptr &p_function_id);
		type_ptr parse_assignment(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_target_id);
		void parse_statement_list(symbol_table_node_ptr &p_function_id, token_code terminator);
		void parse_ENUM(symbol_table_node_ptr &p_function_id);
		void parse_DO(symbol_table_node_ptr &p_function_id);
		void parse_WHILE(symbol_table_node_ptr &p_function_id);
		void parse_IF(symbol_table_node_ptr &p_function_id);
		void parse_FOR(symbol_table_node_ptr &p_function_id);
		void parse_compound(symbol_table_node_ptr &p_function_id);
		void parse_RETURN(symbol_table_node_ptr &p_function_id);
		void parse_ASM(symbol_table_node_ptr &p_function_id);

		void get_token(void) {
			p_token = p_scanner->get();
			token = p_token->code();
		}

		int put_location_marker(symbol_table_node_ptr &p_function_id) {
			int location = std::distance(p_function_id->defined.routine.program_code.begin(),
				p_function_id->defined.routine.program_code.end());
			return location - 1;
		}

		void fixup_location_marker(symbol_table_node_ptr &p_function_id, int location) {
			int jump_location = std::distance(p_function_id->defined.routine.program_code.begin(),
				p_function_id->defined.routine.program_code.end());

			p_function_id->defined.routine.program_code.at(location).arg0.i_ = jump_location;
		}

		void set_break_jump(symbol_table_node_ptr &p_function_id, int start_marker) {
			int break_point = current_location(p_function_id);

			// Search for any emitted BREAK_MARKER
			for (int i = start_marker; i < break_point; ++i) {
				if (p_function_id->defined.routine.program_code[i].op == opcode::BREAK_MARKER) {
					p_function_id->defined.routine.program_code[i].op = opcode::GOTO;
					p_function_id->defined.routine.program_code[i].arg0.i_ = break_point;
					break;
				}
			}
		}

		int current_location(symbol_table_node_ptr &p_function_id) {
			return std::distance(p_function_id->defined.routine.program_code.begin(),
				p_function_id->defined.routine.program_code.end());
		}

		symbol_table_node_ptr search_local(const std::wstring name) {
			return symtab_stack.search_local(name);
		}

		symbol_table_node_ptr search_all(const std::wstring name) {
			return symtab_stack.search_all(name);
		}

		symbol_table_node_ptr find(const std::wstring name) {
			symbol_table_node_ptr p_node = search_all(name);

			if (p_node == nullptr){
				cx_error(ERR_UNDEFINED_IDENTIFIER);
			}

			return p_node;
		}

		std::pair<local::iterator, local::iterator> find_functions(const std::wstring name) {
			return symtab_stack.find_functions(name);
		}

		void copy_quoted_string(wchar_t *p_string, const wchar_t *p_quoted_string) const {
			size_t length = wcslen(p_quoted_string) - 2;
			wcscpy(p_string, &p_quoted_string[1]);
			p_string[length] = '\0';
		}

		symbol_table_node_ptr enter_local(const wchar_t *p_string,
			define_code dc = DC_UNDEFINED) {
			return symtab_stack.enter_local(p_string, dc);
		}

		symbol_table_node_ptr enter_new_local(std::wstring name,
			define_code dc = DC_UNDEFINED) {
			return symtab_stack.enter_new_local(name, dc);
		}

		void enter_new_local(symbol_table_node_ptr &p_new_id) {
			symtab_stack.enter_new_local(p_new_id);
		}

		void enter_new_function(symbol_table_node_ptr &p_new_id) {
			symtab_stack.enter_new_function(p_new_id);
		}

		void conditional_get_token(token_code tc, error_code ec) {
			if (tc == token)get_token();
			else cx_error(ec);
		}

		void conditional_get_token_append(token_code tc, error_code ec) {
			if (tc == token) get_token();
			else cx_error(ec);
		}

		void resync(const token_code *p_list1,
			const token_code *p_list2 = nullptr,
			const token_code *p_list3 = nullptr);

		void emit(symbol_table_node_ptr &p_function_id, opcode op1);
		void emit(symbol_table_node_ptr &p_function_id, opcode op1, value arg1);
		void emit(symbol_table_node_ptr &p_function_id, opcode op1, value arg1, value arg2);
		void emit_store(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id);
		void emit_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id);
		void emit_ax_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, type_ptr &p_type);
		void emit_ax_store(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id);
		void emit_inc(symbol_table_node_ptr &p_function_id, type_ptr &p_type, value v_);
		void emit_add(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_sub(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_const(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id);
		void emit_mul(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_div(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_mod(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_not_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_eq_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_lt(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_gt(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_lt_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_gt_eq(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_lnot(symbol_table_node_ptr &p_function_id, type_ptr &p_type);
		void emit_store_no_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id);
	public:

		parser(text_in_buffer *p_buffer, bool std_lib_module = false)
			: p_scanner(new text_scanner(p_buffer)), is_module(std_lib_module) {
			file_name = p_buffer->file_name();
		}

		~parser(void) {
			delete p_scanner;
		}

		symbol_table_node_ptr parse(void);
	};
}
#endif