#include <cstdio>
#include <cstdint>
#include "token.h"

namespace cx{
	/** get_escape_char             Return escape char
	 *
	 * simple-escape-sequence: one of       \’ \" \? \\
	 *                                      \a \b \f \n \r \t \v
	 * octal-escape-sequence:       \ octal-digit
	 *                              \ octal-digit octal-digit
	 *                              \ octal-digit octal-digit octal-digit
	 * hexadecimal-escape-sequence: \x hexadecimal-digit
	 *                              hexadecimal-escape-sequence hexadecimal-digit
	 *
	 * @param c : char following a \.
	 * @return the escape char.
	 */
	char token::get_escape_char(const char& c) {
		switch (c) {
		case '\'': return '\'';
		case '\"': return '\"';
		case '\?': return '\?';
		case '\\': return '\\';
		case 'a': return '\a';
		case 'b': return '\b';
		case 'f': return '\f';
		case 'n': return '\n';
		case 'r': return '\r';
		case 't': return '\t';
		case 'v': return '\v';
		case '0': return '\0';
		default:
			cx_error(ERR_INVALID_ESCAPE_CHAR);
		}

		return '\0';
	}

	/*******************
	 *                 *
	 *  string__ Tokens*
	 *                 *
	 *******************/

	/** get     get a string token from the source.
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void string_token::get(text_in_buffer &buffer) {
		char ch; // current character
		char *ps = string; // ptr to char in string

		*ps++ = '\"'; // opening quote

		// get the string.
		ch = buffer.get_char(); // first char after opening quote
		while (ch != EOF_CHAR) {
			if (ch == '\"') { // look for another quote

				/* Fetched a quote.  Now check for an adjacent quote,
				 * since two consecutive quotes represent a single
				 * quote in the string. */
				ch = buffer.get_char();
				if (ch != '\"') break; /* not another quote, so previous
										* quote ended the string */
			}// Replace the end of line character with a blank.
			else if (ch == '\0') ch = ' ';

			if (ch == '\\') {
				*ps++ = get_escape_char(buffer.get_char());
			}
			else {
				// Append current char to string, then get the next__ char.
				*ps++ = ch;
			}

			ch = buffer.get_char();
		}

		if (ch == EOF_CHAR) cx_error(ERR_UNEXPECTED_EOF);

		*ps++ = '\"'; // closing quote
		*ps = '\0';
	}

	/** get         Extract single quoted char ' '
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void char_token::get(text_in_buffer &buffer) {
		char ch; // current character
		char *ps = string; // ptr to char in string

		*ps++ = '\''; // opening quote
		// get the string.
		ch = buffer.get_char(); // first char after opening quote

		if (ch == '\\') {
			*ps++ = get_escape_char(buffer.get_char());
		}
		else {
			*ps++ = ch;
		}

		// Append current char to string, then get the next__ char.
		ch = buffer.get_char();

		if (ch != '\'') cx_error(ERR_MISSING_SINGLE_QUOTE);
		if (ch == EOF_CHAR) cx_error(ERR_UNEXPECTED_EOF);

		ch = buffer.get_char();
		*ps++ = '\''; // closing quote
		*ps = '\0';
	}

	void char_token::print(void) const {
		sprintf(list.text, "\t%-18s %-s", ">> char:", string);
		list.put_line();
	}

	/** print       print the token to the list file.
	 *
	 */
	void string_token::print(void) const {
		sprintf(list.text, "\t%-18s %-s", ">> string:", string);
		list.put_line();
	}

	/********************
	 *                  *
	 *  Special Tokens  *
	 *                  *
	 ********************/

	/** get         Extract a one-, two-, or three-character special symbol
	 *              token from the source.
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void special_token::get(text_in_buffer &buffer) {
		char ch = buffer.current_char();
		char *ps = string;

		*ps++ = ch;

		switch (ch) {
		case '^': ch = buffer.get_char();
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_BIT_XOR_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_BIT_XOR;
			break;
		case '&': ch = buffer.get_char();
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_BIT_AND_EQUAL;
				buffer.get_char();
			}
			else if (ch == '&') {
				*ps++ = '&';
				code__ = TC_LOGIC_AND;
				buffer.get_char();
			}
			else code__ = TC_BIT_AND;
			break;
		case '|': ch = buffer.get_char();
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_BIT_OR_EQUAL;
				buffer.get_char();
			}
			else if (ch == '|') {
				*ps++ = '|';
				code__ = TC_LOGIC_OR;
				buffer.get_char();
			}
			else code__ = TC_BIT_OR;
			break;
		case '~': ch = buffer.get_char();
			code__ = TC_BIT_NOT;
			break;
		case '*': ch = buffer.get_char();
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_STAR_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_STAR;
			break;
		case '(': code__ = TC_LEFT_PAREN;
			buffer.get_char();
			break;
		case ')': code__ = TC_RIGHT_PAREN;
			buffer.get_char();
			break;
		case '-': ch = buffer.get_char();
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_MINUS_EQUAL;
				buffer.get_char();
			}
			else if (ch == '-') {
				*ps++ = '-';
				code__ = TC_MINUS_MINUS;
				buffer.get_char();
			}
			else if (ch == '>') {
				*ps++ = '>';
				code__ = TC_POINTER_MEMBER;
				buffer.get_char();
			}
			else code__ = TC_MINUS;
			break;
		case '+': ch = buffer.get_char();
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_PLUS_EQUAL;
				buffer.get_char();
			}
			else if (ch == '+') {
				*ps++ = '+';
				code__ = TC_PLUS_PLUS;
				buffer.get_char();
			}
			else code__ = TC_PLUS;
			break;
		case '=': ch = buffer.get_char();
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_EQUAL_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_EQUAL;
			break;
		case '[': code__ = TC_LEFT_SUBSCRIPT;
			buffer.get_char();
			break;
		case ']': code__ = TC_RIGHT_SUBSCRIPT;
			buffer.get_char();
			break;
		case '{': code__ = TC_LEFT_BRACKET;
			buffer.get_char();
			break;
		case '}': code__ = TC_RIGHT_BRACKET;
			buffer.get_char();
			break;
		case ';': code__ = TC_SEMICOLON;
			buffer.get_char();
			break;
		case ',': code__ = TC_COMMA;
			buffer.get_char();
			break;
		case '/': ch = buffer.get_char(); // /, /=
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_DIVIDE_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_DIVIDE;
			break;
		case ':': ch = buffer.get_char(); // : or ::
			if (ch == ':') {
				*ps++ = ':';
				code__ = TC_COLON_COLON;
				buffer.get_char();
			}
			else code__ = TC_COLON;
			break;
		case '<': ch = buffer.get_char(); // < or <=
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_LESSTHAN_EQUAL;
				buffer.get_char();
			}
			else if (ch == '<') {
				*ps++ = '<';
				ch = buffer.get_char();
				if (ch == '=') {
					*ps++ = '=';
					code__ = TC_BIT_LEFTSHIFT_EQUAL;
					buffer.get_char();
				}
				else code__ = TC_BIT_LEFTSHIFT;
			}
			else code__ = TC_LESSTHAN;
			break;
		case '>': ch = buffer.get_char(); // > or >=
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_GREATERTHAN_EQUAL;
				buffer.get_char();
			}
			else if (ch == '>') {
				*ps++ = '>';
				ch = buffer.get_char();
				if (ch == '=') {
					*ps++ = '=';
					code__ = TC_BIT_RIGHTSHIFT_EQUAL;
					buffer.get_char();
				}
				else code__ = TC_BIT_RIGHTSHIFT;
			}
			else code__ = TC_GREATERTHAN;
			break;
		case '!': ch = buffer.get_char(); // ! or !=
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_NOT_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_LOGIC_NOT;
			break;
		case '%': ch = buffer.get_char(); // ! or !=
			if (ch == '=') {
				*ps++ = '=';
				code__ = TC_MODULAS_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_MODULAS;
			break;
		case '.': ch = buffer.get_char();
			if (ch == '.') {
				*ps++ = '.';
				code__ = TC_DOT_DOT;
				buffer.get_char();
			}
			else code__ = TC_DOT;
			break;
		case '?': code__ = TC_QUESTIONMARK;
			buffer.get_char();
			break;
		case '#': code__ = TC_POUND;
			buffer.get_char();
			break;
		case '\"': code__ = TC_DOUBLE_QUOTE;
			buffer.get_char();
		default: code__ = TC_ERROR; // error
			buffer.get_char();
			cx_error(ERR_UNRECOGNIZABLE);
			break;
		}

		*ps = '\0';
	}

	/** print       print the token to the list file.
	 *
	 */
	void special_token::print(void) const {
		sprintf(list.text, "\t%-18s %-s", ">> special:", string);
		list.put_line();
	}

	/*****************
	 *               *
	 *  cx_error Token  *
	 *               *
	 *****************/

	/** get         Extract an invalid character from the source.
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void error_token::get(text_in_buffer &buffer) {
		string[0] = buffer.current_char();
		string[1] = '\0';

		buffer.get_char();
		cx_error(ERR_UNRECOGNIZABLE);
	}
}