/*
 * File:   symtable.h
 * Author: aaro3965
 *
 * Created on June 19, 2013, 12:17 AM
 */

#ifndef symtable_h
#define	symtable_h

#include <map>
#include <cstring>
#include "misc.h"



extern bool xreference_flag;
extern int current_line_number;
extern int current_nesting_level;
extern int asm_label_index;

union cx_stack_item;

class cx_symtab;
class cx_symtab_node;
class cx_line_num_list;
class cx_icode;
class cx_type;

// for public, private and protected scopes
///typedef std::map<cx_token_code, cx_symtab *> cx_scoped_symtab;

enum cx_define_code {
    dc_undefined, dc_constant, dc_type, dc_variable, dc_member,
    dc_value_parm, dc_reference, dc_pointer,
    dc_program, dc_function
};

enum cx_routine_code {
    rc_declared, rc_forward,
};

struct cx_local_ids {
    cx_symtab_node *p_parms_ids;
    cx_symtab_node *p_constant_ids;
    cx_symtab_node *p_type_ids;
    cx_symtab_node *p_variable_ids;
    cx_symtab_node *p_function_ids;
};

class cx_define {
public:

    cx_define_code how;

    union {

        struct {
            cx_data_value value;
        } constant;

        struct {
            cx_routine_code which;
            int return_marker;              // used for globals return location
            int parm_count;
            int total_parm_size;
            int total_local_size;
            cx_local_ids locals;
            cx_symtab *p_symtab;
            cx_icode *p_icode;
        } routine;

        struct {
            int offset;
        } data;
    };

    cx_define(cx_define_code dc) {
        how = dc;
    }
    ~cx_define();
};

class cx_symtab_node {
    cx_symtab_node *left__, *right__;
    char *p_string;
    short xsymtab;
    short xnode;
    cx_line_num_list *p_line_num_list;

    friend class cx_symtab;

public:

    cx_symtab_node *next__;
    cx_symtab_node *prev;

    cx_type *p_type;

    cx_define defn;
    int level;
    int label_index;
    int global_finish_location;
    int string_length;
    bool found_global_end;

    // pointer to runstack item
    cx_stack_item *runstack_item;

    cx_symtab_node(const char *p_string, cx_define_code dc = dc_undefined);
    ~cx_symtab_node();

    cx_symtab_node *left_subtree(void) const {
        return left__;
    }

    cx_symtab_node *right___subtree(void) const {
        return right__;
    }

    char *string__(void) const {
        return p_string;
    }

    /*void rename_node(const char *p_string) {
        if (p_string != nullptr) {
            delete p_string;
            p_string = nullptr;
        }

        p_string = new char[strlen(p_string)];
        strcpy(p_string, p_string);
    }*/

    short symtab_index(void) const {
        return xsymtab;
    }

    short node_index(void) const {
        return xnode;
    }

    void convert(cx_symtab_node *p_vector_nodes[]);

    void print(void) const;
    void print_identifier(void) const;
    void print_constant(void) const;
    void print_var_or_field(void) const;
    void print_type(void) const;
};

class cx_symtab {
    cx_symtab_node *root__;
    cx_symtab_node **p_vector_nodes;
    short nodes_count;
    short xsymtab;
    cx_symtab *next__;

public:

    cx_symtab() : nodes_count(0), xsymtab(0) {
        extern int symtab_count;
        extern cx_symtab *p_symtab_list;

        root__ = nullptr;
        p_vector_nodes = nullptr;
        xsymtab = symtab_count++;

        next__ = p_symtab_list;
        p_symtab_list = this;
    }

    ~cx_symtab() {
        if (root__ != nullptr) delete root__;
        if (p_vector_nodes != nullptr) delete [] p_vector_nodes;
    }

    cx_symtab_node *search(const char *p_string) const;
    cx_symtab_node *enter(const char *p_string, cx_define_code dc = dc_undefined);
    cx_symtab_node *enter_new(const char *p_string, cx_define_code dc = dc_undefined);

    cx_symtab_node *root(void) const {
        return root__;
    }

/*    void connect_tables(cx_scoped_symtab &class_symtab) {

        root__ = class_symtab[tc_PUBLIC]->root__;
        root__->left__ = class_symtab[tc_PROTECTED]->root__;
        root__->right__ = class_symtab[tc_PRIVATE]->root__;
    }*/

    cx_symtab_node *get(short xnode) const {
        if (p_vector_nodes == nullptr) return nullptr;

        return p_vector_nodes[xnode];
    }

    cx_symtab *next(void) const {
        return next__;
    }

    cx_symtab_node **node_vector(void) const {
        return p_vector_nodes;
    }

    int node_count(void)const {
        return nodes_count;
    }

    void print(void) const {
        root__->print();
    }

    void convert(cx_symtab *p_vector_symtabs[]);

};

class cx_line_num_node {
    cx_line_num_node *next__;
    const int number;

    friend class cx_line_num_list;

public:

    cx_line_num_node() :
    number(current_line_number) {
        next__ = nullptr;
    }
};

class cx_line_num_list {
    cx_line_num_node *head, *tail;

public:

    cx_line_num_list() :
    head(new cx_line_num_node), tail(new cx_line_num_node) {
    }

    virtual ~cx_line_num_list();

    void update(void);
    void print(int new_line_flag, int indent) const;
};

///  cx_symtab_stack      Symbol table stack class.

class cx_symtab_stack {

    enum {
        max_nesting_level = 8
    };

    cx_symtab *p_symtabs[max_nesting_level]; // stack of symbol table ptrs

    //void InitializeMain(void);

public:
    cx_symtab_stack(void);
    ~cx_symtab_stack(void);

    cx_symtab_node *search_local(const char *p_string) {
        return p_symtabs[current_nesting_level]->search(p_string);
    }

    cx_symtab_node *enter_local(const char *p_string,
            cx_define_code dc = dc_undefined) {
        return p_symtabs[current_nesting_level]->enter(p_string, dc);
    }

    cx_symtab_node *enter_new_local(const char *p_string,
            cx_define_code dc = dc_undefined) {
        return p_symtabs[current_nesting_level]->enter_new(p_string, dc);
    }

    cx_symtab *get_current_symtab(void) const {
        return p_symtabs[current_nesting_level];
    }

    void set_current_symtab(cx_symtab *p_symtab) {
        p_symtabs[current_nesting_level] = p_symtab;
    }

    void set_scope(int scopeLevel) {
        current_nesting_level = scopeLevel;
    }

    cx_symtab_node *search_available_scopes(const char *p_string) const;
    cx_symtab_node *search_all(const char *p_string) const;
    cx_symtab_node *find(const char *p_string) const;
    void enter_scope(void);
    cx_symtab *exit_scope(void);
};

#endif	/* SYMTABLE_H */

