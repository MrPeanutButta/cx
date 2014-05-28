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
#include <string>
#include <memory>
#include "misc.h"
#include "exec.h"

struct cx_stack_item;
class cx_type;
class cx_symtab_node;
class cx_executor;
class cx_runtime_stack;

typedef std::vector<cx_stack_item*> cx_stack;
typedef cx_stack::iterator cx_stack_iterator;
typedef cx_type *(*ext_call)(cx_runtime_stack *, cx_symtab_node *, const cx_type *);

extern bool xreference_flag;
extern int current_line_number;
extern int current_nesting_level;
extern int asm_label_index;

struct cx_stack_item;

class cx_symtab;
class cx_symtab_node;
class cx_line_num_list;
class cx_icode;
class cx_type;

// for public, private and protected scopes
///typedef std::map<cx_token_code, cx_symtab *> cx_scoped_symtab;

enum cx_define_code {
    dc_undefined, dc_constant, dc_type, dc_variable, dc_member,
    dc_value_parm, dc_reference, dc_pointer, dc_this,
    dc_program, dc_function, dc_namespace
};

enum cx_function_code {
    func_declared, func_forward, func_standard, func_std_member//, func_std_iterator
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
    bool is_this_ptr;
    
    struct {
        cx_symtab_node *p_node;
        cx_stack_item *p_stack_item;
    } this_ptr;

    union {

        struct {
            cx_data_value value;
        } constant;

        struct {
            cx_function_code which;

            int return_marker; // used for globals return location

            struct {
                int loop_start; // icode positions
                int loop_end;
                int current_iteration; // each indexer to determine which each call
                int postfix;
            } iterator;

            int parm_count;
            int total_parm_size;
            int total_local_size;

            ext_call ext_function; // extended function call

            cx_local_ids locals;
            cx_symtab *p_symtab;
            cx_icode *p_icode;
        } routine;

        struct {
            FILE *stream;
        } io;
    };

    cx_define(cx_define_code dc) {
        how = dc;
        io.stream = nullptr;
        this_ptr.p_stack_item = nullptr;
        is_this_ptr = false;
		routine.locals.p_function_ids = nullptr;
    }
    ~cx_define();
};

class cx_symtab_node {
    cx_symtab_node *left__, *right__;
    std::string node_name;

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

    cx_symtab_node *right_subtree(void) const {
        return right__;
    }

    const char *string__(void) const {
        return node_name.c_str();
    }

    short symtab_index(void) const {
        return xsymtab;
    }

    short node_index(void) const {
        return xnode;
    }

    void print(void) const;
    void print_identifier(void) const;
    void print_constant(void) const;
    void print_var_or_field(void) const;
    void print_type(void) const;
};

class cx_symtab {
    cx_symtab_node *root__;
    short nodes_count;
    cx_symtab *next__;

public:

    cx_symtab() : nodes_count(0) {
        extern cx_symtab *p_symtab_list;

        this->root__ = nullptr;
        this->next__ = p_symtab_list;

        p_symtab_list = this;
    }

    ~cx_symtab() {
        if (root__ != nullptr) delete root__;

        root__ = nullptr;
    }

    cx_symtab_node *search(const char *p_string) const;
    cx_symtab_node *enter(const char *p_string, cx_define_code dc = dc_undefined);
    cx_symtab_node *enter_new(const char *p_string, cx_define_code dc = dc_undefined);

    cx_symtab_node *root(void) const {
        return root__;
    }

    cx_symtab *next(void) const {
        return next__;
    }

    // DEPRECATED //

    cx_symtab_node **node_vector(void) const {
        //return p_vector_nodes;
        return nullptr;
    }

    int node_count(void)const {
        return nodes_count;
    }

    void print(void) const {
        root__->print();
    }

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

