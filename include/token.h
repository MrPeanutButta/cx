#ifndef token_h
#define token_h

#include <cstdint>
#include "misc.h"
#include "error.h"
#include "buffer.h"

extern cx_char_code_map char_code_map;

///  cx_token              Abstract token class.
class cx_icode;

class cx_token {
protected:

    friend cx_icode;

    cx_token_code code__;
    cx_data_type type__;
    cx_data_value value__;
    char string[max_input_buffer_size];

public:

    cx_token(void) {
        code__ = tc_dummy;
        type__ = ty_dummy;
        value__.int__ = 0;
        string[0] = '\0';
    }

    cx_token_code code() const {
        return code__;
    }

    cx_data_type type() const {
        return type__;
    }

    cx_data_value value() const {
        return value__;
    }

    char *string__() {
        return string;
    }

    char get_escape_char(const char &c);
    virtual void get(cx_text_in_buffer &buffer) = 0;
    virtual int is_delimiter(void) const = 0;
    virtual void print(void) const = 0;
};

///  cx_word_token          Word token subclass of cx_token.

class cx_word_token : public cx_token {
    void check_for_reserved_word(void);

public:
    virtual void get(cx_text_in_buffer &buffer);

    virtual int is_delimiter(void) const {
        return false;
    }
    virtual void print(void) const;
};

//  cx_number_token        Number token subclass of cx_token.

class cx_number_token : public cx_token {
    char ch; // char fetched from input buffer
    char *ps; // ptr into token string
    int digit_count; // total no. of digits in number
    bool count_error_flag; // true if too many digits, else false

    uint8_t radix; // number base

    int accumulate_value(cx_text_in_buffer &buffer,
            float &value, cx_error_code ec);

    bool is_x_digit(const char &c);
    int char_value(const char &c);

public:

    cx_number_token() {
        code__ = tc_number;
    }

    virtual void get(cx_text_in_buffer &buffer);

    virtual int is_delimiter(void) const {
        return false;
    }
    virtual void print(void) const;
};

///  cx_string_token        string__ token subclass of cx_token.

class cx_string_token : public cx_token {
public:

    cx_string_token() {
        code__ = tc_string;
    }

    virtual void get(cx_text_in_buffer &buffer);

    virtual int is_delimiter(void) const {
        return true;
    }
    virtual void print(void) const;
};

class cx_char_token : public cx_token {
public:

    cx_char_token() {
        code__ = tc_char;
    }

    virtual void get(cx_text_in_buffer &buffer);

    virtual int is_delimiter(void) const {
        return true;
    }
    virtual void print(void) const;
};

///  cx_special_token       Special token subclass of cx_token.

class cx_special_token : public cx_token {
public:
    virtual void get(cx_text_in_buffer &buffer);

    virtual int is_delimiter(void) const {
        return true;
    }
    virtual void print(void) const;
};

///  cx_EOF_token           End-of-file token subclass of cx_token.

class cx_EOF_token : public cx_token {
public:

    cx_EOF_token() {
        code__ = tc_end_of_file;
    }

    virtual void get(cx_text_in_buffer &buffer) {
    }

    virtual int is_delimiter(void) const {
        return false;
    }

    virtual void print(void) const {
    }
};

///  cx_error_token         cx_error token subclass of cx_token.

class cx_error_token : public cx_token {
public:

    cx_error_token() {
        code__ = tc_error;
    }

    virtual void get(cx_text_in_buffer &buffer);

    virtual int is_delimiter(void) const {
        return false;
    }

    virtual void print(void) const {
    }
};

#endif
