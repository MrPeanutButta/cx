#include <cstdio>
#include <cstdint>
#include "token.h"

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
char cx_token::get_escape_char(const char& c) {
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
            cx_error(err_invalid_escape_char);
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
void cx_string_token::get(cx_text_in_buffer &buffer) {
    char ch; // current character
    char *ps = string; // ptr to char in string

    *ps++ = '\"'; // opening quote

    // get the string.
    ch = buffer.get_char(); // first char after opening quote
    while (ch != eof_char) {
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
        } else {
            // Append current char to string, then get the next__ char.
            *ps++ = ch;
        }

        ch = buffer.get_char();
    }

    if (ch == eof_char) cx_error(err_unexpected_eof);

    *ps++ = '\"'; // closing quote
    *ps = '\0';
}

/** get         Extract single quoted char ' '
 *
 * @param buffer : ptr to text input buffer.
 */
void cx_char_token::get(cx_text_in_buffer &buffer) {
    char ch; // current character
    char *ps = string; // ptr to char in string

    *ps++ = '\''; // opening quote
    // get the string.
    ch = buffer.get_char(); // first char after opening quote

    if (ch == '\\') {
        *ps++ = get_escape_char(buffer.get_char());
    } else {
        *ps++ = ch;
    }

    // Append current char to string, then get the next__ char.
    ch = buffer.get_char();

    if (ch != '\'') cx_error(err_missing_single_quote);
    if (ch == eof_char) cx_error(err_unexpected_eof);

    ch = buffer.get_char();
    *ps++ = '\''; // closing quote
    *ps = '\0';
}

void cx_char_token::print(void) const {
    sprintf(list.text, "\t%-18s %-s", ">> char:", string);
    list.put_line();
}

/** print       print the token to the list file.
 *
 */
void cx_string_token::print(void) const {
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
void cx_special_token::get(cx_text_in_buffer &buffer) {
    char ch = buffer.current_char();
    char *ps = string;

    *ps++ = ch;

    switch (ch) {
        case '^': ch = buffer.get_char();
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_bit_XOR_equal;
                buffer.get_char();
            } else code__ = tc_bit_XOR;
            break;
        case '&': ch = buffer.get_char();
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_bit_AND_equal;
                buffer.get_char();
            } else if (ch == '&') {
                *ps++ = '&';
                code__ = tc_logic_AND;
                buffer.get_char();
            } else code__ = tc_bit_AND;
            break;
        case '|': ch = buffer.get_char();
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_bit_OR_equal;
                buffer.get_char();
            } else if (ch == '|') {
                *ps++ = '|';
                code__ = tc_logic_OR;
                buffer.get_char();
            } else code__ = tc_bit_OR;
            break;
        case '~': ch = buffer.get_char();
            code__ = tc_bit_NOT;
            break;
        case '*': ch = buffer.get_char();
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_star_equal;
                buffer.get_char();
            } else code__ = tc_star;
            break;
        case '(': code__ = tc_left_paren;
            buffer.get_char();
            break;
        case ')': code__ = tc_right_paren;
            buffer.get_char();
            break;
        case '-': ch = buffer.get_char();
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_minus_equal;
                buffer.get_char();
            } else if (ch == '-') {
                *ps++ = '-';
                code__ = tc_minus_minus;
                buffer.get_char();
            } else if (ch == '>') {
                *ps++ = '>';
                code__ = tc_pointer_member;
                buffer.get_char();
            } else code__ = tc_minus;
            break;
        case '+': ch = buffer.get_char();
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_plus_equal;
                buffer.get_char();
            } else if (ch == '+') {
                *ps++ = '+';
                code__ = tc_plus_plus;
                buffer.get_char();
            } else code__ = tc_plus;
            break;
        case '=': ch = buffer.get_char();
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_equal_equal;
                buffer.get_char();
            } else code__ = tc_equal;
            break;
        case '[': code__ = tc_left_subscript;
            buffer.get_char();
            break;
        case ']': code__ = tc_right_subscript;
            buffer.get_char();
            break;
        case '{': code__ = tc_left_bracket;
            buffer.get_char();
            break;
        case '}': code__ = tc_right_bracket;
            buffer.get_char();
            break;
        case ';': code__ = tc_semicolon;
            buffer.get_char();
            break;
        case ',': code__ = tc_comma;
            buffer.get_char();
            break;
        case '/': ch = buffer.get_char(); // /, /=
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_divide_equal;
                buffer.get_char();
            } else code__ = tc_divide;
            break;
        case ':': ch = buffer.get_char(); // : or ::
            if (ch == ':') {
                *ps++ = ':';
                code__ = tc_colon_colon;
                buffer.get_char();
            } else code__ = tc_colon;
            break;
        case '<': ch = buffer.get_char(); // < or <=
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_lessthan_equal;
                buffer.get_char();
            } else if (ch == '<') {
                *ps++ = '<';
                ch = buffer.get_char();
                if (ch == '=') {
                    *ps++ = '=';
                    code__ = tc_bit_leftshift_equal;
                    buffer.get_char();
                } else code__ = tc_bit_leftshift;
            } else code__ = tc_lessthan;
            break;
        case '>': ch = buffer.get_char(); // > or >=
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_greaterthan_equal;
                buffer.get_char();
            } else if (ch == '>') {
                *ps++ = '>';
                ch = buffer.get_char();
                if (ch == '=') {
                    *ps++ = '=';
                    code__ = tc_bit_rightshift_equal;
                    buffer.get_char();
                } else code__ = tc_bit_rightshift;
            } else code__ = tc_greaterthan;
            break;
        case '!': ch = buffer.get_char(); // ! or !=
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_not_equal;
                buffer.get_char();
            } else code__ = tc_logic_NOT;
            break;
        case '%': ch = buffer.get_char(); // ! or !=
            if (ch == '=') {
                *ps++ = '=';
                code__ = tc_modulas_equal;
                buffer.get_char();
            } else code__ = tc_modulas;
            break;
        case '.': ch = buffer.get_char();
            if (ch == '.') {
                *ps++ = '.';
                code__ = tc_dot_dot;
                buffer.get_char();
            } else code__ = tc_dot;
            break;
        case '?': code__ = tc_questionmark;
            buffer.get_char();
            break;
        case '#': code__ = tc_pound;
            buffer.get_char();
            break;
        case '\"': code__ = tc_double_quote;
            buffer.get_char();
        default: code__ = tc_error; // error
            buffer.get_char();
            cx_error(err_unrecognizable);
            break;
    }

    *ps = '\0';
}

/** print       print the token to the list file.
 *
 */
void cx_special_token::print(void) const {
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
void cx_error_token::get(cx_text_in_buffer &buffer) {
    string[0] = buffer.current_char();
    string[1] = '\0';

    buffer.get_char();
    cx_error(err_unrecognizable);
}
