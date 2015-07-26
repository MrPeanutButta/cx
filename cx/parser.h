#ifndef PARSER_H
#define PARSER_H
//#pragma once
#include <string>
//#include "misc.h"
#include "buffer.h"
#include "error.h"
#include "token.h"
#include "scanner.h"
#include "types.h"
#include "symtab.h"

namespace cx{

	extern symbol_table_ptr p_global_symtab;

	///  cx_parser     Parser class.

	class parser {
		text_scanner * const p_scanner; // ptr to the scanner
		token *p_token; // ptr to the current token
		token_code token; // code of current token
		symbol_table_stack symtab_stack;
		bool is_module;
		std::string file_name;

		symbol_table_node_ptr parse_function_header(symbol_table_node_ptr &p_function_id);

		void parse_block(symbol_table_node_ptr &p_function_id);
		void parse_iterator_block(symbol_table_node_ptr &p_function_id);
		void parse_formal_parm_list(symbol_table_node_ptr &p_function_id);

		cx_type::type_ptr parse_subroutine_call(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node_id);
		cx_type::type_ptr parse_declared_subroutine_call(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node_id);

		void parse_actual_parm_list(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node_id);
		void parse_actual_parm(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node_id, symbol_table_node_ptr &p_formal_id);

		// declarations
		symbol_table_node_ptr allocate_new_node(symbol_table_node_ptr &p_function_id);
		void parse_declarations_or_assignment(symbol_table_node_ptr &p_function_id);
		void parse_constant_declaration(symbol_table_node_ptr &p_function_id);
		void parse_constant(symbol_table_node_ptr &p_const_id);
		void parse_identifier_constant(symbol_table_node_ptr &p_id1, token_code sign);

		void parse_definitions(symbol_table_node_ptr &p_function_id);
		//void ParseIntegerDeclaration(symbol_table_node_ptr &p_function_id);
		void parse_type_definitions(symbol_table_node_ptr &p_function_id);
		cx_type::type_ptr parse_type_spec(symbol_table_node_ptr &p_node);

		cx_type::type_ptr parse_identifier_type(symbol_table_node_ptr &p_id2);

		cx_type::type_ptr parse_enum_header(symbol_table_node_ptr &p_function_id);
		cx_type::type_ptr parse_enumeration_type(void);

		cx_type::type_ptr parse_subrange_type(symbol_table_node_ptr &p_min_id);
		cx_type::type_ptr parse_subrange_limit(symbol_table_node_ptr &p_limit_id, int &limit);

		cx_type::type_ptr parse_string_type(symbol_table_node_ptr &p_function_id,
			symbol_table_node_ptr &p_string_node);
		cx_type::type_ptr parse_array_type(symbol_table_node_ptr &p_function_id,
			symbol_table_node_ptr &p_array_node);
		cx_type::type_ptr parse_unksize_array_type(symbol_table_node_ptr &p_function_id,
			symbol_table_node_ptr &p_array_node);

		void parse_index_type(symbol_table_node_ptr &p_array_node);
		size_t array_size(cx_type::type_ptr &p_array_type);
		cx_type::type_ptr parse_complex_type(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node);

		//void parse_variable_declarations(symbol_table_node_ptr &p_function_id);
		void parse_member_decls(symbol_table_node_ptr &p_function_id, cx_type::type_ptr p_complex_type, int offset);
		//void ParseVarOrFieldDecls(symbol_table_node_ptr &p_function_id,
		//cx_type::type_ptr p_complex_type,
		//int offset);
		//symbol_table_node_ptr &parse_id_sublist(const symbol_table_node_ptr &p_function_id,
		//const cx_type::type_ptr &p_complex_type, symbol_table_node_ptr &p_last_id);

