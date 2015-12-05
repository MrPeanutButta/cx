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

#ifndef TOKEN_H
#define TOKEN_H
#include <cstdint>
#include "error.h"
#include "buffer.h"
#include "types.h"

namespace cx{

	enum char_code {
		CC_LETTER, CC_DIGIT, CC_SPECIAL, CC_QUOTE, CC_DOUBLE_QUOTE, CC_WHITE_SPACE,
		CC_END_OF_FILE, CC_ERROR
	};
	///  cx_token_code          Token codes.

	enum token_code {
		TC_DUMMY = 0,
		TC_IDENTIFIER, TC_NUMBER, TC_STRING, TC_WSTRING, TC_CHAR, TC_END_OF_FILE, TC_ERROR,

		//OPERATORS AND PUNCTUATION
		//BITWISE
		TC_BIT_XOR, // ^
		TC_BIT_AND, // &
		TC_BIT_OR, // |
		TC_BIT_NOT, // ~
		TC_BIT_XOR_EQUAL, // ^=
		TC_BIT_AND_EQUAL, // &=
		TC_BIT_OR_EQUAL, // |=
		TC_BIT_LEFTSHIFT, // <<
		TC_BIT_LEFTSHIFT_EQUAL, // <<=
		TC_BIT_RIGHTSHIFT, // >>
		TC_BIT_RIGHTSHIFT_EQUAL, // >>=

		TC_MINUS, // -
		TC_MINUS_EQUAL, // -=
		TC_PLUS, // +
		TC_PLUS_EQUAL, // +=
		TC_EQUAL, // =
		TC_MINUS_MINUS, // --
		TC_PLUS_PLUS, // ++
		TC_DIVIDE, // /
		TC_DIVIDE_EQUAL, // /=
		TC_STAR, // *
		TC_STAR_EQUAL, // *=
		TC_LESSTHAN, // <
		TC_GREATERTHAN, // >
		TC_EQUAL_EQUAL, // ==
		TC_LESSTHAN_EQUAL, // <=
		TC_GREATERTHAN_EQUAL, // >=
		TC_NOT_EQUAL, // !=
		TC_MODULAS, // %
		TC_MODULAS_EQUAL, // %=

		//PUNCTUATION
		TC_LEFT_SUBSCRIPT, // [
		TC_RIGHT_SUBSCRIPT, // ]
		TC_QUESTIONMARK, // ?
		TC_POUND, // #
		TC_DOT_STAR, // .*
		TC_LEFT_PAREN, // (
		TC_RIGHT_PAREN, // )

		TC_LEFT_BRACKET, // {
		TC_RIGHT_BRACKET, // }
		TC_COLON, // :
		TC_SEMICOLON, // ;

		TC_COMMA, // ,
		TC_DOT_DOT_DOT, // ...

		// MEMBER ACCESS
		TC_DOT, // .
		TC_DOT_DOT, // ..
		TC_COLON_COLON, // ::
		TC_POINTER_MEMBER, // ->
		TC_MEMBER_POINTER, // ->*

		//LOGIC
		TC_LOGIC_OR, // ||
		TC_LOGIC_AND, // &&
		TC_LOGIC_NOT, // !
		TC_SINGLE_QUOTE, // '
		TC_DOUBLE_QUOTE, // "

		TC_IF, TC_RETURN, TC_CONTINUE, TC_FRIEND, TC_GOTO, TC_TRY, TC_DELETE,
		TC_TYPEID, TC_DO, TC_SIGNED, TC_TYPENAME, TC_ENUM,
		TC_BREAK, TC_SIZEOF, TC_CASE, TC_STATIC,
		TC_UNSIGNED, TC_CATCH, TC_ELSE, TC_NAMESPACE, TC_USING,
		TC_NEW, TC_VIRTUAL, TC_EXPLICIT, TC_NOEXCEPT,
		TC_EXPORT, TC_SWITCH,
		TC_EXTERN, TC_OPERATOR, TC_TEMPLATE, TC_CONST,
		TC_PRIVATE, TC_THIS, TC_WHILE, TC_PROTECTED, TC_THREADLOCAL,
		TC_FOR, TC_PUBLIC, TC_THROW, TC_DEFAULT, TC_TYPEDEF, TC_MUTABLE,

		// DIRECTIVES
		TC_INCLUDE, TC_WARN, TC_IMPORT,

		MC_LOCATION_MARKER = 126,
		MC_LINE_MARKER = 127,
		TC_ASM
	};

	typedef std::map<std::wstring, token_code> token_map;

