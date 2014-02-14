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

#ifdef _WIN32
#include <windows.h>
#endif


extern bool cx_dev_debug_flag;
class cx_type;

union mem_block {
    uint8_t uint8__;
    bool bool__;
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
        char *t = (char *) basic_types.addr__;
        char *y = (char *) address;

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
		extern bool cx_dev_debug_flag;

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
private:
    cx_stack cx_runstack;

    const cx_frame_header *p_stackbase;
    cx_frame_header *p_frame_base; // ptr to current stack frame base

    cx_stack_iterator it_frame_base; // iterator to frame base

public:
    cx_runtime_stack(void);
	~cx_runtime_stack(void){
		cx_runstack.clear();
	}

    void push(const bool &value) {
        cx_runstack.push_back(new cx_stack_item((bool)value));
    }

    void push(const uint8_t &value) {
        cx_runstack.push_back(new cx_stack_item((uint8_t) value));
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
        cx_runstack.push_back(new cx_stack_item((void*) addr));
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
public:
    void execute_function(cx_symtab_node *p_function_id);
    void enter_function(cx_symtab_node *p_function_id);
    void execute_iterator(cx_symtab_node *p_function_id);
    void enter_iterator(cx_symtab_node *p_function_id);
    void execute_iterator_params (cx_symtab_node* p_function_id);
    void exit_function(cx_symtab_node *p_function_id);
    cx_type *execute_function_call(cx_symtab_node *p_function_id);
    cx_type *execute_decl_function_call(cx_symtab_node *p_function_id);
    cx_type *execute_std_function_call(cx_symtab_node *p_function_id);
    void execute_actual_parameters(cx_symtab_node *p_function_id);
	cx_type *execute_variable(const cx_symtab_node *p_id, bool address_flag);
private:
    // Statements
    cx_symtab_node *enter_new(cx_symtab_node *p_function_id,
            const char *p_string);
    //cx_symtab_node *allocate_new_node(cx_symtab_node *p_function_id);
    void execute_statement(cx_symtab_node *p_function_id);
    void execute_statement_list(cx_symtab_node *p_function_id,
            cx_token_code terminator);
	void execute_assignment(cx_symtab_node *p_target_id);

    // assignent
    void assign(cx_symtab_node* p_target_id,
            cx_type* p_target_type, cx_type* p_expr_type,
            cx_stack_item* p_target,
            void* &p_target_address);

    // compound assignment
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
    void or_equal(const cx_symtab_node* p_target_id,
            cx_type* p_target_type, const cx_type* p_expr_type,
            cx_stack_item* p_target);

    // increment
    void plus_plus(cx_type* p_target_type,
            cx_stack_item* p_target);
    void minus_minus(cx_type* p_target_type,
            cx_stack_item* p_target);

    // relational
    void execute_relational(cx_token_code op, cx_type *lhs, cx_type *rhs);
    void equal_equal(cx_type *lhs, cx_type *rhs);
    void not_equal(cx_type *lhs, cx_type *rhs);
    void lessthan(cx_type *lhs, cx_type *rhs);
    void greaterthan(cx_type *lhs, cx_type *rhs);
    void lessthan_equal(cx_type *lhs, cx_type *rhs);
    void greaterthan_equal(cx_type *lhs, cx_type *rhs);

    // unary
    void unary_negate(cx_type *expr);
    void unary_bit_not(cx_type *expr);

    // additive
    cx_type *execute_additive(cx_token_code op, cx_type *lhs, cx_type *rhs);
    cx_type *plus(cx_type *lhs, cx_type *rhs);
    void minus(cx_type *lhs, cx_type *rhs);
    void bit_leftshift(cx_type *lhs, cx_type *rhs);
    void bit_rightshift(cx_type *lhs, cx_type *rhs);
    void bit_and(cx_type *lhs, cx_type *rhs);
    void bit_xor(cx_type *lhs, cx_type *rhs);
    void bit_or(cx_type *lhs, cx_type *rhs);
    void logic_or(cx_type *lhs, cx_type *rhs);

    // multiplicative
    cx_type *execute_multiplicative(cx_token_code op, cx_type *lhs, cx_type *rhs);
    void multiply(cx_type *lhs, cx_type *rhs);
    void divide(cx_type *lhs, cx_type *rhs);
    void modulas(cx_type *lhs, cx_type *rhs);
    void logic_and(cx_type *lhs, cx_type *rhs);

    // factors
    cx_type *number(cx_symtab_node *num);
    void logic_not(void);

    void file_out(const cx_symtab_node* p_target_id,
            const cx_type* p_expr_type);

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
	cx_type *execute_subscripts(const cx_type *p_type);
    cx_type *execute_field(cx_type *);
    cx_type *execute_initialization_list(void);
    cx_type *execute_std_member_call(cx_symtab_node *p_function_id, cx_type *p_type);
    
    cx_type *alloc_temp_rvalue(const cx_type *lhs, cx_type * rhs);

    void cx_malloc(cx_symtab_node* p_target_id,
            cx_type* p_target_type, cx_type* p_expr_type, cx_stack_item* p_target,
            void* &p_target_address);

    // Tracing
    void trace_routine_entry(const cx_symtab_node * p_function_id);
    void trace_routine_exit(const cx_symtab_node * p_function_id);
    void trace_statement(void);
    void trace_data_store(const cx_symtab_node *p_target_id,
            const cx_stack_item &p_data_value,
            const cx_type * p_data_type);
    void trace_data_fetch(const cx_symtab_node *p_id,
            const cx_stack_item &p_data_value,
            const cx_type * p_data_type);
    void trace_data_value(const cx_stack_item &p_data_value,
            const cx_type * p_data_type);

    void range_check(const cx_type *p_target_type, int value);

public:

    void get_next_token(void){
        get_token();
    }
    
    int location(void)const{
        return current_location();
    }
    
    void goto_location(int location) {
        go_to(location);
    }
    
    void push(const uint8_t & value) {
        run_stack.push(value);
    }

    void push(const bool &value) {
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
        char *t = (char *) addr;
        run_stack.push((void*) addr);
    }

    void pop(void) {
        run_stack.pop();
    }

    cx_stack_item * top(void) const {
        return run_stack.top();
    }

    const cx_stack_iterator iterator_top(void) {
        return run_stack.iterator_top();
    }

    cx_executor(void) : cx_backend() {
        statement_count = 0;

        trace_routine_flag = cx_dev_debug_flag;
        trace_statement_flag = cx_dev_debug_flag;
        trace_store_flag = cx_dev_debug_flag;
        trace_fetch_flag = cx_dev_debug_flag;

        break_loop = false;
    }

	~cx_executor(void){
#ifdef _WIN32
		extern std::vector<HINSTANCE> windows_libs;
		for (HINSTANCE &h_lib : windows_libs) FreeLibrary(h_lib);
#endif
	}

    virtual void go(cx_symtab_node * p_program_id);
};

#endif