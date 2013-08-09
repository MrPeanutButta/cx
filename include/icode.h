#ifndef icode_h
#define icode_h

#include <fstream>
#include "token.h"
#include "scanner.h"

const cx_token_code mc_line_marker = ((cx_token_code) 127);
const cx_token_code mc_location_marker = ((cx_token_code) 126);

class cx_symtab_node;

///  cx_icode      Intermediate code subclass of cx_scanner.

class cx_icode : public cx_scanner {

    enum {
        code_segment_size = 4096
    };

    char *p_code; // ptr to the code segment
    char *cursor; // ptr to current code location
    cx_symtab_node *p_node; // ptr to extracted symbol table node

    void check_bounds(int size);
    cx_symtab_node *get_symtab_node(void);

public:
    cx_icode(const cx_icode &icode); // copy constructor

    cx_icode(void) {
        p_code = cursor = new char[code_segment_size];
    }

    ~cx_icode(void) {
        if (p_code != nullptr) delete[] p_code;
    }

    // aappend to already existing icode
    void append(const cx_icode &icode);
    
    void put(cx_token_code tc);
    void put(const cx_symtab_node *p_node);
    void insert_line_marker(void);
    int put_location_marker(void);
    void fixup_location_marker(int location);
    int get_location_marker(void);
    void put_case_item(int value, int location);
    void get_case_item(int &value, int &location);

    void reset(void) {
        cursor = p_code;
    }

    void go_to(int location) {
        cursor = p_code + location;
    }

    int current_location(void) const {
        return cursor - p_code;
    }

    cx_symtab_node *symtab_node(void) const {
        return p_node;
    }

    virtual cx_token *get(void);
};

#endif
