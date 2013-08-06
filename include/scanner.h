#ifndef scanner_h
#define scanner_h

#include "misc.h"
#include "buffer.h"
#include "token.h"

///  cx_scanner            Abstract scanner class.

class cx_scanner {
protected:

    // Tokens extracted and returned by the scanner.
    cx_word_token word_token;
    cx_number_token number_token;
    cx_string_token string_token;
    cx_char_token char_token;
    cx_special_token special_token;
    cx_EOF_token eof_token;
    cx_error_token error_token;

public:

    virtual ~cx_scanner(void) {
    }

    virtual cx_token *get(void) = 0;
};

///  cx_text_scanner        Text scanner subclass of cx_scanner.

class cx_text_scanner : public cx_scanner {
    cx_text_in_buffer * const p_text_in_buffer; /* ptr to input text buffer
                                                 * to scan */

    void skip_whitespace(void);

public:
    cx_text_scanner(cx_text_in_buffer *p_buffer);

    virtual ~cx_text_scanner(void) {
        delete p_text_in_buffer;
    }

    virtual cx_token *get(void);
};

#endif
