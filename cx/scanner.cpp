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
		for (i = 'a'; i <= 'z'; ++i) char_map[i] = CC_LETTER;
		for (i = 'A'; i <= 'Z'; ++i) char_map[i] = CC_LETTER;
		for (i = '0'; i <= '9'; ++i) char_map[i] = CC_DIGIT;

		char_map['_'] = CC_LETTER;

		char_map['+'] = char_map['-'] = CC_SPECIAL;
		char_map['*'] = char_map['/'] = CC_SPECIAL;
		char_map['='] = char_map['^'] = CC_SPECIAL;
		char_map['.'] = char_map[','] = CC_SPECIAL;
		char_map['<'] = char_map['>'] = CC_SPECIAL;
		char_map['('] = char_map[')'] = CC_SPECIAL;
		char_map['['] = char_map[']'] = CC_SPECIAL;
		char_map['{'] = char_map['}'] = CC_SPECIAL;
		char_map[':'] = char_map[';'] = CC_SPECIAL;

		char_map['#'] = char_map['?'] = CC_SPECIAL;
		char_map['~'] = char_map['|'] = CC_SPECIAL;
		char_map['&'] = char_map['!'] = CC_SPECIAL;
		char_map['%'] = CC_SPECIAL;

		char_map[' '] = char_map['\t'] = CC_WHITE_SPACE;
		char_map['\n'] = char_map['\0'] = CC_WHITE_SPACE;
		char_map['\r'] = char_map['\f'] = CC_WHITE_SPACE;
		char_map['\\'] = CC_WHITE_SPACE;

		char_map['\''] = CC_QUOTE;
		char_map['\"'] = CC_DOUBLE_QUOTE;

		char_map[EOF_CHAR] = CC_END_OF_FILE;

		char_map['`'] = char_map['@'] = CC_ERROR;
	}

	/** skip_whitespace      Repeatedly fetch characters from the
	 *                      text input as long as they're
	 *                      whitespace. Each comment is a whitespace
	 *                      character.
	 *
	 */
	void text_scanner::skip_whitespace(void) {
		char ch = p_text_in_buffer->current_char();

		do {
			if (char_map[ch] == CC_WHITE_SPACE) {
				ch = p_text_in_buffer->get_char();
			}
			else if (ch == '/') {
				ch = p_text_in_buffer->get_char();
				if (ch == '/') {
					while (ch != '\0') ch = p_text_in_buffer->get_char();
				}
				else if (ch == '*') {
					while (ch != EOF_CHAR) {
						ch = p_text_in_buffer->get_char();
						if (ch == '*') {
							ch = p_text_in_buffer->get_char();
							if (ch == '/') {
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
		} while ((char_map[ch] == CC_WHITE_SPACE)|| (ch == '/'));
	}

	/** get         Extract the next__ token from the text input,
	 *              based on the current character.
	 *
	 * @return pointer to the extracted token.
	 */
	token *text_scanner::get(void) {
		token *p_token; // ptr to token to return

		skip_whitespace();

		// Determine the token class, based on the current character.
		switch (char_map[p_text_in_buffer->current_char()]) {
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