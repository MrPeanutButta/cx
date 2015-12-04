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

#include <map>
#include "scanner.h"

namespace cx{

	char_code_map char_map; // maps a character to its code

	/** Constructor     Construct a scanner by constructing the
	 *                  text input file buffer and initializing the
	 *                  character code map.
	 *
	 * @param p_buffer : ptr to text input buffer to scan.
	 */
	text_scanner::text_scanner(text_in_buffer *p_buffer)
		: p_text_in_buffer(p_buffer) {
		char i;

		// Initialize the character code map.
		for (i = L'a'; i <= L'z'; ++i) char_map[i] = CC_LETTER;
		for (i = L'A'; i <= L'Z'; ++i) char_map[i] = CC_LETTER;
		for (i = L'0'; i <= L'9'; ++i) char_map[i] = CC_DIGIT;

		char_map[L'_'] = CC_LETTER;

		char_map[L'+'] = char_map[L'-'] = CC_SPECIAL;
		char_map[L'*'] = char_map[L'/'] = CC_SPECIAL;
		char_map[L'='] = char_map[L'^'] = CC_SPECIAL;
		char_map[L'.'] = char_map[L','] = CC_SPECIAL;
		char_map[L'<'] = char_map[L'>'] = CC_SPECIAL;
		char_map[L'('] = char_map[L')'] = CC_SPECIAL;
		char_map[L'['] = char_map[L']'] = CC_SPECIAL;
		char_map[L'{'] = char_map[L'}'] = CC_SPECIAL;
		char_map[L':'] = char_map[L';'] = CC_SPECIAL;

		char_map[L'#'] = char_map[L'?'] = CC_SPECIAL;
		char_map[L'~'] = char_map[L'|'] = CC_SPECIAL;
		char_map[L'&'] = char_map[L'!'] = CC_SPECIAL;
		char_map[L'%'] = CC_SPECIAL;

		char_map[L' '] = char_map[L'\t'] = CC_WHITE_SPACE;
		char_map[L'\n'] = char_map[L'\0'] = CC_WHITE_SPACE;
		char_map[L'\r'] = char_map[L'\f'] = CC_WHITE_SPACE;
		char_map[L'\\'] = CC_WHITE_SPACE;

		char_map[L'\''] = CC_QUOTE;
		char_map[L'\"'] = CC_DOUBLE_QUOTE;

		char_map[EOF_CHAR] = CC_END_OF_FILE;

		char_map[L'`'] = char_map[L'@'] = CC_ERROR;
	}

	/** skip_whitespace      Repeatedly fetch characters from the
	 *                      text input as long as they're
	 *                      whitespace. Each comment is a whitespace
	 *                      character.
	 *
	 */
	void text_scanner::skip_whitespace(void) {
		wchar_t ch = p_text_in_buffer->current_char();

		do {
			if (char_map[ch] == CC_WHITE_SPACE) {
				ch = p_text_in_buffer->get_char();
			}
			else if (ch == L'/') {
				ch = p_text_in_buffer->get_char();
				if (ch == L'/') {
					while (ch != L'\0') ch = p_text_in_buffer->get_char();
				}
				else if (ch == L'*') {
					while (ch != EOF_CHAR) {
						ch = p_text_in_buffer->get_char();
						if (ch == L'*') {
							ch = p_text_in_buffer->get_char();
							if (ch == L'/') {
								ch = p_text_in_buffer->get_char();
								break;
							}
						}
					}
				}
				else {
					p_text_in_buffer->put_back_char();
					break;
				}
			}
		} while ((char_map[ch] == CC_WHITE_SPACE) || (ch == L'/'));
	}

	/** get         Extract the next__ token from the text input,
	 *              based on the current character.
	 *
	 * @return pointer to the extracted token.
	 */
	token *text_scanner::get(void) {
		token *p_token; // ptr to token to return

		skip_whitespace();

		char_code code;

		wchar_t ch = p_text_in_buffer->current_char();
		code = char_map[ch];

		// Determine the token class, based on the current character.
		switch (code) {
		case CC_LETTER: p_token = &word_token;
			break;
		case CC_DIGIT: p_token = &number_token;
			break;
		case CC_DOUBLE_QUOTE: p_token = &string_token;
			break;
		case CC_QUOTE: p_token = &char_token;
			break;
		case CC_SPECIAL: p_token = &special_token;
			break;
		case CC_END_OF_FILE: p_token = &eof_token;
			break;
		default: p_token = &error_token;
			break;
		}

		// Extract a token of that class, and return a pointer to it.
		p_token->get(*p_text_in_buffer);
		return p_token;
	}
}