#include <utility>
#include <cstring>
#include <cstdio>
#include "token.h"
#include "scanner.h"

/*************************
 *                       *
 *  Reserved Word Table  *
 *                       *
 *************************/

namespace cx{

	///  Reserved word lists
	std::pair<std::string, token_code> map_data[] = {
		std::make_pair("if", TC_IF),
		std::make_pair("return", TC_RETURN),
		std::make_pair("continue", TC_CONTINUE),
		std::make_pair("friend", TC_FRIEND),
		std::make_pair("goto", TC_GOTO),
		std::make_pair("try", TC_TRY),
		std::make_pair("delete", TC_DELETE),
		std::make_pair("typeid", TC_TYPEID),
		std::make_pair("do", TC_DO),
		std::make_pair("signed", TC_SIGNED),
		std::make_pair("typename", TC_TYPENAME),
		std::make_pair("break", TC_BREAK),
		std::make_pair("sizeof", TC_SIZEOF),
		std::make_pair("case", TC_CASE),
		std::make_pair("static", TC_STATIC),
		std::make_pair("unsigned", TC_UNSIGNED),
		std::make_pair("catch", TC_CATCH),
		std::make_pair("else", TC_ELSE),
		std::make_pair("namespace", TC_NAMESPACE),
		std::make_pair("using", TC_USING),
		std::make_pair("new", TC_NEW),
		std::make_pair("virtual", TC_VIRTUAL),
		std::make_pair("explicit", TC_EXPLICIT),
		std::make_pair("noexcept", TC_NOEXCEPT),
		std::make_pair("export", TC_EXPORT),
		std::make_pair("switch", TC_SWITCH),
		std::make_pair("extern", TC_EXTERN),
		std::make_pair("operator", TC_OPERATOR),
		std::make_pair("template", TC_TEMPLATE),
		std::make_pair("const", TC_CONST),
		std::make_pair("private", TC_PRIVATE),
		std::make_pair("this", TC_THIS),
		std::make_pair("while", TC_WHILE),
		std::make_pair("protected", TC_PROTECTED),
		std::make_pair("thread_local", TC_THREADLOCAL),
		std::make_pair("for", TC_FOR),
		std::make_pair("public", TC_PUBLIC),
		std::make_pair("throw", TC_THROW),
		std::make_pair("default", TC_DEFAULT),
		std::make_pair("typedef", TC_TYPEDEF),
		std::make_pair("mutable", TC_MUTABLE),
		std::make_pair("include", TC_INCLUDE),
		std::make_pair("warn", TC_WARN),
		std::make_pair("import", TC_IMPORT),
	};

	token_map cx_reserved_words(map_data, map_data + sizeof map_data / sizeof map_data[0]);

	// tokens that start a declaration
	const token_code tokenlist_declaration_start[] = {
		TC_SIGNED,
		TC_STATIC, TC_UNSIGNED,
		TC_NAMESPACE, TC_USING, TC_CHAR, TC_VIRTUAL,
		TC_EXPORT,
		TC_EXTERN, TC_TEMPLATE, TC_CONST, TC_PRIVATE,
		TC_PROTECTED, TC_THREADLOCAL, TC_PUBLIC, TC_TYPEDEF,
		TC_POUND, TC_DUMMY
	};

	// tokens that follow a declaration
	const token_code tokenlist_declaration_follow[] = {
		TC_SEMICOLON, TC_IDENTIFIER, TC_DUMMY
	};

	// identifier start
	const token_code tokenlist_identifier_start[] = {
		TC_IDENTIFIER, TC_DUMMY
	};

	// tokens that follow a sublist
	const token_code tokenlist_sublist_follow[] = {
		TC_COLON, TC_DUMMY
	};

	// tokens that follow a member/field declaration
	const token_code tokenlist_field_decl_follow[] = {
		TC_SEMICOLON, TC_IDENTIFIER, TC_RIGHT_BRACKET, TC_DUMMY
	};

	// tokens that start an enum
	// XXX broken
	const token_code tokenlist_enum_const_start[] = {
		TC_IDENTIFIER, TC_DUMMY
	};

	// tokens that follow an enum
	// XXX not implemented
	const token_code tokenlist_enum_const_follow[] = {
		TC_RIGHT_BRACKET, TC_COMMA, TC_SEMICOLON, TC_DUMMY
	};

