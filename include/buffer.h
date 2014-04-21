#ifndef buffer_h
#define buffer_h

#ifndef CX_DYNAMIC_API
#include <fstream>
#include <cstdio>
#endif
#include <cstring>
#include "misc.h"
#include "error.h"

/***********
 *         *
 *  Input  *
 *         *
 ***********/

extern const char eof_char;
extern int input_position;
extern int list_flag;
extern int level;

const int max_input_buffer_size = 1024;

///  cx_text_in_buffer       Abstract text input buffer class.

class cx_text_in_buffer {
protected:
    std::ifstream file; // input text file
    char *p_file_name; // ptr to the file name
    char text[max_input_buffer_size]; // input text buffer
    char *p_char; /* ptr to the current char
                   * in the text buffer */

    virtual char get_line(void) = 0;

public:
    cx_text_in_buffer(const char *p_input_file_name, cx_abort_code ac);

    virtual ~cx_text_in_buffer(void) {
        file.close();
        delete p_file_name;
    }

    const char *file_name(void) {
        return p_file_name;
    }

    char current_char(void) const {
        return *p_char;
    }
    char get_char(void);
    char put_back_char(void);
};


///  cx_source_buffer       Source buffer subclass of cx_text_in_buffer.

class cx_source_buffer : public cx_text_in_buffer {
    virtual char get_line(void);

public:
    cx_source_buffer(const char *p_source_file_name);
};

/************
 *          *
 *  Output  *
 *          *
 ************/


///  cx_text_out_buffer      Abstract text output buffer class.

class cx_text_out_buffer {
public:

    virtual ~cx_text_out_buffer() = 0;

    char text[max_input_buffer_size + 16]; // output text buffer

    virtual void put_line(void) = 0;

    void put_line(const char *p_text) {
        strcpy(text, p_text);
        put_line();
    }
};


///  cx_list_buffer         List buffer subclass of cx_text_out_buffer.

class cx_list_buffer : public cx_text_out_buffer {
    std::string source_file_name; // ptr to source file name (for page header)
    int line_count; // count of lines in the current page
public:

    virtual ~cx_list_buffer(void) {
    }

    void initialize(const char *p_file_name);
    virtual void put_line(void);

    void put_line(const char *p_text) {
        cx_text_out_buffer::put_line(p_text);
    }

    void buffer(const char *p_text, int line_number, int nesting_level) {
        sprintf(text, "%4d %d: %s", line_number, nesting_level, p_text);
    }

    void put_line(const char *p_text, int line_number, int nesting_level) {
        sprintf(text, "%4d %d: %s", line_number, nesting_level, p_text);
        put_line();
    }
};

extern cx_list_buffer list;

#endif