#include "common.h"
#include "parser.h"
#include "error.h"

namespace cx{
	/** parse_type_definitions    parse a list of type definitions
	 *                          separated by commas:
	 *
	 *                              <type> <id>, ... ;
	 *
	 * @param p_function_id : ptr to symbol table node of program,
	 *                     procedure, or function identifier.
	 */
	void parser::parse_type_definitions(symbol_table_node_ptr &p_function_id) {
		symbol_table_node_ptr p_last_id;// = nullptr;

		while (token == TC_IDENTIFIER) {
			symbol_table_node_ptr p_type_id = enter_new_local(p_token->string__());

			/*if (p_function_id->defined.routine.locals.types == nullptr) {
				p_function_id->defined.routine.locals.types.symbols.emplace(p_type_id);
			}
			else {
				//p_last_id->next__ = p_type_id;
			}*/

			p_last_id = p_type_id;

			get_token_append();
			conditional_get_token(TC_EQUAL, ERR_MISSING_EQUAL);

			//        set_type(p_type_id->p_type, parse_type_spec());
			p_type_id->defined.defined_how = DC_TYPE;

			if (!p_type_id->p_type->p_type_id) {
				p_type_id->p_type->p_type_id = p_type_id;
			}

			resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);
			conditional_get_token(TC_SEMICOLON, ERR_MISSING_SEMICOLON);

			while (token == TC_SEMICOLON)get_token_append();

			resync(tokenlist_declaration_follow, tokenlist_declaration_start, tokenlist_statement_start);

		}
	}

	/** parse_type_spec       parse a type specification.
	 *
	 * @param p_node : ptr to object node.
	 * @return ptr to type object.
	 */
	cx_type::type_ptr parser::parse_type_spec(symbol_table_node_ptr &p_node) {
		switch (token) {
		case TC_IDENTIFIER:
		{
			symbol_table_node_ptr p_id = find(p_token->string__());

			switch (p_id->defined.defined_how) {
			case DC_TYPE: return parse_identifier_type(p_id);
			case DC_CONSTANT: return parse_subrange_type(p_id);
			default:
				cx_error(ERR_NOT_A_TYPE_IDENTIFIER);
				get_token_append();
				return (p_dummy_type);
			}
		}

		case TC_LEFT_BRACKET: return parse_enumeration_type();

			/* found empty subscript.
			 * array must have initializer */
		case TC_RIGHT_SUBSCRIPT: return parse_assignment(p_node);
			//        case tcRecord: return parse_complex_type();
		case TC_PLUS:
		case TC_MINUS:
		case TC_NUMBER:
		case TC_CHAR:
		case TC_STRING:// return parse_subrange_type(nullptr);

		default:
			cx_error(ERR_INVALID_TYPE);
			return (p_dummy_type);

		}
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
	cx_type::type_ptr parser::parse_identifier_type(const symbol_table_node_ptr &p_id2) {
		get_token_append();
		return p_id2->p_type;
	}

	/** parse_enumeration_type    parse a enumeration type
	 *                          specification:
	 *      enum <id> { <enum-list> };
	 *
	 * @return
	 */
	/*cx_type *cx_parser::parse_enumeration_type(void) {
		cx_type *p_type = new cx_type(fc_enum, sizeof(int), nullptr);
		cx_symtab_node *p_last_id = nullptr;

		int const_value = -1;

		get_token_append();
		resync(tokenlist_enum_const_start);

		while (token == tc_identifier) {
			cx_symtab_node *p_const_id = enter_new_local(p_token->string__());
			++const_value;

			if (p_const_id->defn.how == dc_undefined) {
				p_const_id->defn.how = dc_constant;

				p_const_id->defn.constant.value.int__ = const_value;
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
}