		// expressions
		cx_type::type_ptr parse_rvalue(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &lhs, cx_type::type_ptr &rhs);
		cx_type::type_ptr parse_expression(symbol_table_node_ptr &p_function_id);
		//cx_type::type_ptr ParseSuffix(symbol_table_node_ptr &p_node);
		//void ParseSizeOf(void);
		cx_type::type_ptr parse_simple_expression(symbol_table_node_ptr &p_function_id);
		cx_type::type_ptr parse_term(symbol_table_node_ptr &p_function_id);
		cx_type::type_ptr parse_factor(symbol_table_node_ptr &p_function_id);
		cx_type::type_ptr parse_variable(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, bool reference = false);
		cx_type::type_ptr parse_subscripts(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type);
		cx_type::type_ptr parse_field(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_node, cx_type::type_ptr &p_type);
		cx_type::type_ptr parse_iterator(symbol_table_node_ptr &p_iterator);

		// statements
		void parse_statement(symbol_table_node_ptr &p_function_id);
		cx_type::type_ptr parse_assignment(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_target_id);
		void parse_statement_list(symbol_table_node_ptr &p_function_id, token_code terminator);
		void parse_DO(symbol_table_node_ptr &p_function_id);
		void parse_WHILE(symbol_table_node_ptr &p_function_id);
		void parse_IF(symbol_table_node_ptr &p_function_id);
		void parse_FOR(symbol_table_node_ptr &p_function_id);
		void parse_SWITCH(symbol_table_node_ptr &p_function_id);
		void parse_case_branch(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_expr_type);
		void parse_case_label(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_expr_type);
		void parse_compound(symbol_table_node_ptr &p_function_id);
		void parse_RETURN(symbol_table_node_ptr &p_function_id);

		void parse_execute_directive(symbol_table_node_ptr &p_function_id);

		void get_token(void) {
			p_token = p_scanner->get();
			token = p_token->code();
		}

		// TODO - this should be deleted
		void get_token_append(void) {
			get_token();
			//icode.put(token);
		}

		void insert_line_marker(void) {
			//icode.insert_line_marker();
		}

/*		int put_location_marker(void) {
			return icode.put_location_marker();
		}*/

	/*	void fixup_location_marker(int location) {
			icode.fixup_location_marker(location);
		}*/

		symbol_table_node_ptr search_local(const std::string name) {
			return symtab_stack.search_local(name);
		}

		symbol_table_node_ptr search_all(const std::string name) const {
			return symtab_stack.search_all(name);
		}

		symbol_table_node_ptr find(const std::string name) const {
			symbol_table_node_ptr p_node = search_all(name);

			if (p_node == nullptr){
				cx_error(ERR_UNDEFINED_IDENTIFIER);
			}

			return p_node;
		}

		void copy_quoted_string(char *p_string, const char *p_quoted_string) const {
			int length = strlen(p_quoted_string) - 2;
			strncpy(p_string, &p_quoted_string[1], length);
			p_string[length] = '\0';
		}

		symbol_table_node_ptr enter_local(const char *p_string,
			define_code dc = DC_UNDEFINED) {
			return symtab_stack.enter_local(p_string, dc);
		}

		symbol_table_node_ptr enter_new_local(const char *p_string,
			define_code dc = DC_UNDEFINED) {
			return symtab_stack.enter_new_local(p_string, dc);
		}

		void conditional_get_token(token_code tc, error_code ec) {
			if (tc == token)get_token();
			else cx_error(ec);
		}

		void conditional_get_token_append(token_code tc, error_code ec) {
			if (tc == token) get_token_append();
			else cx_error(ec);
		}

		void resync(const token_code *p_list1,
			const token_code *p_list2 = nullptr,
			const token_code *p_list3 = nullptr);

		void emit(symbol_table_node_ptr &p_function_id, opcode op1);
		void emit(symbol_table_node_ptr &p_function_id, opcode op1, value arg1);
		void emit(symbol_table_node_ptr &p_function_id, opcode op1, value arg1, value arg2);
		void emit_store(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id);
		void emit_load(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, bool reference);
		void emit_inc(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id, value v_, cx::opcode order_op);
		void emit_add(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type);
		void emit_sub(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type);
		void emit_const(symbol_table_node_ptr &p_function_id, symbol_table_node_ptr &p_id);
		void emit_mul(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type);
		void emit_div(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type);
		void emit_mod(symbol_table_node_ptr &p_function_id, cx_type::type_ptr &p_type);
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