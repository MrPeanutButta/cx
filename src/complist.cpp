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

using namespace std;

void cx_compact_list_buffer::put_blank(void) {
    if (++text_length >= max_compact_text_length - 1) put_line();
    else *p_text++ = ' ';
}

void cx_compact_list_buffer::put(const char* p_string) {

    int tokenLength(strlen(p_string));

    if (text_length + tokenLength >= max_compact_text_length - 1) {
        put_line();
    }

    strcpy(p_text, p_string);
    p_text += tokenLength;
    text_length += tokenLength;

}

void cx_compact_list_buffer::put_line(void) {
    if (text_length > 0) {
        *p_text = '\0';
        cout << text << endl;

        p_text = text;
        text_length = 0;
    }
}