	// tokens that follow subrange
	// XXX deprecated from the old Pascal Interp
	const token_code tokenlist_subrange_limit_follow[] = {
		TC_DOT_DOT, TC_IDENTIFIER, TC_PLUS, TC_MINUS, TC_STRING,
		TC_RIGHT_SUBSCRIPT, TC_COMMA, TC_SEMICOLON, TC_DUMMY
	};

	// tokens that start an index
	// XXX broken
	const token_code tokenlist_index_start[] = {
		TC_IDENTIFIER, TC_NUMBER, TC_STRING, TC_LEFT_PAREN, TC_PLUS, TC_MINUS,
		TC_LEFT_SUBSCRIPT, TC_RIGHT_SUBSCRIPT, TC_DUMMY
	};

	// tokens that follow an index
	const token_code tokenlist_index_follow[] = {
		TC_COMMA, TC_RIGHT_SUBSCRIPT, TC_SEMICOLON, TC_DUMMY
	};

	// XXX idk if this is used
	const token_code tokenlist_index_list_follow[] = {
		TC_IDENTIFIER, TC_RIGHT_SUBSCRIPT, TC_LEFT_PAREN, TC_PLUS, TC_MINUS, TC_NUMBER,
		TC_STRING, TC_SEMICOLON,
		TC_DUMMY
	};

	// XXX deprecated
	const token_code tokenlist_subscript_or_field_start[] = {
		TC_COLON_COLON,
		TC_POINTER_MEMBER, TC_MEMBER_POINTER, TC_DOT, TC_LEFT_SUBSCRIPT, TC_DUMMY
	};

	// tokens that follow an identifier
	const token_code tokenlist_identifier_follow[] = {
		TC_COMMA, TC_IDENTIFIER, TC_COLON, TC_COLON_COLON, TC_SEMICOLON,
		TC_POINTER_MEMBER, TC_RIGHT_PAREN, TC_MEMBER_POINTER, TC_DOT, TC_DUMMY
	};

	// tokens that can start a statement
	const token_code tokenlist_statement_start[] = {
		TC_SWITCH, TC_FOR, TC_DO, TC_WHILE, TC_IDENTIFIER,
		TC_COLON_COLON, TC_RETURN, TC_CONTINUE, TC_IF,
		TC_FRIEND, TC_GOTO, TC_TRY, TC_DELETE,
		TC_SIGNED, TC_BREAK, TC_STATIC,
		TC_UNSIGNED, TC_CATCH, TC_NAMESPACE, TC_USING, TC_CHAR,
		TC_VIRTUAL, TC_EXPORT,
		TC_EXTERN, TC_TEMPLATE, TC_CONST,
		TC_PRIVATE, TC_THIS, TC_PROTECTED, TC_THREADLOCAL,
		TC_PUBLIC, TC_THROW, TC_TYPEDEF, TC_POUND, TC_LEFT_BRACKET,
		TC_DUMMY
	};

	// tokens that can follow a statement
	const token_code tokenlist_statement_follow[] = {
		TC_SEMICOLON, TC_LEFT_BRACKET, TC_RIGHT_BRACKET, TC_ELSE, TC_WHILE,
		TC_DUMMY
	};

	const token_code tokenlist_caselabel_start[] = {
		TC_LEFT_BRACKET, TC_CASE, TC_DUMMY
	};

	const token_code tokenlist_expression_start[] = {
		TC_PLUS, TC_MINUS, TC_IDENTIFIER, TC_NUMBER, TC_STRING,
		TC_BIT_NOT, TC_LOGIC_NOT, TC_LEFT_PAREN, TC_BIT_AND,
		TC_LEFT_SUBSCRIPT, TC_DUMMY
	};

	const token_code tokenlist_expression_follow[] = {
		TC_COMMA, TC_RIGHT_PAREN, TC_RIGHT_SUBSCRIPT, TC_PLUS_PLUS,
		TC_MINUS_MINUS, TC_COLON, TC_RIGHT_BRACKET, TC_SEMICOLON, TC_DOT,
		TC_EQUAL_EQUAL, TC_NOT_EQUAL, TC_LESSTHAN, TC_GREATERTHAN, TC_LESSTHAN_EQUAL, TC_GREATERTHAN_EQUAL,
		TC_DUMMY
	};

