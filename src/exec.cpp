/** Executor
 * exec.cpp
 *
 * Execute the intermediate code.
 */

#include <iostream>
#include "exec.h"



/*******************
 *                 *
 *  Runtime Stack  *
 *                 *
 *******************/

///  Constructor

cx_runtime_stack::cx_runtime_stack(void) {

    memset(&stack, 0, sizeof (stack));

    tos = &stack[-1]; // point to just below bottom of stack
    p_frame_base = &stack[0]; // point to bottom of stack

    // Initialize the program's stack frame at the bottom.
    push(0); // function return value
    push(0); // static  link
    push(0); // dynamic link
    push(0); // return address icode pointer
    push(0); // return address icode location

    //p_frame_base = stack[0]; // point to bottom of stack
}

/** push_frame_header     push the callee subroutine's stack frame
 *                     header onto the runtime stack.  (Leave
 *                     it inactive.)
 *
 * @param old_level : nesting level of the caller routine
 * @param new_level : nesting level of the callee subroutine's
 *                     formal parameters and local variables
 * @param p_icode   : ptr to caller's intermediate code
 *
 * @return: ptr to the base of the callee's stack frame
 */
cx_stack_item *cx_runtime_stack::push_frame_header(int old_level, int new_level,
        cx_icode *p_icode, size_t return_size) {

    cx_frame_header *p_header = (cx_frame_header *) p_frame_base;
    cx_stack_item *p_new_frame_base = tos + 1; /* point to item just above
                                                //*  current top_of_stack item */

    /*push((void *) new cx_stack_item(0));
    cx_stack_item *p_new_frame_base = tos;*/

    push(0); // function return value (placeholder)
    // cx_stack_item *p_new_frame_base = tos;

    // Compute the static link.
    if (new_level == old_level + 1) {

        /*--Callee nested within caller:
         *--push address of caller's stack frame.*/
        push(p_header);
    } else if (new_level == old_level) {

        /*--Callee at same level as caller:
          --push address of common parent's stack frame.*/
        push(p_header->static_link.addr__);
    } else /* new_level < old_level */ {

        /*--Callee nested less deeply than caller:
          --push address of nearest common ancestor's stack frame.*/
        int delta = old_level - new_level;

        while (delta-- >= 0) {
            p_header = (cx_frame_header *) p_header->static_link.addr__;
        }
        push(p_header);
    }

    push(p_frame_base); // dynamic link    
    push(p_icode); // return address icode pointer
    push(0); // return address icode location (placeholder)
    
    return p_new_frame_base;
}

/** activate_frame       Activate the newly-allocated stack frame
 *                       by pointing the frame base pointer to it
 *                       and setting the return address location.
 *
 * @param p_new_frame_base : ptr to the new stack frame base
 * @param location      : return address location
 */

void cx_runtime_stack::activate_frame(cx_stack_item *p_new_frame_base,
        int location) {
    p_frame_base = p_new_frame_base;
    ((cx_frame_header *) p_frame_base)->return_address
            .location.int__ = location;
}

/** pop_frame    pop the current frame from the runtime stack.
 *              If it's for a function, leave the return value
 *              on the top of the stack.
 *
 * @param p_function_id : ptr to subroutine name's symbol table node
 * @param p_icode     : ref to ptr to caller's intermediate code
 */
void cx_runtime_stack::pop_frame(const cx_symtab_node *p_function_id,
        cx_icode *&p_icode) {

    cx_frame_header *p_header = (cx_frame_header *) p_frame_base;

    // Don't do anything if it's the bottommost stack frame.
    if (p_frame_base != &stack[0]) {
        
        // Return to the caller's intermediate code.
        p_icode = (cx_icode *) p_header->return_address.icode.addr__;
        p_icode->go_to(p_header->return_address.location.int__);

        // Leave a function value on top.
        tos = (cx_stack_item *) p_frame_base;
        if (p_function_id->defn.how != dc_function) --tos;
        p_frame_base = (cx_stack_item *) p_header->dynamic_link.addr__;
    }
}

/** allocate_value       Allocate a runtime stack item for the
 *                       value of a formal parameter or a local
 *                       variable.
 *
 * @param p_id : ptr to symbol table node of variable or parm
 */
