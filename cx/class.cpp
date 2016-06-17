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
	void parser::parse_class_declaration(symbol_table_node_ptr &p_function_id) {
		if (token != token_code::TC_IDENTIFIER) {
			cx_error(error_code::ERR_MISSING_IDENTIFIER);
		}

		symbol_table_node_ptr p_new_id = nullptr;

		p_new_id = search_local(p_token->string);

		if (p_new_id != nullptr) {
			cx_error(error_code::ERR_REDEFINED_IDENTIFIER);
		}

		p_new_id = std::make_shared<symbol_table_node>(p_token->string, define_code::DC_TYPE);
		p_new_id->p_type = std::make_shared<cx_type>(F_REFERENCE, T_REFERENCE, 0, p_new_id, p_std_type_members);

		get_token();
		symtab_stack.enter_scope();

		parse_block(p_new_id);
		p_new_id->p_type->complex.p_class_scope = symtab_stack.exit_scope();

		p_function_id->defined.routine.p_type_ids.emplace_back(p_new_id);
	}

	void parser::parse_init(symbol_table_node_ptr &p_function_id) {
		
	}

	void parser::parse_dispose(symbol_table_node_ptr &p_function_id) {

	}
}