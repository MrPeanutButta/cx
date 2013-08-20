#ifndef exec_h
#define exec_h

#include <vector>
#include "error.h"
#include "symtable.h"
#include "types.h"
#include "icode.h"
#include "backend.h"

///  cx_stack_item          Item pushed onto the runtime stack.
union cx_stack_item {
    int int__;
    float float__;
    char char__;
    void *addr__;
    char buffer[512];
};

///  cx_runtime_stack       Runtime stack class.

class cx_runtime_stack {

    enum {
        stack_size = 128,
        frame_header_size = 5,
    };

    // Stack frame header

    struct cx_frame_header {
        cx_stack_item function_value;
        cx_stack_item static_link;
        cx_stack_item dynamic_link;

        struct {
            cx_stack_item icode;
            cx_stack_item location;
        } return_address;
    };

    cx_stack_item stack[stack_size]; // stack items
    cx_stack_item *tos; // ptr to the top of the stack
    cx_stack_item *p_frame_base; // ptr to current stack frame base

public:
    cx_runtime_stack(void);

    void push(int value) {
        if (tos < &stack[stack_size - 1]) (++tos)->int__ = value;
        else cx_runtime_error(rte_stack_overflow);
    }

    void push(float value) {
        if (tos < &stack[stack_size - 1]) (++tos)->float__ = value;
        else cx_runtime_error(rte_stack_overflow);
    }

    void push(char value) {
        if (tos < &stack[stack_size - 1]) (++tos)->char__ = value;
        else cx_runtime_error(rte_stack_overflow);
    }

    void push(void *addr) {
        if (tos < &stack[stack_size - 1]) (++tos)->addr__ = addr;
        else cx_runtime_error(rte_stack_overflow);
    }

    cx_stack_item *push_frame_header(int old_level, int new_level,
            cx_icode *p_icode);
    void activate_frame(cx_stack_item *p_new_frame_base, int location);
    void pop_frame(const cx_symtab_node *p_function_id, cx_icode *&p_icode);

    cx_stack_item *pop(void) {
        return tos--;
    }

    cx_stack_item *top_of_stack(void) const {
        return tos;
    }

    void allocate_value(cx_symtab_node *p_id);
    void deallocate_value(const cx_symtab_node *p_id);

    cx_stack_item *get_value_address(const cx_symtab_node *p_id);
};

//  cx_executor           Executor subclass of cx_backend.

class cx_executor : public cx_backend {
    long statement_count; // count of executed statements
    cx_runtime_stack run_stack; // ptr to runtime stack

    bool eof_flag; // true if at end of file, else false

    bool break_loop; // if true, breaks current loop

    // Trace flags
    int trace_routine_flag; // true to trace routine entry/exit
    int trace_statement_flag; // true to trace statements
    int trace_store_flag; // true to trace data stores
    int trace_fetch_flag; // true to trace data fetches

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
    cx_symtab_node *enter_new(cx_symtab_node *p_function_id, const char *p_string);
    cx_symtab_node *allocate_new_node(cx_symtab_node *p_function_id);
    void execute_statement(cx_symtab_node *p_function_id);
    void execute_statement_list(cx_symtab_node *p_function_id, cx_token_code terminator);
    void execute_assignment(const cx_symtab_node *p_target_id);
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
            const void *p_data_value,
            const cx_type *p_data_type);
    void trace_data_fetch(const cx_symtab_node *p_id,
            const void *p_data_value,
            const cx_type *p_data_type);
    void trace_data_value(const void *p_data_value,
            const cx_type *p_data_type);

    void range_check(const cx_type *p_target_type, int value);

    void push(int value) {
        run_stack.push(value);
    }

    void push(float value) {
        run_stack.push(value);
    }

    void push(char value) {
        run_stack.push(value);
    }

    void push(void *addr) {
        run_stack.push(addr);
    }

    cx_stack_item *pop(void) {
        return run_stack.pop();
    }

    cx_stack_item *top_of_stack(void) const {
        return run_stack.top_of_stack();
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