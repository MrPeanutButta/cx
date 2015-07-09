#ifndef BACKEND_H
#define BACKEND_H

#include "misc.h"
#include "symtable.h"
#include "token.h"
#include "icode.h"

/// cx_backend            Abstract back end class.

class cx_backend {
protected:
    cx_token *p_token; // ptr to the current token
    cx_token_code token; // code of current token
    cx_icode *p_icode; // ptr to current icode
    cx_symtab_node *p_node; // ptr to symtab node

    void get_token(void) {
        p_token = p_icode->get();
        token = p_token->code();
        p_node = p_icode->symtab_node();
    }

    void go_to(int location) {
        p_icode->go_to(location);
    }

    int current_location(void) const {
        return p_icode->current_location();
    }

    int get_location_marker(void) {
        return p_icode->get_location_marker();
    }

    void get_case_item(int &value, int &location) {
        p_icode->get_case_item(value, location);
    }

public:

    virtual ~cx_backend(void) {
    }

    virtual void go(cx_symtab_node *p_function_id) = 0;
};

#endif

