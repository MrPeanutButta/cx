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
	wchar_t token::get_escape_char(const wchar_t& c) {
		switch (c) {
		case L'\'': return L'\'';
		case L'\"': return L'\"';
		case L'\?': return L'\?';
		case L'\\': return L'\\';
		case L'a': return L'\a';
		case L'b': return L'\b';
		case L'f': return L'\f';
		case L'n': return L'\n';
		case L'r': return L'\r';
		case L't': return L'\t';
		case L'v': return L'\v';
		case L'0': return L'\0';
		default:
			cx_error(ERR_INVALID_ESCAPE_CHAR);
		}

		return L'\0';
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
		wchar_t ch = buffer.current_char(); // current character

		wchar_t *ps = string; // ptr to char in string

		*ps++ = L'\"'; // opening quote

		// get the string.
	
		ch = buffer.get_char(); // first char after opening quote
		
		while (ch != EOF_CHAR) {
		
			if (ch == L'\"') { // look for another quote

				/* Fetched a quote.  Now check for an adjacent quote,
				 * since two consecutive quotes represent a single
				 * quote in the string. */
				ch = buffer.get_char();
				if (ch != L'\"') break; /* not another quote, so previous
										* quote ended the string */
			}// Replace the end of line character with a blank.
			else if (ch == L'\0') ch = L' ';

			if (ch == L'\\') {
				*ps++ = get_escape_char(buffer.get_char());
			}
			else {
				// Append current char to string, then get the next__ char.
				*ps++ = ch;
			}

			ch = buffer.get_char();
		}

		if (ch == EOF_CHAR) cx_error(ERR_UNEXPECTED_EOF);

		*ps++ = L'\"'; // closing quote
		*ps = L'\0';
	}

	void string_token::wget(text_in_buffer &buffer) {
		wchar_t ch = (buffer.current_char() | buffer.current_char() | buffer.current_char() | buffer.current_char());

		// TODO check for L call wget and return

		wchar_t *ps = string; // ptr to char in string

		*ps++ = L'\"'; // opening quote

		// get the string.

		ch = buffer.get_char(); // first char after opening quote

		while (ch != EOF_CHAR) {

			if (ch == L'\"') { // look for another quote

				/* Fetched a quote.  Now check for an adjacent quote,
				* since two consecutive quotes represent a single
				* quote in the string. */
				ch = buffer.get_char();
				if (ch != L'\"') break; /* not another quote, so previous
									   * quote ended the string */
			}// Replace the end of line character with a blank.
			else if (ch == L'\0') ch = L' ';

			if (ch == L'\\') {
				// TODO needs to check wchar
				*ps++ = get_escape_char(buffer.get_char());
			}
			else {
				// Append current char to string, then get the next__ char.
				*ps++ = ch;
			}

			ch = buffer.get_char();
		}

		if (ch == EOF_CHAR) cx_error(ERR_UNEXPECTED_EOF);

		*ps++ = L'\"'; // closing quote
		*ps = L'\0';

		this->code__ = TC_WSTRING;
	}

	/** get         Extract single quoted char ' '
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void char_token::get(text_in_buffer &buffer) {

		wchar_t ch = buffer.current_char(); // current character
		wchar_t *ps = string; // ptr to char in string

		*ps++ = '\''; // opening quote
		// get the string.
		ch = buffer.get_char(); // first char after opening quote

		if (ch == L'\\') {
			*ps++ = get_escape_char(buffer.get_char());
		}
		else {
			*ps++ = ch;
		}

		// Append current char to string, then get the next__ char.
		ch = buffer.get_char();

		if (ch != L'\'') cx_error(ERR_MISSING_SINGLE_QUOTE);
		if (ch == EOF_CHAR) cx_error(ERR_UNEXPECTED_EOF);

		ch = buffer.get_char();
		*ps++ = L'\''; // closing quote
		*ps = L'\0';
	}

	void char_token::print(void) const {
//		sprintf(list.text, "\t%-18s %-s", ">> char:", string);
		list.put_line();
	}

	/** print       print the token to the list file.
	 *
	 */
	void string_token::print(void) const {
//		sprintf(list.text, "\t%-18s %-s", ">> string:", string);
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
		wchar_t ch = buffer.current_char();
		wchar_t *ps = string;
		
		*ps++ = ch;

		switch (ch) {
		case L'^': ch = buffer.get_char();
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_BIT_XOR_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_BIT_XOR;
			break;
		case L'&': ch = buffer.get_char();
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_BIT_AND_EQUAL;
				buffer.get_char();
			}
			else if (ch == L'&') {
				*ps++ = L'&';
				code__ = TC_LOGIC_AND;
				buffer.get_char();
			}
			else code__ = TC_BIT_AND;
			break;
		case L'|': ch = buffer.get_char();
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_BIT_OR_EQUAL;
				buffer.get_char();
			}
			else if (ch == L'|') {
				*ps++ = L'|';
				code__ = TC_LOGIC_OR;
				buffer.get_char();
			}
			else code__ = TC_BIT_OR;
			break;
		case L'~': ch = buffer.get_char();
			code__ = TC_BIT_NOT;
			break;
		case L'*': ch = buffer.get_char();
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_STAR_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_STAR;
			break;
		case L'(': code__ = TC_LEFT_PAREN;
			buffer.get_char();
			break;
		case L')': code__ = TC_RIGHT_PAREN;
			buffer.get_char();
			break;
		case L'-': ch = buffer.get_char();
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_MINUS_EQUAL;
				buffer.get_char();
			}
			else if (ch == L'-') {
				*ps++ = L'-';
				code__ = TC_MINUS_MINUS;
				buffer.get_char();
			}
			else if (ch == L'>') {
				*ps++ = L'>';
				code__ = TC_POINTER_MEMBER;
				buffer.get_char();
			}
			else code__ = TC_MINUS;
			break;
		case L'+': ch = buffer.get_char();
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_PLUS_EQUAL;
				buffer.get_char();
			}
			else if (ch == L'+') {
				*ps++ = L'+';
				code__ = TC_PLUS_PLUS;
				buffer.get_char();
			}
			else code__ = TC_PLUS;
			break;
		case L'=': ch = buffer.get_char();
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_EQUAL_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_EQUAL;
			break;
		case L'[': code__ = TC_LEFT_SUBSCRIPT;
			buffer.get_char();
			break;
		case L']': code__ = TC_RIGHT_SUBSCRIPT;
			buffer.get_char();
			break;
		case L'{': code__ = TC_LEFT_BRACKET;
			buffer.get_char();
			break;
		case L'}': code__ = TC_RIGHT_BRACKET;
			buffer.get_char();
			break;
		case L';': code__ = TC_SEMICOLON;
			buffer.get_char();
			break;
		case L',': code__ = TC_COMMA;
			buffer.get_char();
			break;
		case L'/': ch = buffer.get_char(); // /, /=
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_DIVIDE_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_DIVIDE;
			break;
		case L':': ch = buffer.get_char(); // : or ::
			if (ch == L':') {
				*ps++ = L':';
				code__ = TC_COLON_COLON;
				buffer.get_char();
			}
			else code__ = TC_COLON;
			break;
		case L'<': ch = buffer.get_char(); // < or <=
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_LESSTHAN_EQUAL;
				buffer.get_char();
			}
			else if (ch == L'<') {
				*ps++ = L'<';
				ch = buffer.get_char();
				if (ch == L'=') {
					*ps++ = L'=';
					code__ = TC_BIT_LEFTSHIFT_EQUAL;
					buffer.get_char();
				}
				else code__ = TC_BIT_LEFTSHIFT;
			}
			else code__ = TC_LESSTHAN;
			break;
		case L'>': ch = buffer.get_char(); // > or >=
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_GREATERTHAN_EQUAL;
				buffer.get_char();
			}
			else if (ch == L'>') {
				*ps++ = L'>';
				ch = buffer.get_char();
				if (ch == L'=') {
					*ps++ = L'=';
					code__ = TC_BIT_RIGHTSHIFT_EQUAL;
					buffer.get_char();
				}
				else code__ = TC_BIT_RIGHTSHIFT;
			}
			else code__ = TC_GREATERTHAN;
			break;
		case L'!': ch = buffer.get_char(); // ! or !=
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_NOT_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_LOGIC_NOT;
			break;
		case L'%': ch = buffer.get_char(); // ! or !=
			if (ch == L'=') {
				*ps++ = L'=';
				code__ = TC_MODULAS_EQUAL;
				buffer.get_char();
			}
			else code__ = TC_MODULAS;
			break;
		case L'.': ch = buffer.get_char();
			if (ch == L'.') {
				*ps++ = L'.';
				code__ = TC_DOT_DOT;
				buffer.get_char();
			}
			else code__ = TC_DOT;
			break;
		case L'?': code__ = TC_QUESTIONMARK;
			buffer.get_char();
			break;
		case L'#': code__ = TC_POUND;
			buffer.get_char();
			break;
		case L'\"': code__ = TC_DOUBLE_QUOTE;
			buffer.get_char();
		default: code__ = TC_ERROR; // error
			buffer.get_char();
			cx_error(ERR_UNRECOGNIZABLE);
			break;
		}

		*ps = L'\0';
	}

	/** print       print the token to the list file.
	 *
	 */
	void special_token::print(void) const {
//		sprintf(list.text, "\t%-18s %-s", ">> special:", string);
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
		string[1] = L'\0';

		buffer.get_char();
		cx_error(ERR_UNRECOGNIZABLE);
	}
}