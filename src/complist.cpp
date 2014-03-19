/** Compact Source Listing
 * complist.cpp
 *
 * NOTE:
 *      this module may be deprecated. -aaron
 *
 */

#include <cstring>
#include <iostream>
#include "complist.h"

void cx_compact_list_buffer::put_blank(void) {
    if (++text_length >= max_compact_text_length - 1) put_line();
    else *p_text++ = ' ';
}

void cx_compact_list_buffer::put(const char* p_string) {

    int token_length(strlen(p_string));

    if (text_length + token_length >= max_compact_text_length - 1) {
        put_line();
    }

    strcpy(p_text, p_string);
    p_text += token_length;
    text_length += token_length;

}

void cx_compact_list_buffer::put_line(void) {
    if (text_length > 0) {
        *p_text = '\0';
        std::cout << text << std::endl;

        p_text = text;
        text_length = 0;
    }
}

