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
}