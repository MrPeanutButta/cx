#include <cstdio>
#include <iostream>
#include "error.h"
#include "buffer.h"
#include "symtab.h"
#include "types.h"
#include "icode.h"

int asm_label_index = 0; // assembly label index
bool xreference_flag = false; // true = cross-referencing on, false = off

/****************
 *              *
 *  Definition  *
 *              *
 ****************/

/** Destructor      Delete the local symbol table and icode of a
 *                  program, procedure or function definition.
 *                  Note that the parameter and local identifier
 *                  chains are deleted along with the local
 *                  symbol table.
 *
 */
cx_define::~cx_define(void) {
    switch (how) {

            // program_id is now managed unique_ptr
            //case dc_program:
        case dc_function:

            if (routine.which == func_declared) {
                if (routine.p_symtab != nullptr) delete routine.p_symtab;
                if (routine.p_icode != nullptr) delete routine.p_icode;
            }
            break;

        default: break;
    }
}

/***********************
 *                     *
 *  Symbol Table Node  *
 *                     *
 ***********************/

/** Constructor     Construct a symbol table node by initial-
 *                  izing its subtree pointers and the pointer
 *                  to its symbol string.
 *
 * @param p_str : ptr to the symbol string.
 * @param dc    : definition code.
 */
cx_symtab_node::cx_symtab_node(const char *p_str, cx_define_code dc)
: defn(dc) {
    left__ = right__ = next__ = nullptr;
    p_line_num_list = nullptr;
    p_type = nullptr;
    xnode = 0;
    global_finish_location = 0;
    found_global_end = false;
    level = current_nesting_level;
    label_index = ++asm_label_index;

	//this->runstack_item = nullptr;
    // Allocate and copy the symbol string.
    node_name = p_str;
}

/** Destructor      Deallocate a symbol table node.
 *
 */
cx_symtab_node::~cx_symtab_node(void) {
    void remove_type(cx_type *&p_type);

    // First the subtrees (if any).
    if (left__ != nullptr) delete left__;
    if (right__ != nullptr) delete right__;

    // Then delete this node's components.
    if (p_line_num_list != nullptr) delete p_line_num_list;
    if (p_type != nullptr) remove_type(p_type);

    p_type = nullptr;
    p_line_num_list = nullptr;
    left__ = nullptr;
    right__ = nullptr;
}

/******************
 *                *
 *  Symbol Table  *
 *                *
 ******************/

void final_copy(cx_symtab_node *p_dst, const cx_symtab_node *p_node){

	set_type(p_dst->p_type, p_node->p_type);

	p_dst->defn.routine.locals.p_parms_ids = p_node->defn.routine.locals.p_parms_ids;
	p_dst->defn.routine.locals.p_constant_ids = p_node->defn.routine.locals.p_constant_ids;
	p_dst->defn.routine.locals.p_type_ids = p_node->defn.routine.locals.p_type_ids;
	p_dst->defn.routine.locals.p_variable_ids = p_node->defn.routine.locals.p_variable_ids;
	p_dst->defn.routine.locals.p_function_ids = p_node->defn.routine.locals.p_function_ids;

	p_dst->defn.routine.p_symtab = p_node->defn.routine.p_symtab;
	p_dst->defn.routine.p_icode = p_node->defn.routine.p_icode;
	p_dst->defn.routine.ext_function = p_node->defn.routine.ext_function;

	p_dst->defn.this_ptr.p_node = p_node->defn.this_ptr.p_node;
	p_dst->defn.this_ptr.p_stack_item = p_node->defn.this_ptr.p_stack_item;

	p_dst->defn.io.stream = p_node->defn.io.stream;

}

