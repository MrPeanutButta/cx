/*
 * File:   complist.h
 * Author: aaron
 *
 * Created on June 13, 2013, 11:31 PM
 */

#ifndef complist_h
#define	complist_h

#include "buffer.h"

const int max_compact_text_length = 72;

class cx_compact_list_buffer : public cx_text_out_buffer {
private:
    int text_length;
    char *p_text;

public:

    cx_compact_list_buffer() :
    text_length(0) {
        p_text = text;
        *p_text = '\0';
    }

    ~cx_compact_list_buffer() {

    };

    void put_blank(void);
    void put(const char *p_string);
    void put_line(void);

};

#endif	/* COMPLIST_H */

