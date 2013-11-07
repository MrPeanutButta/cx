#ifndef exec_h
#define exec_h

#include <vector>
#include <algorithm>
#include <cstdint>
#include <iostream>
#include "error.h"
#include "symtable.h"
#include "types.h"
#include "icode.h"
#include "backend.h"

class cx_type;
extern bool cx_dev_debug_flag;

union mem_block {
    // basic types

    // unsigned
    uint8_t uint8__;
    uint16_t uint16__;
    uint32_t uint32__;
    uint64_t uint64__;

    bool bool__;

    // signed
    int int__;
    float float__;

    char char__;
    wchar_t wchar__;

    // pointer, reference, record or array
    void *addr__;
};

///  cx_stack_item          Item pushed onto the runtime stack.

struct cx_stack_item {

    cx_stack_item() {
    }

    ~cx_stack_item() {
    }

    /* overloaded ctors makes it easier
     * to perform a push */
    cx_stack_item(const uint8_t &value) {
        basic_types.uint8__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new uint8 = " << basic_types.uint8__ << std::endl;
    }

    cx_stack_item(const uint16_t &value) {
        basic_types.uint16__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new uint16 = " << basic_types.uint16__ << std::endl;
    }

    cx_stack_item(const uint32_t &value) {
        basic_types.uint32__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new uint32 = " << basic_types.uint32__ << std::endl;
    }

    cx_stack_item(const uint64_t &value) {
        basic_types.uint64__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new uint64 = " << basic_types.uint64__ << std::endl;
    }

    cx_stack_item(const bool &value) {
        basic_types.bool__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new bool = " << basic_types.bool__ << std::endl;
    }

    cx_stack_item(const int &value) {
        basic_types.int__ = value;

        if (cx_dev_debug_flag) {
            if (value == -1) {
                std::clog << "* return value place holder\n";
            } else {
                std::clog << "new int = " << basic_types.int__ << std::endl;
            }
        }
    }

    cx_stack_item(const float &value) {
        basic_types.float__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new float = " << basic_types.float__ << std::endl;
    }

    cx_stack_item(const char &value) {
        basic_types.char__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new char = " << basic_types.char__ << std::endl;
    }

    cx_stack_item(const wchar_t &value) {
        basic_types.wchar__ = value;

        if (cx_dev_debug_flag)
            std::clog << "new wchar = " << basic_types.wchar__ << std::endl;
    }

    cx_stack_item(void *address) {
        basic_types.addr__ = address;

        if (cx_dev_debug_flag)
            std::clog << "new ptr = " << basic_types.addr__ << std::endl;
    }

    mem_block basic_types;

};

typedef std::vector<cx_stack_item *> cx_stack;
typedef cx_stack::iterator cx_stack_iterator;

// Stack frame header

struct cx_frame_header : public cx_stack_item {

    cx_frame_header(const cx_frame_header *orig) {

        function_value = orig->function_value;
        static_link = orig->static_link;
        dynamic_link = orig->dynamic_link;

        return_address.icode = orig->return_address.icode;
        return_address.location = orig->return_address.location;

        if (cx_dev_debug_flag)
            std::clog << ">>* new (copy) stack frame element :" << this << std::endl;
    }

    cx_frame_header(void) {

        function_value = nullptr;
        static_link = new cx_stack_item;
        dynamic_link = new cx_stack_item;

        return_address.icode = new cx_stack_item;
        return_address.location = new cx_stack_item;

        if (cx_dev_debug_flag)
            std::clog << ">>* new (empty) stack frame element:" << this << std::endl;
    }

    cx_stack_item *function_value;
    cx_stack_item *static_link;
    cx_stack_item *dynamic_link;

    struct {
        cx_stack_item *icode;
        cx_stack_item *location;
        cx_stack_iterator previous_header;
    } return_address;

    // index of frame header
    int frame_header_index;
};

///  cx_runtime_stack       Runtime stack class.

class cx_runtime_stack {
    cx_stack cx_runstack;

    const cx_frame_header *p_stackbase;
    cx_frame_header *p_frame_base; // ptr to current stack frame base

    cx_stack_iterator it_frame_base; // iterator to frame base

public:
    cx_runtime_stack(void);

    void push(const bool &value) {
        cx_runstack.push_back(new cx_stack_item((bool)value));
    }

    void push(const uint8_t &value) {
        cx_runstack.push_back(new cx_stack_item((uint8_t) value));
    }

    void push(const uint16_t &value) {
        cx_runstack.push_back(new cx_stack_item((uint16_t) value));
    }

    void push(const uint32_t &value) {
        cx_runstack.push_back(new cx_stack_item((uint32_t) value));
    }

    void push(const uint64_t &value) {
        cx_runstack.push_back(new cx_stack_item((uint64_t) value));
    }

    void push(const wchar_t &value) {
        cx_runstack.push_back(new cx_stack_item((wchar_t) value));
    }

    void push(const int &value) {
        cx_runstack.push_back(new cx_stack_item((int) value));
    }

    void push(const float &value) {
        cx_runstack.push_back(new cx_stack_item((float) value));
    }

    void push(const char &value) {
        cx_runstack.push_back(new cx_stack_item((char) value));
    }

    void push(void *addr) {
        cx_runstack.push_back(new cx_stack_item(addr));
    }

    void push_frame(void) {
        cx_runstack.push_back(new cx_frame_header);

        cx_frame_header *prev = (cx_frame_header *) top();

        prev->return_address.previous_header = it_frame_base;
        it_frame_base = iterator_top();
    }

