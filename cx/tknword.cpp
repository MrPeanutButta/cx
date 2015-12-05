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
	std::pair<std::wstring, token_code> map_data[] = {
		std::make_pair(L"if", TC_IF),
		std::make_pair(L"return", TC_RETURN),
		std::make_pair(L"continue", TC_CONTINUE),
		std::make_pair(L"friend", TC_FRIEND),
		std::make_pair(L"goto", TC_GOTO),
		std::make_pair(L"try", TC_TRY),
		std::make_pair(L"delete", TC_DELETE),
		std::make_pair(L"typeid", TC_TYPEID),
		std::make_pair(L"do", TC_DO),
		std::make_pair(L"signed", TC_SIGNED),
		std::make_pair(L"typename", TC_TYPENAME),
		std::make_pair(L"break", TC_BREAK),
		std::make_pair(L"sizeof", TC_SIZEOF),
		std::make_pair(L"case", TC_CASE),
		std::make_pair(L"static", TC_STATIC),
		std::make_pair(L"unsigned", TC_UNSIGNED),
		std::make_pair(L"catch", TC_CATCH),
		std::make_pair(L"else", TC_ELSE),
		std::make_pair(L"namespace", TC_NAMESPACE),
		std::make_pair(L"using", TC_USING),
		std::make_pair(L"new", TC_NEW),
		std::make_pair(L"virtual", TC_VIRTUAL),
		std::make_pair(L"explicit", TC_EXPLICIT),
		std::make_pair(L"noexcept", TC_NOEXCEPT),
		std::make_pair(L"export", TC_EXPORT),
		std::make_pair(L"enum", TC_ENUM),
		std::make_pair(L"switch", TC_SWITCH),
		std::make_pair(L"extern", TC_EXTERN),
		std::make_pair(L"operator", TC_OPERATOR),
		std::make_pair(L"template", TC_TEMPLATE),
		std::make_pair(L"const", TC_CONST),
		std::make_pair(L"private", TC_PRIVATE),
		std::make_pair(L"this", TC_THIS),
		std::make_pair(L"while", TC_WHILE),
		std::make_pair(L"protected", TC_PROTECTED),
		std::make_pair(L"thread_local", TC_THREADLOCAL),
		std::make_pair(L"for", TC_FOR),
		std::make_pair(L"public", TC_PUBLIC),
		std::make_pair(L"throw", TC_THROW),
		std::make_pair(L"default", TC_DEFAULT),
		std::make_pair(L"typedef", TC_TYPEDEF),
		std::make_pair(L"mutable", TC_MUTABLE),
		std::make_pair(L"include", TC_INCLUDE),
		std::make_pair(L"warn", TC_WARN),
		std::make_pair(L"import", TC_IMPORT),
		std::make_pair(L"asm", TC_ASM)
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
		TC_VIRTUAL, TC_EXPORT, TC_ENUM,
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

	// TODO add ++ --
	const token_code tokenlist_unary_ops[] = {
		TC_PLUS, TC_MINUS, TC_BIT_NOT, TC_LOGIC_NOT, TC_DUMMY
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

		wchar_t ch = buffer.current_char(); // char fetched from input
		wchar_t *ps = this->string;

		// get the word.
		do {
			*ps++ = ch;
			ch = buffer.get_char();
		} while ((char_map[ch] == CC_LETTER)
			|| (char_map[ch] == CC_DIGIT));

		*ps = L'\0';

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
//			sprintf(list.text, "\t%-18s %-s", ">> identifier:", string);
		}
		else {
//			sprintf(list.text, "\t%-18s %-s", ">> reserved word:", string);
		}

		list.put_line();
	}
}