#include <map>
#include "scanner.h"
#include "misc.h"

cx_char_code_map char_code_map; // maps a character to its code

/** Constructor     Construct a scanner by constructing the
 *                  text input file buffer and initializing the
 *                  character code map.
 *
 * @param p_buffer : ptr to text input buffer to scan.
 */
cx_text_scanner::cx_text_scanner(cx_text_in_buffer *p_buffer)
: p_text_in_buffer(p_buffer) {
    char i;

    // Initialize the character code map.
    for (i = 'a'; i <= 'z'; ++i) char_code_map[i] = cc_letter;
    for (i = 'A'; i <= 'Z'; ++i) char_code_map[i] = cc_letter;
    for (i = '0'; i <= '9'; ++i) char_code_map[i] = cc_digit;

    char_code_map['_' ] = cc_letter;

    char_code_map['+' ] = char_code_map['-' ] = cc_special;
    char_code_map['*' ] = char_code_map['/' ] = cc_special;
    char_code_map['=' ] = char_code_map['^' ] = cc_special;
    char_code_map['.' ] = char_code_map[',' ] = cc_special;
    char_code_map['<' ] = char_code_map['>' ] = cc_special;
    char_code_map['(' ] = char_code_map[')' ] = cc_special;
    char_code_map['[' ] = char_code_map[']' ] = cc_special;
    char_code_map['{' ] = char_code_map['}' ] = cc_special;
    char_code_map[':' ] = char_code_map[';' ] = cc_special;

    char_code_map['#' ] = char_code_map['?' ] = cc_special;
    char_code_map['~' ] = char_code_map['|' ] = cc_special;
    char_code_map['&' ] = char_code_map['!' ] = cc_special;
    char_code_map['%' ] = cc_special;

    char_code_map[' ' ] = char_code_map['\t'] = cc_white_space;
    char_code_map['\n'] = char_code_map['\0'] = cc_white_space;
    char_code_map['\r'] = char_code_map['\f'] = cc_white_space;
    char_code_map['\\'] = cc_white_space;

    char_code_map['\''] = cc_quote;
    char_code_map['\"'] = cc_double_quote;

    char_code_map[eof_char] = cc_end_of_file;

    char_code_map['`'] = char_code_map['@'] = cc_error;
}

/** skip_whitespace      Repeatedly fetch characters from the
 *                      text input as long as they're
 *                      whitespace. Each comment is a whitespace
 *                      character.
 *
 */
void cx_text_scanner::skip_whitespace(void) {
    char ch = p_text_in_buffer->current_char();

    do {
        if (char_code_map[ch] == cc_white_space) {
            ch = p_text_in_buffer->get_char();
        } else if (ch == '/') {
            ch = p_text_in_buffer->get_char();
            if (ch == '/') {
                while (ch != '\0') ch = p_text_in_buffer->get_char();
            } else if (ch == '*') {
                while (ch != eof_char) {
                    ch = p_text_in_buffer->get_char();
                    if (ch == '*') {
                        ch = p_text_in_buffer->get_char();
                        if (ch == '/') {
                            ch = p_text_in_buffer->get_char();
                            break;
                        }
                    }
                }
            } else {
                p_text_in_buffer->put_back_char();
                break;
            }
        }
    } while ((char_code_map[ch] == cc_white_space)
            || (ch == '/'));
}

/** get         Extract the next__ token from the text input,
 *              based on the current character.
 *
 * @return pointer to the extracted token.
 */
cx_token * cx_text_scanner::get(void) {
    cx_token *p_token; // ptr to token to return

    skip_whitespace();

    // Determine the token class, based on the current character.
    switch (char_code_map[p_text_in_buffer->current_char()]) {
    case cc_letter: p_token = &word_token;
        break;
    case cc_digit: p_token = &number_token;
        break;
    case cc_double_quote: p_token = &string_token;
        break;
    case cc_quote: p_token = &char_token;
        break;
    case cc_special: p_token = &special_token;
        break;
    case cc_end_of_file: p_token = &eof_token;
        break;
    default: p_token = &error_token;
        break;
    }

    // Extract a token of that class, and return a pointer to it.
    p_token->get(*p_text_in_buffer);
    return p_token;
}