    cx_frame_header *push_frame_header(int old_level, int new_level,
            cx_icode *p_icode);
    void activate_frame(cx_frame_header *p_new_frame_base, const int &location);
    void pop_frame(const cx_symtab_node *p_function_id, cx_icode *&p_icode);

    void pop(void) {
        cx_runstack.pop_back();
    }

    void pop(const cx_stack_iterator &item) {

        if (std::find(it_frame_base,
                cx_runstack.end(),
                *item) != cx_runstack.end()) {

            cx_runstack.erase(item);
        } else {

            if (cx_dev_debug_flag)
                std::clog << "item not found on the stack\n";

        }
    }

    cx_stack_item *top(void) const {
        return cx_runstack.back();
    }

    const cx_stack_iterator iterator_top(void) {
        return (cx_runstack.end() - 1);
    }

    void allocate_value(cx_symtab_node *p_id);
    void deallocate_value(cx_symtab_node *p_id);

    cx_stack_item *get_value_address(const cx_symtab_node *p_id);
};

//  cx_executor           Executor subclass of cx_backend.

class cx_executor : public cx_backend {
    long statement_count; // count of executed statements
    cx_runtime_stack run_stack; // ptr to runtime stack

    bool eof_flag; // true if at end of file, else false

    bool break_loop; // if true, breaks current loop

    // Trace flags
    bool trace_routine_flag; // true to trace routine entry/exit
    bool trace_statement_flag; // true to trace statements
    bool trace_store_flag; // true to trace data stores
    bool trace_fetch_flag; // true to trace data fetches

    // Routines
    void initialize_global(cx_symtab_node *p_program_id);
    void execute_routine(cx_symtab_node *p_function_id);
    void enter_routine(cx_symtab_node *p_function_id);
    void exit_routine(cx_symtab_node *p_function_id);
    cx_type *execute_subroutine_call(cx_symtab_node *p_function_id);
    cx_type *execute_declared_subroutine_call(cx_symtab_node *p_function_id);
    cx_type *execute_standard_subroutine_call(cx_symtab_node *p_function_id);
    void execute_actual_parameters(cx_symtab_node *p_function_id);

    // Statements
    cx_symtab_node *enter_new(cx_symtab_node *p_function_id,
            const char *p_string);
    cx_symtab_node *allocate_new_node(cx_symtab_node *p_function_id);
    void execute_statement(cx_symtab_node *p_function_id);
    void execute_statement_list(cx_symtab_node *p_function_id,
            cx_token_code terminator);
    void execute_assignment(const cx_symtab_node *p_target_id);

    void assign(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target,
            void* &p_target_address);

    void plus_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target,
            void* &p_target_address);
    void minus_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);
    void star_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);
    void divide_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);
    void modulas_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);
    void leftshift_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);
    void rightshift_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);
    void and_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);
    void xor_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);


    void plus_plus(cx_type* p_target_type,
            cx_stack_item* p_target);
    void minus_minus(cx_type* p_target_type,
            cx_stack_item* p_target);

    void execute_DO(cx_symtab_node *p_function_id);
    void execute_WHILE(cx_symtab_node *p_function_id);
    void execute_IF(cx_symtab_node *p_function_id);
    void execute_FOR(cx_symtab_node *p_function_id);
    void execute_SWITCH(cx_symtab_node *p_function_id);
    void execute_RETURN(cx_symtab_node *p_function_id);
    void execute_compound(cx_symtab_node *p_function_id);

    // Expressions
    cx_type *execute_expression(void);
    cx_type *execute_simple_expression(void);
    cx_type *execute_term(void);
    cx_type *execute_factor(void);
    cx_type *execute_constant(const cx_symtab_node *p_id);
    cx_type *execute_variable(const cx_symtab_node *p_id, bool address_flag);
    cx_type *execute_subscripts(const cx_type *p_type);
    cx_type *execute_field(void);

    // Tracing
    void trace_routine_entry(const cx_symtab_node *p_function_id);
    void trace_routine_exit(const cx_symtab_node *p_function_id);
    void trace_statement(void);
    void trace_data_store(const cx_symtab_node *p_target_id,
            const cx_stack_item &p_data_value,
            const cx_type *p_data_type);
    void trace_data_fetch(const cx_symtab_node *p_id,
            const cx_stack_item &p_data_value,
            const cx_type *p_data_type);
    void trace_data_value(const cx_stack_item &p_data_value,
            const cx_type *p_data_type);

    void range_check(const cx_type *p_target_type, int value);

    void push(const uint8_t &value) {
        run_stack.push(value);
    }

    void push(const int &value) {
        run_stack.push(value);
    }

    void push(const float value) {
        run_stack.push(value);
    }

    void push(const char &value) {
        run_stack.push(value);
    }

    void push(void *addr) {
        run_stack.push(addr);
    }

    void pop(void) {
        run_stack.pop();
    }

    cx_stack_item *top(void) const {
        return run_stack.top();
    }

    const cx_stack_iterator iterator_top(void) {
        return run_stack.iterator_top();
    }

public:

    cx_executor(void) : cx_backend() {
        statement_count = 0;

        extern bool cx_dev_debug_flag;

        trace_routine_flag = cx_dev_debug_flag;
        trace_statement_flag = cx_dev_debug_flag;
        trace_store_flag = cx_dev_debug_flag;
        trace_fetch_flag = cx_dev_debug_flag;

        break_loop = false;
    }

    virtual void go(cx_symtab_node *p_program_id);
};

#endif