/** search      search the symbol table for the node with a
*              given name string.
*
* @param p_string : ptr to the name string to search for.
* @return ptr to the node if found, else nullptr.
*/
void cx_symtab::copy_into(cx_symtab *p_dst) const {
	cx_symtab_node *p_node = root__; // ptr to symbol table node
	cx_symtab_node *p_cnode = nullptr;

	// copy left
	while (p_node) {
		p_cnode = p_dst->enter(p_node->node_name.c_str());

		memcpy(p_cnode, p_node, sizeof(p_node));
		final_copy(p_cnode, p_node);
		p_node = p_node->left__;
	}

	p_node = root__->right__;

	// copy right
	while (p_node) {
		p_cnode = p_dst->enter(p_node->node_name.c_str());

		memcpy(p_cnode, p_node, sizeof(p_node));
		final_copy(p_cnode, p_node);
		p_node = p_node->right__;
	}

}


/** search      search the symbol table for the node with a
 *              given name string.
 *
 * @param p_string : ptr to the name string to search for.
 * @return ptr to the node if found, else nullptr.
 */
cx_symtab_node *cx_symtab::search(const char *p_string) const {
    cx_symtab_node *p_node = root__; // ptr to symbol table node

    // Loop to search the table.
    while (p_node) {
        //int comp = strcmp(p_string, p_node->node_name.c_str()); // compare names
        int comp = std::string(p_string).compare(p_node->node_name);
        if (comp == 0) break; // found!

        // Not yet found:  next__ search left__ or right__ subtree.
        p_node = comp < 0 ? p_node->left__ : p_node->right__;
    }

    return p_node; // ptr to node, or nullptr if not found
}

/** enter       search the symbol table for the node with a
 *              given name string.  If the node is found, return
 *              a pointer to it.  Else if not found, enter a new
 *              node with the name string, and return a pointer
 *              to the new node.
 *
 * @param p_string : ptr to the name string to enter.
 * @param dc      : definition code.
 * @return ptr to the node, whether existing or newly-entered.
 */
cx_symtab_node *cx_symtab::enter(const char *p_string, cx_define_code dc) {
    cx_symtab_node *p_node; // ptr to node
    cx_symtab_node **ppNode = &root__; // ptr to ptr to node

    // Loop to search table for insertion point.
    while ((p_node = *ppNode) != nullptr) {
        //int comp = strcmp(p_string, p_node->node_name.c_str()); // compare strings
        int comp = std::string(p_string).compare(p_node->node_name);

        if (comp == 0) return p_node; // found!

        // Not yet found:  next__ search left__ or right__ subtree.
        ppNode = comp < 0 ? &(p_node->left__) : &(p_node->right__);
    }

    // Create and insert a new node.
    p_node = new cx_symtab_node(p_string, dc); // create a new node,
    p_node->xnode = nodes_count++; // node indexes,
    *ppNode = p_node; // insert it, and

	p_node->defn.routine.locals.p_parms_ids = nullptr;
	p_node->defn.routine.locals.p_constant_ids = nullptr;
	p_node->defn.routine.locals.p_type_ids = nullptr;
	p_node->defn.routine.locals.p_variable_ids = nullptr;
	p_node->defn.routine.locals.p_function_ids = nullptr;
	p_node->runstack_item = nullptr;

    return p_node; // return a ptr to it
}

/** enter_new    search the symbol table for the given name
 *              string.  If the name is not already in there,
 *              enter it.  Otherwise, flag the redefined
 *              identifier error.
 *
 * @param p_string : ptr to name string to enter.
 * @param dc      : definition code.
 * @return ptr to symbol table node.
 */
cx_symtab_node *cx_symtab::enter_new(const char *p_string, cx_define_code dc) {
    cx_symtab_node *p_node = search(p_string);

    if (!p_node) p_node = enter(p_string, dc);
    else cx_error(err_redefined_identifier);

    return p_node;
}

/************************
 *		             *
 *  Symbol Table Stack  *
 *		             *
 ************************/

/** Constructor	    Initialize the global (level 0) symbol
 *		    table, and set the others to nullptr.
 *
 */
cx_symtab_stack::cx_symtab_stack(void) {
    extern cx_symtab cx_global_symtab;
    extern cx_symtab_node *p_main_function_id;
    void initialize_std_functions(cx_symtab * p_symtab);

    current_nesting_level = 0;
    for (int i = 1; i < max_nesting_level; ++i) p_symtabs[i] = nullptr;

    // Initialize the global nesting level.
    p_symtabs[0] = &cx_global_symtab;

    if (p_main_function_id == nullptr) {
        initialize_builtin_types(&cx_global_symtab);
    }

    //initialize_std_functions(p_symtabs[0]);
}

