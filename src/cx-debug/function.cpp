/** Executor (Routines)
 * exec_routine.cpp
 *
 * Execute programs, functions, and calls
 * to declared and standard subroutines.
 */

#include <memory.h>
#include "common.h"
#include "exec.h"

/** execute_routine    	Execute a program, procedure, or
 *			function.
 *
 * @param p_function_id : ptr to the routine symtab node
 */

void cx_executor::execute_function (cx_symtab_node *p_function_id) {
    enter_function(p_function_id);
    execute_compound(p_function_id);
    exit_function(p_function_id);
}

void cx_executor::execute_iterator(cx_symtab_node* p_function_id) {
	int old_level = current_nesting_level; // level of caller
	int new_level = p_function_id->level + 1; // level of callee's locals
	cx_symtab_node *p_formal_id = p_function_id->defn.routine.locals.p_parms_ids;
	int *iteration = &p_function_id->defn.routine.iterator.current_iteration;
	const int end = p_function_id->defn.routine.member_of.p_node->p_type->array.element_count;
	int loop_start = p_function_id->defn.routine.iterator.loop_start;
	const int size = p_function_id->defn.routine.member_of.p_node->p_type->array.p_element_type->size;

	int index = p_function_id->defn.routine.iterator.current_iteration = 0;
	cx_symtab_node *p_var = p_function_id->defn.routine.member_of.p_node;

	get_token();

	if (token == tc_left_paren) {
		execute_iterator_params(p_function_id);
	}

	//  )
	get_token();
	current_nesting_level = new_level;
	enter_iterator(p_function_id);

	char *addr = (char *)p_var->runstack_item->basic_types.addr__;
	while (*iteration < end){

		// push actual parameter values onto the stack.
		push(addr + (size * (*iteration)));
		
		p_formal_id->runstack_item = (cx_stack_item *)top()->basic_types.addr__;

		++*iteration;
		execute_statement(p_function_id);
		//pop();
		go_to(loop_start);
		while(token != tc_dummy)get_token();
		while(token == tc_dummy)get_token();
		pop();
	}


	exit_function(p_function_id);
	pop(); // pop off param

	// Return to the caller.  Restore the current token.
	current_nesting_level = old_level;
	go_to(p_function_id->defn.routine.iterator.loop_end);
	while (token != tc_dummy)get_token();
	while (token == tc_dummy)get_token();

}

void cx_executor::execute_iterator_params (cx_symtab_node* p_function_id) {
    
    cx_symtab_node *p_formal_id; // ptr to formal parm's symtab node
    cx_symtab_node *p_var_node = p_function_id->defn.routine.member_of.p_node;

    cx_type *p_actual_type = p_var_node->p_type;

    p_formal_id = p_function_id->defn.routine.locals.p_parms_ids;

    cx_type *p_formal_type = p_formal_id->p_type;
    get_token();

    /* Reference parameter: execute_variable will leave the actual
     * parameter's address on top of the stack. */
    if (p_formal_id->defn.how == dc_reference) {
		p_formal_id->runstack_item = top();
    }// value parameter
    else {

        if (!p_formal_type->is_scalar_type()) {

            /* Formal parameter is an array or a record:
             * Make a copy of the actual parameter's value. */

            const int size = p_actual_type->size;
            void *p_target_address = nullptr;
            const int num_of_elements = size / p_actual_type->base_type()->size;

            p_target_address = realloc(p_target_address, size + 1);

            if (p_target_address == nullptr) {
                perror("realloc");
                exit(0);
            }

            void *p_source = top()->basic_types.addr__;

            memset(p_target_address, 0, size + 1);
            memcpy(p_target_address, p_source, size + 1);

            char *y = (char *) p_target_address;
            char *t = (char *) p_source;
            pop();
            push((void*) p_target_address);

            set_type(p_formal_type->array.p_element_type, p_actual_type->array.p_element_type);
            p_formal_type->array.element_count = num_of_elements;
            p_formal_type->array.max_index = num_of_elements;
            p_formal_type->size = size;
            p_formal_id->runstack_item = top();

        } else {
            cx_stack_item *t = (cx_stack_item *) top()->basic_types.addr__;
            pop();
            switch (p_formal_id->p_type->type_code) {
                case cx_uint8:
                    push((uint8_t) t->basic_types.uint8__);
                    break;
                case cx_int:
                    push((int) t->basic_types.int__);
                    break;
                case cx_char:
                    push((char) t->basic_types.char__);
                    break;
                case cx_bool:
                    push((bool)t->basic_types.bool__);
                    break;
                case cx_float:
                    push((float) t->basic_types.float__);
                    break;
                case cx_wchar:
                    push((wchar_t)t->basic_types.wchar__);
                    break;
                default: break;
            }
            p_formal_id->runstack_item = top();
        }
    }

    get_token();
}