	// tokens that can start a statement
	extern const token_code tokenlist_statement_start[];
	// tokens that can follow a statement
	extern const token_code tokenlist_statement_follow[];
	extern const token_code tokenlist_caselabel_start[];
	extern const token_code tokenlist_expression_start[];
	extern const token_code tokenlist_expression_follow[];
	extern const token_code tokenlist_assign_ops[];
	extern const token_code tokenlist_relation_ops[];
	extern const token_code tokenlist_unary_ops[];
	extern const token_code tokenlist_add_ops[];
	extern const token_code tokenlist_mul_ops[];
	extern const token_code tokenlist_program_end[];
	extern const token_code tokenlist_equal_equal[];
	extern const token_code tokenlist_do[];
	extern const token_code tokenlist_left_bracket[];
	extern const token_code tokenlist_colon[];
	extern const token_code tokenlist_right___bracket[];
	extern const token_code tokenlist_semicolon[];
	extern const token_code tokenlist_right___paren[];
	extern const token_code tokenlist_left_paren[];
	extern const token_code tokenlist_declaration_start[];
	extern const token_code tokenlist_declaration_follow[];
	extern const token_code tokenlist_identifier_start[];
	extern const token_code tokenlist_sublist_follow[];
	extern const token_code tokenlist_field_decl_follow[];
	extern const token_code tokenlist_enum_const_start[];
	extern const token_code tokenlist_enum_const_follow[];
	extern const token_code tokenlist_subrange_limit_follow[];
	extern const token_code tokenlist_index_start[];
	extern const token_code tokenlist_index_follow[];
	extern const token_code tokenlist_index_list_follow[];
	extern const token_code tokenlist_subscript_or_field_start[];
	extern const token_code tokenlist_identifier_follow[];

	bool token_in(token_code tc, const token_code *p_list);

	typedef std::map<wchar_t, char_code> char_code_map;
	extern char_code_map char_map;

	class token {
	protected:
		token_code code__;
		type_code type__;
		value value__;
		
	public:
		wchar_t string[MAX_INPUT_BUFFER_SIZE];

		token(void) {
			code__ = TC_DUMMY;
			type__ = T_DUMMY;;
			memset(&value__, 0, sizeof value__);
			memset(&string, 0, sizeof string);
		}

		token_code code() const { return code__; }
		type_code type() const { return type__; }
		value value() const { return value__; }

		wchar_t get_escape_char(const wchar_t &c);
		virtual void get(text_in_buffer &buffer) = 0;
		virtual int is_delimiter(void) const = 0;
	};

	///  word_token          Word token subclass of token.

	class word_token : public token {
		void check_for_reserved_word(void);

	public:
		virtual void get(text_in_buffer &buffer);
		virtual int is_delimiter(void) const {return false;}
		virtual void print(void) const;
	};

	//  number_token        Number token subclass of token.

	class number_token : public token {
		wchar_t ch; // char fetched from input buffer
		wchar_t *ps; // ptr into token string
		int digit_count; // total no. of digits in number
		bool count_error_flag; // true if too many digits, else false

		uint8_t radix; // number base

		int accumulate_value(text_in_buffer &buffer,
			cx_real &value, error_code ec);

		bool is_x_digit(const wchar_t &c);
		int char_value(const wchar_t &c);

	public:

		number_token() {
			code__ = TC_NUMBER;
		}

		virtual void get(text_in_buffer &buffer);
		virtual int is_delimiter(void) const {return false;}
		virtual void print(void) const;
	};

	///  string_token        string__ token subclass of token.

	class string_token : public token {
	public:

		string_token() {
			code__ = TC_STRING;
		}

		virtual void get(text_in_buffer &buffer);
		virtual int is_delimiter(void) const {return true;}
		virtual void print(void) const;
	};

	class char_token : public token {
	public:

		char_token() {
			code__ = TC_CHAR;
		}

		virtual void get(text_in_buffer &buffer);		
		virtual int is_delimiter(void) const {return true;}
		virtual void print(void) const;
	};

	///  special_token       Special token subclass of token.

	class special_token : public token {
	public:
		virtual void get(text_in_buffer &buffer);
		virtual int is_delimiter(void) const {return true;}
		virtual void print(void) const;
	};

	///  EOF_token           End-of-file token subclass of token.

	class eof_token : public token {
	public:

		virtual void get(text_in_buffer &buffer) {
			// Squelch comile errors
			if (buffer.current_char()) {}

			code__ = TC_END_OF_FILE;
		}

		virtual int is_delimiter(void) const {return false;}
		virtual void print(void) const {}
	};

	///  error_token         error token subclass of token.

	class error_token : public token {
	public:

		error_token() {
			code__ = TC_ERROR;
		}

		virtual void get(text_in_buffer &buffer);
		virtual int is_delimiter(void) const {return false;}
		virtual void print(void) const {}
	};

}
#endif