/** Destructor	    Remove the predefined types.
 *
 */
cx_symtab_stack::~cx_symtab_stack(void) {
    remove_builtin_types();
}

/** search_all   search the symbol table stack for the given
 *              name string.
 *
 * @param p_string : ptr to name string to find.
 * @return ptr to symbol table node if found, else nullptr.
 */
cx_symtab_node *cx_symtab_stack::search_all(const char *p_string) const {
    for (int i = current_nesting_level; i >= 0; --i) {
        cx_symtab_node *p_node = p_symtabs[i]->search(p_string);
        if (p_node) return p_node;
    }

    return nullptr;
}

/** find        search the symbol table stack for the given
 *              name string.  If the name is not already in
 *              there, flag the undefined identifier error,
 *		and then enter the name into the local symbol
 *		table.
 *
 * @param p_string : ptr to name string to find.
 * @return ptr to symbol table node.
 */
cx_symtab_node *cx_symtab_stack::find(const char *p_string) const {
    cx_symtab_node *p_node = search_all(p_string);

    if (!p_node) {
        cx_error(err_undefined_identifier);
        p_node = p_symtabs[current_nesting_level]->enter(p_string);
    }

    return p_node;
}

/** enter_scope	enter a new nested scope.  Increment the nesting
 *		level.  push new scope's symbol table onto the
 *		stack.
 *
 */
void cx_symtab_stack::enter_scope(void) {

    if (++current_nesting_level > max_nesting_level) {
        cx_error(err_nesting_too_deep);
        abort_translation(abort_nesting_too_deep);
    }

    set_current_symtab(new cx_symtab);
}

/** exit_scope	Exit the current scope and return to the
 *		enclosing scope.  Decrement the nesting level.
 *		pop the closed scope's symbol table off the
 *		stack and return a pointer to it.
 *
 * @return ptr to closed scope's symbol table.
 */
cx_symtab *cx_symtab_stack::exit_scope(void) {
    return p_symtabs[current_nesting_level--];
}

/**********************
 *                    *
 *  Line Number List  *
 *                    *
 **********************/

/** Destructor      Deallocate a line number list.
 *
 */
cx_line_num_list::~cx_line_num_list(void) {
    // Loop to delete each node in the list.
    while (head) {
        cx_line_num_node *p_node = head; // ptr to node to delete
        head = head->next__; // move down the list
        delete p_node; // delete node
    }
}

/** update      update the list by appending a new line number
 *              node if the line number isn't already in the
 *              list.
 *
 */
void cx_line_num_list::update(void) {
    // If the line number is already there, it'll be at the tail.
    if (tail && (tail->number == current_line_number)) return;

    // Append the new node.
    tail->next__ = new cx_line_num_node;
    tail = tail->next__;
}

/** print       print the line number list.  Use more than one
 *              line if necessary; indent subsequent lines.
 *
 * @param new_line_flag : if true, start a new line immediately.
 * @param indent      : amount to indent subsequent lines.
 */
void cx_line_num_list::print(int new_line_flag, int indent) const {
    const int max_line_number_print_width = 4;
    const int max_line_numbers_per_line = 10;

    int n; // count of numbers per line
    cx_line_num_node *p_node; // ptr to line number node
    char *plt = &list.text[strlen(list.text)];
    // ptr to where in list text to append

    n = new_line_flag ? 0 : max_line_numbers_per_line;

    // Loop over line number nodes in the list.
    for (p_node = head; p_node; p_node = p_node->next__) {

        // Start a new list line if the current one is full.
        if (n == 0) {
            list.put_line();
            sprintf(list.text, "%*s", indent, " ");
            plt = &list.text[indent];
            n = max_line_numbers_per_line;
        }

        // Append the line number to the list text.
        sprintf(plt, "%*d", max_line_number_print_width, p_node->number);
        plt += max_line_number_print_width;
        --n;
    }

    list.put_line();
}