void cx_runtime_stack::allocate_value(cx_symtab_node *p_id) {
    cx_type *p_type = p_id->p_type; // ptr to type object of value

    if ((p_type->form != fc_array) && (p_type->form != fc_complex)) {
        if (p_type == p_integer_type) push(0);
        else if (p_type == p_float_type) push(0.0f);
        else if (p_type == p_boolean_type) push(0);
        else if (p_type == p_char_type) push('\0');
        else if (p_type->form == fc_enum) push(0);
    } else {

        // Array or record
        void *addr = new char[p_type->size];
        push(addr);
    }

    /* save runstack address.
     * this negates the need to calculate the
     * variables offset. */
    p_id->runstack_item = top_of_stack();
}

/** deallocate_value    Deallocate the data area of an array or
 *                      record value of a formal value parameter
 *                      or a local variable.
 *
 * @param p_id : ptr to symbol table node of variable or parm
 */
void cx_runtime_stack::deallocate_value(const cx_symtab_node *p_id) {
    if ((!p_id->p_type->is_scalar_type()) && (p_id->defn.how != dc_reference)) {
        cx_stack_item *p_value = p_id->runstack_item;

        if (p_value->addr__ != nullptr) {
            delete[] p_value->addr__;
            p_value->addr__ = nullptr;
        }
    }
}
/** get_value_address     get the address of the runtime stack
 *                      item that contains the value of a formal
 *                      parameter or a local variable.  If
 *                      nonlocal, follow the static links to the
 *                      appropriate stack frame.
 *
 * @param p_id : ptr to symbol table node of variable or parm
 *
 * @return    : ptr to the runtime stack item containing the
 *                 variable, parameter, or function return value
 */
cx_stack_item *cx_runtime_stack::get_value_address(const cx_symtab_node *p_id) {
    bool functionFlag = p_id->defn.how == dc_function; // true if function
    //   else false
    cx_frame_header *p_header = (cx_frame_header *) p_frame_base;

    /*--Compute the difference between the current nesting level
     *--and the level of the variable or parameter.  Treat a function
     *--value as if it were a local variable of the function.  (Local
     *--variables are one level higher than the function name.)*/
    int delta = current_nesting_level - p_id->level;
    if (functionFlag) --delta;

    // Chase static links delta times.
    while (delta-- > 0) {
        p_header = (cx_frame_header *) p_header->static_link.addr__;
    }

    return functionFlag ? &p_header->function_value
            : p_id->runstack_item;
}

/**************
 *            *
 *  Executor  *
 *            *
 **************/


///  go                  Start the executor.

void cx_executor::go(cx_symtab_node *p_program_id) {
    // Initialize standard input and output.
    eof_flag = std::cin.eof();
    std::cout.setf(std::ios::fixed, std::ios::floatfield);

    // Execute the program.
    break_loop = false;

    initialize_global(p_program_id);
    exit_routine(p_program_id);

    extern bool cx_dev_debug_flag;

    if (cx_dev_debug_flag) {
        // print the executor's summary.
        std::cout << std::endl;
        std::cout << "Successful completion.  " << statement_count
                << " statements executed." << std::endl;
    }
}

/** range_check      Range check an assignment to a subrange.
 *
 * @param p_target_type : ptr to target type object
 * @param value       : integer value to assign
 */
void cx_executor::range_check(const cx_type *p_target_type, int value) {

    if ((p_target_type->form == fc_array)
            && ((value < p_target_type->array.min_index)
            || (value > p_target_type->array.maxIndex))) {
        cx_runtime_error(rte_value_out_of_range);
    }
}

/** initialize_global    Init global scope and execute it's icode
 *                      to make sure global variable get initialized
 *
 * @param p_program_id
 */
void cx_executor::initialize_global(cx_symtab_node* p_program_id) {

    // Set up a new stack frame for the callee.
    cx_stack_item *p_new_frame_base = run_stack.push_frame_header
            (0, 0, p_program_id->defn.routine.p_icode);

    // Activate the new stack frame ...
    current_nesting_level = 0;
    run_stack.activate_frame(p_new_frame_base, p_program_id->defn.routine.return_marker);

    enter_routine(p_program_id);
    get_token();
    execute_statement(p_program_id);
}