	const token_code tokenlist_assign_ops[] = {
		TC_BIT_XOR_EQUAL, // ^=
		TC_BIT_AND_EQUAL, // &=
		TC_BIT_OR_EQUAL, // |=
		TC_BIT_LEFTSHIFT_EQUAL, // <<=
		TC_BIT_RIGHTSHIFT_EQUAL, // >>=

		TC_MINUS_EQUAL, // -=
		TC_PLUS_EQUAL, // +=
		TC_MINUS_MINUS, // --
		TC_PLUS_PLUS, // ++
		TC_DIVIDE_EQUAL, // /=
		TC_STAR_EQUAL, // *=
		TC_MODULAS_EQUAL, // %=
		TC_EQUAL, // =
		TC_RETURN, // RETURN (EXPR)
		TC_DUMMY
	};

	const token_code tokenlist_relation_ops[] = {
		TC_EQUAL_EQUAL, TC_NOT_EQUAL, TC_LESSTHAN, TC_GREATERTHAN, TC_LESSTHAN_EQUAL, TC_GREATERTHAN_EQUAL, TC_DUMMY
	};

	const token_code tokenlist_unary_ops[] = {
		TC_PLUS, TC_MINUS, TC_BIT_NOT, TC_DUMMY
	};

	const token_code tokenlist_add_ops[] = {
		TC_PLUS, TC_MINUS, TC_LOGIC_OR, TC_BIT_LEFTSHIFT, TC_BIT_RIGHTSHIFT,
		TC_BIT_AND, TC_BIT_XOR, TC_BIT_OR, TC_DUMMY
	};

	const token_code tokenlist_mul_ops[] = {
		TC_STAR, TC_DIVIDE, TC_MODULAS, TC_LOGIC_AND, TC_DUMMY
	};

	const token_code tokenlist_program_end[] = {
		TC_RETURN, TC_RIGHT_BRACKET, TC_END_OF_FILE, TC_DUMMY
	};

	const token_code tokenlist_equal_equal[] = { TC_EQUAL_EQUAL, TC_DUMMY };
	const token_code tokenlist_do[] = { TC_DO, TC_DUMMY };
	const token_code tokenlist_left_bracket[] = { TC_LEFT_BRACKET, TC_DUMMY };
	const token_code tokenlist_colon[] = { TC_COLON, TC_DUMMY };
	const token_code tokenlist_right___bracket[] = { TC_RIGHT_BRACKET, TC_DUMMY };
	const token_code tokenlist_semicolon[] = { TC_SEMICOLON, TC_DUMMY };
	const token_code tokenlist_right___paren[] = { TC_RIGHT_PAREN, TC_DUMMY };
	const token_code tokenlist_left_paren[] = { TC_LEFT_PAREN, TC_DUMMY };

	bool token_in(token_code tc, const token_code *p_list) {
		const token_code *p_code;

		if (!p_list) return false;

		for (p_code = p_list; *p_code; ++p_code) {
			if (*p_code == tc) return true;
		}

		return false;
	}

	/*****************
	 *               *
	 *  Word Tokens  *
	 *               *
	 *****************/

	/** get     Extract a word token from the source and downshift
	 *          its characters.  Check if it's a reserved word.
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void word_token::get(text_in_buffer &buffer) {
		extern cx::char_code_map char_map;

		char ch = buffer.current_char(); // char fetched from input
		char *ps = this->string;

		// get the word.
		do {
			*ps++ = ch;
			ch = buffer.get_char();
		} while ((char_map[ch] == CC_LETTER)
			|| (char_map[ch] == CC_DIGIT));

		*ps = '\0';

		check_for_reserved_word();
	}

	/** check_for_reserved_word    Is the word token a reserved word?
	 *                          If yes, set the its token code to
	 *                          the appropriate code.  If not, set
	 *                          the token code to tc_identifier.
	 */
	void word_token::check_for_reserved_word(void) {

		code__ = TC_IDENTIFIER; // first assume it's an identifier

		/* from the reserved word table, check to see if the word
		 * is in there. */
		token_code c_ = cx_reserved_words[this->string];
		if (c_ > 0) code__ = c_;

	}

	/** print       print the token to the list file.
	 */
	void word_token::print(void) const {
		if (code__ == TC_IDENTIFIER) {
			sprintf(list.text, "\t%-18s %-s", ">> identifier:", string);
		}
		else {
			sprintf(list.text, "\t%-18s %-s", ">> reserved word:", string);
		}

		list.put_line();
	}
}