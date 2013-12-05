/** Executor
 * exec.cpp
 *
 * Execute the intermediate code.
 */

#include <iostream>
#include "exec.h"

extern  cx_type *p_integer_type;
extern  cx_type *p_float_type;
extern  cx_type *p_boolean_type;
extern  cx_type *p_char_type;

/*******************
 *                 *
 *  Runtime Stack  *
 *                 *
 *******************/

///  Constructor

cx_runtime_stack::cx_runtime_stack (void) {

    // Initialize the program's stack frame at the bottom.

    push_frame(); // function return value
    p_frame_base = (cx_frame_header *) top(); // point to bottom of stack
    p_stackbase = (cx_frame_header *) top();

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
cx_frame_header *
cx_runtime_stack::push_frame_header (int old_level, int new_level,
                                     cx_icode *p_icode) {

    push(-1); // function return value (placeholder)
    cx_stack_item *return_value = (cx_stack_item *) top();

    // push new frame header just above the return value
    push_frame();
    cx_frame_header *p_new_frame_base = (cx_frame_header *) top();
    p_new_frame_base->frame_header_index = cx_runstack.size() - 1;

    p_new_frame_base->function_value = return_value;

    // link back to original frame base
    p_new_frame_base->dynamic_link->basic_types.addr__ = p_frame_base;
    p_new_frame_base->return_address.icode->basic_types.addr__ = p_icode;

    return p_new_frame_base;
}

/** activate_frame       Activate the newly-allocated stack frame
 *                       by pointing the frame base pointer to it
 *                       and setting the return address location.
 *
 * @param p_new_frame_base : ptr to the new stack frame base
 * @param location      : return address location
 */

void
cx_runtime_stack::activate_frame (cx_frame_header *p_new_frame_base,
                                  const int &location) {

    p_frame_base = p_new_frame_base;
    p_frame_base->return_address.location->basic_types.int__ = location;

}

/** pop_frame    pop the current frame from the runtime stack.
 *              If it's for a function, leave the return value
 *              on the top of the stack.
 *
 * @param p_function_id : ptr to subroutine name's symbol table node
 * @param p_icode     : ref to ptr to caller's intermediate code
 */
void
cx_runtime_stack::pop_frame (const cx_symtab_node *p_function_id,
                             cx_icode *&p_icode) {

    cx_frame_header *p_header = p_frame_base;

    // Don't do anything if it's the bottommost stack frame.
    if (p_frame_base != p_stackbase) {
        // Return to the caller's intermediate code.
        p_icode = (cx_icode *) p_header->return_address.icode->basic_types.addr__;
        p_icode->go_to(p_header->return_address.location->basic_types.int__);

        it_frame_base = p_header->return_address.previous_header;

        int start = p_header->frame_header_index;

        // cut the stack back and leave frame header on TOS
        cx_runstack.erase(cx_runstack.begin() + start, cx_runstack.end());

        if (p_function_id->defn.how != dc_function) pop();

        p_frame_base = (cx_frame_header *) p_header->dynamic_link->basic_types.addr__;
    }
}

/** allocate_value       Allocate a runtime stack item for the
 *                       value of a formal parameter or a local
 *                       variable.
 *
 * @param p_id : ptr to symbol table node of variable or parm
 */
void
cx_runtime_stack::allocate_value (cx_symtab_node *p_id) {
    cx_type *p_type = p_id->p_type; // ptr to type object of value

    if ((p_type->form != fc_array) && (p_type->form != fc_complex)) {
        if (p_type == p_integer_type) push((int) 0);
        else if (p_type == p_float_type) push((float) 0.0);
        else if (p_type == p_wchar_type) push((wchar_t)'\0');
        else if (p_type == p_uint8_type) push((uint8_t) 0);
        else if (p_type == p_boolean_type) push((bool)false);
        else if (p_type == p_char_type) push((char) '\0');
        else if (p_type->form == fc_enum) push((int) 0);
    } else {

        //        if (p_type->size > 0) {
        //            // Array or record
        //            const int size = p_type->size;
        //            void *addr = malloc(size + 1);
        //            push(addr);
        //        } else {
        push((void *) nullptr);
        //}
    }

    /* save runstack address.
     * this negates the need to calculate the
     * variables offset. */
    p_id->runstack_item = top();
}

/** deallocate_value    Deallocate the data area of an array or
 *                      record value of a formal value parameter
 *                      or a local variable.
 *
 * @param p_id : ptr to symbol table node of variable or parm
 */
void
cx_runtime_stack::deallocate_value (cx_symtab_node *p_id) {

    if (p_id->runstack_item == nullptr) return;

    void *addr = p_id->runstack_item->basic_types.addr__;
    cx_type_form_code form = p_id->p_type->form;
    cx_define_code def_how = p_id->defn.how;

    if ((addr != nullptr) && (form == fc_array) && (def_how != dc_reference)) {
        free(p_id->runstack_item->basic_types.addr__);
    }

    p_id->runstack_item = nullptr;
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
cx_stack_item *
cx_runtime_stack::get_value_address (const cx_symtab_node *p_id) {
    bool functionFlag = p_id->defn.how == dc_function; // true if function
    //   else false
    cx_frame_header *p_header = (cx_frame_header *) p_frame_base;

    return functionFlag ? p_header->function_value
            : p_id->runstack_item;
}

/**************
 *            *
 *  Executor  *
 *            *
 **************/


///  go                  Start the executor.

void
cx_executor::go (cx_symtab_node *p_program_id) {
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
        std::cout << "\nSuccessful completion.  " << statement_count
                << " statements executed.\n\n";
    }
}

/** range_check      Range check an assignment to a subrange.
 *
 * @param p_target_type : ptr to target type object
 * @param value       : integer value to assign
 */
void
cx_executor::range_check (const cx_type *p_target_type, int value) {

    if ((p_target_type->form == fc_array)
        && ((value < p_target_type->array.min_index)
            || (value > p_target_type->array.max_index))) {
        cx_runtime_error(rte_value_out_of_range);
    }
}

/** initialize_global    Init global scope and execute it's icode
 *                      to make sure global variable get initialized
 *
 * @param p_program_id
 */
void
cx_executor::initialize_global (cx_symtab_node* p_program_id) {

    // Set up a new stack frame for the callee.
    cx_frame_header *p_new_frame_base = run_stack.push_frame_header
            (0, 0, p_program_id->defn.routine.p_icode);

    // Activate the new stack frame
    current_nesting_level = 0;
    run_stack.activate_frame(p_new_frame_base, p_program_id->defn.routine.return_marker);

    enter_routine(p_program_id);
    get_token();
    execute_statement(p_program_id);
}
