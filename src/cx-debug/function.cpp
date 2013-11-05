/** Executor (Routines)
 * exec_routine.cpp
 *
 * Execute programs, functions, and calls
 * to declared and standard subroutines.
 */

#include <memory.h>
#include "common.h"
#include "cx-debug/exec.h"

/** execute_routine    	Execute a program, procedure, or
 *			function.
 *
 * @param p_function_id : ptr to the routine symtab node
 */
void cx_executor::execute_routine(cx_symtab_node *p_function_id) {

    enter_routine(p_function_id);

    execute_compound(p_function_id);

    exit_routine(p_function_id);
}

/** enter_routine    	enter a routine:  Switch to its
 *			intermediate code and allocate its
 *			local variables on the runtime stack.
 *
 * @param p_function_id : ptr to routine name's symbol table node
 */
void cx_executor::enter_routine(cx_symtab_node * p_function_id) {
    cx_symtab_node *p_id; // ptr to local variable's symtab node

    trace_routine_entry(p_function_id);

    // Allocate the callee's local variables.
    for (p_id = p_function_id->defn.routine.locals.p_variable_ids;
            p_id;
            p_id = p_id->next__) run_stack.allocate_value(p_id);

    // Switch to the callee's intermediate code.
    p_icode = p_function_id->defn.routine.p_icode;
    p_icode->reset();

}

/** exit_routine    	Exit a routine:  Deallocate its local
 *			parameters and variables, pop its frame
 *			off the runtime stack, and return to the
 *			caller's intermediate code.
 *
 * @param p_function_id : ptr to routine name's symbol table node
 */
void cx_executor::exit_routine(cx_symtab_node *p_function_id) {
    cx_symtab_node *p_id; // ptr to symtab node of local variable or parm

    trace_routine_exit(p_function_id);

    // Deallocate local parameters and variables.
    for (p_id = p_function_id->defn.routine.locals.p_parms_ids;
            p_id;
            p_id = p_id->next__) run_stack.deallocate_value(p_id);

    for (p_id = p_function_id->defn.routine.locals.p_variable_ids;
            p_id;
            p_id = p_id->next__) run_stack.deallocate_value(p_id);

    // pop off the callee's stack frame and return to the caller's
    // intermediate code.
    run_stack.pop_frame(p_function_id, p_icode);
}

/** execute_subroutine_call	Execute a call to a procedure or
 *				or a function.
 *
 * @param p_function_id : ptr to the subroutine name's symtab node
 *
 * @return: ptr to the call's type object
 */
cx_type *cx_executor::execute_subroutine_call(cx_symtab_node *p_function_id) {
    /*return p_function_id->defn.routine.which == rc_declared
                ? execute_declared_subroutine_call(p_function_id)
                : execute_standard_subroutine_call(p_function_id);*/
    return execute_declared_subroutine_call(p_function_id);
}

/** execute_declared_subroutine_call   Execute a call to a declared
 *				    procedure or function.
 *
 * @param p_function_id : ptr to the subroutine name's symtab node
 *
 * @return: ptr to the call's type object
 */
cx_type *cx_executor::execute_declared_subroutine_call
(cx_symtab_node *p_function_id) {
    int old_level = current_nesting_level; // level of caller
    int new_level = p_function_id->level + 1; // level of callee's locals

    // Set up a new stack frame for the callee.
    cx_frame_header *p_new_frame_base = run_stack.push_frame_header
            (old_level, new_level, p_icode);

    // push actual parameter values onto the stack.
    get_token();

    if (token == tc_left_paren) {
        execute_actual_parameters(p_function_id);
    }

    //  )
    get_token();

    // Activate the new stack frame ...
    current_nesting_level = new_level;
    run_stack.activate_frame(p_new_frame_base, current_location() - 1);

    // ... and execute the callee.
    execute_routine(p_function_id);

    // Return to the caller.  Restore the current token.
    current_nesting_level = old_level;
    get_token();

    return p_function_id->p_type;
}

/** execute_actual_parameters	Execute the actual parameters of
 *				a declared subroutine call.
 *
 * @param p_function_id : ptr to the subroutine name's symtab node
 */
void cx_executor::execute_actual_parameters(cx_symtab_node *p_function_id) {
    cx_symtab_node *p_formal_id; // ptr to formal parm's symtab node

    // Loop to execute each actual parameter.
    for (p_formal_id = p_function_id->defn.routine.locals.p_parms_ids;
            p_formal_id;
            p_formal_id = p_formal_id->next__) {

        cx_type *p_formal_type = p_formal_id->p_type;
        get_token();

        /* Reference parameter: execute_variable will leave the actual
         * parameter's address on top of the stack. */
        if (p_formal_id->defn.how == dc_reference) {

            const int size = p_node->p_type->size;
            p_formal_type->form = p_node->p_type->form;
            execute_variable(p_node, true);

            if (p_formal_type->form == fc_array) {

                p_formal_type->size = size;
                p_formal_type->array.element_count = size;
                p_formal_type->array.max_index = size;

                p_formal_id->runstack_item = top();

            } else {
                p_formal_id->runstack_item = top();
            }

            get_token();
        }// value parameter
        else {
            cx_type *p_actual_type = execute_expression();

            if ((p_formal_type == p_float_type) &&
                    (p_actual_type->base_type() == p_integer_type)) {

                // real formal := integer actual:
                // convert integer value to real.
                float ff = top()->basic_types.int__;
                pop();

                push(ff);
                p_formal_id->runstack_item = top();
            } else if (!p_formal_type->is_scalar_type()) {

                /* Formal parameter is an array or a record:
                 * Make a copy of the actual parameter's value. */

                const int size = p_actual_type->size;
                void *p_target_address = nullptr;
                const int num_of_elements = size / p_actual_type->base_type()->size;

                p_target_address = realloc(p_target_address, size);
                memset(p_target_address, 0, size);

                if (p_target_address == nullptr) {
                    perror("realloc");
                    exit(0);
                }

                void *p_source = top()->basic_types.addr__;
                memcpy(p_target_address, p_source, size);

                pop();
                push(p_target_address);

                p_formal_type->array.element_count = num_of_elements;
                p_formal_type->array.max_index = num_of_elements;
                p_formal_type->size = size;
                p_formal_type->form = fc_array;
                p_formal_id->runstack_item = top();
            } else {

                // Range check an integer or enumeration
                // formal parameter.
                range_check(p_formal_type, top()->basic_types.int__);
                p_formal_id->runstack_item = top();
            }
        }
    }

    if (token == tc_left_paren) get_token();
}

/** execute_RETURN	Assign a return value to the functions StackItem and
 *                      set current location to the return line of the caller.
 *
 *      return;
 *      return <expression>;
 *
 * @param p_function_id : ptr to the subroutine name's symtab node
 */
void cx_executor::execute_RETURN(cx_symtab_node *p_function_id) {
    execute_assignment(p_function_id);
    token = tc_dummy;
}