void cx_executor::enter_iterator (cx_symtab_node* p_function_id) {
    cx_symtab_node *p_id; // ptr to local variable's symtab node

    trace_routine_entry(p_function_id);

    // Allocate the callee's local variables.
    for (p_id = p_function_id->defn.routine.locals.p_variable_ids;
         p_id;
         p_id = p_id->next__) run_stack.allocate_value(p_id);

    // Switch to the callee's intermediate code.
    //p_icode = p_function_id->defn.routine.p_icode;
    //p_icode->reset();
}

/** enter_routine    	enter a routine:  Switch to its
 *			intermediate code and allocate its
 *			local variables on the runtime stack.
 *
 * @param p_function_id : ptr to routine name's symbol table node
 */
void cx_executor::enter_function (cx_symtab_node * p_function_id) {
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
void cx_executor::exit_function (cx_symtab_node *p_function_id) {
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
	if (p_function_id->defn.routine.which != func_std_iterator){
		run_stack.pop_frame(p_function_id, p_icode);
	}
}

/** execute_subroutine_call	Execute a call to a procedure or
 *				or a function.
 *
 * @param p_function_id : ptr to the subroutine name's symtab node
 *
 * @return: ptr to the call's type object
 */
cx_type *cx_executor::execute_function_call (cx_symtab_node *p_function_id) {
    cx_type *p_result_type = nullptr;

    switch (p_function_id->defn.routine.which) {
        case func_declared:
            p_result_type = execute_decl_function_call(p_function_id);
            break;
        case func_standard:
            p_result_type = execute_std_function_call(p_function_id);
            break;
        default:
            p_result_type = (*p_function_id->defn.routine.std_member)
                    (this, p_function_id, p_function_id->p_type);

            //@TODO need error out, may be forwarded but no function body
            break;
    }

    return p_result_type;
}

cx_type *cx_executor::execute_std_member_call (cx_symtab_node *p_function_id, cx_type *p_type) {
    get_token();

    if (token == tc_left_paren) {
        // push actual parameter values onto the stack.
        execute_actual_parameters(p_function_id);
        //  )
        get_token();
    }

    return (*p_function_id->defn.routine.std_member)(this, p_function_id, p_type);
}

/** execute_declared_subroutine_call   Execute a call to a declared
 *				    procedure or function.
 *
 * @param p_function_id : ptr to the subroutine name's symtab node
 *
 * @return: ptr to the call's type object
 */
cx_type *cx_executor::execute_decl_function_call
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
    execute_function(p_function_id);

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
void cx_executor::execute_actual_parameters (cx_symtab_node *p_function_id) {
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
            set_type(p_formal_type, p_node->p_type);
            execute_variable(p_node, true);
            /*
                        if (p_formal_type->form == fc_array) {

                            p_formal_type->size = size;
                            p_formal_type->array.element_count = size;
                            p_formal_type->array.max_index = size;

                        }*/

            p_formal_id->runstack_item = top();
            get_token();
        }// value parameter
        else {
            cx_type *p_actual_type = execute_expression();

            if (!p_formal_type->is_scalar_type()) {

                /* Formal parameter is an array or a record:
                 * Make a copy of the actual parameter's value. */

                const int size = p_actual_type->size;
                void *p_target_address = nullptr;
                const int num_of_elements = size / p_actual_type->base_type()->size;

                p_target_address = realloc(p_target_address, size + 1);

                if (p_target_address == nullptr) {
                    perror("realloc");
                    exit(0);
                }

                void *p_source = top()->basic_types.addr__;

                memset(p_target_address, 0, size + 1);
                memcpy(p_target_address, p_source, size + 1);

                char *y = (char *) p_target_address;
                char *t = (char *) p_source;
                pop();
                push((void*) p_target_address);

                set_type(p_formal_type->array.p_element_type, p_actual_type->array.p_element_type);
                p_formal_type->array.element_count = num_of_elements;
                p_formal_type->array.max_index = num_of_elements;
                p_formal_type->size = size;
                p_formal_id->runstack_item = top();

            } else {
				cx_stack_item *t = (cx_stack_item *)top()->basic_types.addr__;
				pop();
				switch (p_formal_id->p_type->type_code) {
				case cx_uint8:
					push((uint8_t)t->basic_types.uint8__);
					break;
				case cx_int:
					push((int)t->basic_types.int__);
					break;
				case cx_char:
					push((char)t->basic_types.char__);
					break;
				case cx_bool:
					push((bool)t->basic_types.bool__);
					break;
				case cx_float:
					push((float)t->basic_types.float__);
					break;
				case cx_wchar:
					push((wchar_t)t->basic_types.wchar__);
					break;
				default: break;
				}
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
void cx_executor::execute_RETURN (cx_symtab_node * p_function_id) {
    execute_assignment(p_function_id);
    token = tc_dummy;
}

cx_type *cx_executor::execute_std_function_call (cx_symtab_node* p_function_id) {
    if (p_function_id->defn.routine.std_function != nullptr) {
        return (*p_function_id->defn.routine.std_function)(this, p_function_id);
    } else {

    }
    //@TODO need error checking
    return p_dummy_type;
}