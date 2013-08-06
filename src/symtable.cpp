#include <cstdio>
#include <iostream>
#include "error.h"
#include "buffer.h"
#include "symtable.h"
#include "types.h"
#include "icode.h"

//

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

        case dc_program:
        case dc_function:

            if (routine.which == rc_declared) {
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

    // Allocate and copy the symbol string.
    p_string = new char[strlen(p_str) + 1];
    strcpy(p_string, p_str);

    // If cross-referencing, update the line number list.
    if (xreference_flag) p_line_num_list = new cx_line_num_list;
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
    if (p_string != nullptr) delete[] p_string;
    if (p_line_num_list != nullptr) delete p_line_num_list;
    if (p_type != nullptr) remove_type(p_type);
}

/** print       print the symbol table node to the list file.
 *              First print the node's left__ subtree, then the
 *              node itself, and finally the node's right__
 *              subtree.  For the node itself, first print its
 *              symbol string, and then its line numbers. 
 */
void cx_symtab_node::print(void) const {
    const int max_name_print_width = 16;

    // Pirst, print left__ subtree
    if (left__) left__->print();

    // print the node:  first the name, then the list of line numbers,
    //                  and then the identifier information.
    sprintf(list.text, "%*s", max_name_print_width, p_string);
    if (p_line_num_list) {
        p_line_num_list->print(strlen(p_string) > max_name_print_width,
                max_name_print_width);
    } else list.put_line();
    print_identifier();

    // Finally, print right__ subtree
    if (right__) right__->print();
}

/** print_identifier        print information about an
 *                          identifier's definition and type.
 * 
 */
void cx_symtab_node::print_identifier(void) const {
    switch (defn.how) {
        case dc_constant: print_constant();
            break;
        case dc_type: print_type();
            break;

        case dc_variable:
        case dc_member: print_var_or_field();
            break;
    }
}

/** print_constant       print information about a constant
 *                      identifier for the cross-reference.
 * 
 */
void cx_symtab_node::print_constant(void) const {
    extern cx_list_buffer list;

    list.put_line();
    list.put_line("Defined constant");

    // value
    if ((p_type == p_integer_type) ||
            (p_type->form == fc_enum)) {
        sprintf(list.text, "value = %d",
                defn.constant.value.int__);
    } else if (p_type == p_float_type) {
        sprintf(list.text, "value = %g",
                defn.constant.value.float__);
    } else if (p_type == p_char_type) {
        sprintf(list.text, "value = '%c'",
                defn.constant.value.char__);
    } else if (p_type->form == fc_array) {
        sprintf(list.text, "value = '%s'",
                defn.constant.value.p_string);
    }
    list.put_line();

    // type information
    if (p_type) p_type->print_type_spec(cx_type::vc_terse);
    list.put_line();
}

/** print_var_or_field         print information about a variable
 *                          or record field identifier for the
 *                          cross-reference.
 * 
 */
void cx_symtab_node::print_var_or_field(void) const {
    extern cx_list_buffer list;

    list.put_line();
    list.put_line(defn.how == dc_variable ? "Declared variable"
            : "Declared record field");

    // type information
    if (p_type) p_type->print_type_spec(cx_type::vc_terse);
    if ((defn.how == dc_variable) || (this->next__)) list.put_line();
}

/** print_type           print information about a type
 *                      identifier for the cross-reference.
 * 
 */
void cx_symtab_node::print_type(void) const {
    list.put_line();
    list.put_line("Defined type");

    if (p_type) p_type->print_type_spec(cx_type::vc_verbose);
    list.put_line();
}

/** convert     convert the symbol table node into a form
 *		suitable for the back end.
 * 
 * @param p_vector_nodes : vector of node ptrs.
 */
void cx_symtab_node::convert(cx_symtab_node *p_vector_nodes[]) {
    // First, convert the left__ subtree.
    if (left__ != nullptr) left__->convert(p_vector_nodes);

    // convert the node.
    p_vector_nodes[xnode] = this;

    // Finally, convert the right__ subtree.
    if (right__ != nullptr) right__->convert(p_vector_nodes);
}

/******************
 *                *
 *  Symbol Table  *
 *                *
 ******************/

/** search      search the symbol table for the node with a
 *              given name string.
 * 
 * @param p_string : ptr to the name string to search for.
 * @return ptr to the node if found, else nullptr.
 */
cx_symtab_node *cx_symtab::search(const char *p_string) const {
    cx_symtab_node *p_node = root__; // ptr to symbol table node
    int comp;

    // Loop to search the table.
    while (p_node) {
        comp = strcmp(p_string, p_node->p_string); // compare names
        if (comp == 0) break; // found!

        // Not yet found:  next__ search left__ or right__ subtree.
        p_node = comp < 0 ? p_node->left__ : p_node->right__;
    }

    // If found and cross-referencing, update the line number list.
    if (xreference_flag && (comp == 0)) p_node->p_line_num_list->update();

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
        int comp = strcmp(p_string, p_node->p_string); // compare strings
        if (comp == 0) return p_node; // found!

        // Not yet found:  next__ search left__ or right__ subtree.
        ppNode = comp < 0 ? &(p_node->left__) : &(p_node->right__);
    }

    // Create and insert a new node.
    p_node = new cx_symtab_node(p_string, dc); // create a new node,
    p_node->xsymtab = xsymtab; // set its symtab and
    p_node->xnode = nodes_count++; // node indexes,
    *ppNode = p_node; // insert it, and
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

/** convert     convert the symbol table into a form suitable
 *		for the back end.
 * 
 * @param p_vector_symtabs : vector of symbol table pointers.
 */
void cx_symtab::convert(cx_symtab *p_vector_symtabs[]) {
    // Point the appropriate entry of the symbol table pointer vector
    // to this symbol table.
    p_vector_symtabs[xsymtab] = this;

    // Allocate the symbol table node pointer vector
    // and convert the nodes.
    p_vector_nodes = new cx_symtab_node *[nodes_count];
    root__->convert(p_vector_nodes);
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
    void initialize_std_functions(cx_symtab * p_symtab);

    current_nesting_level = 0;
    for (int i = 1; i < max_nesting_level; ++i) p_symtabs[i] = nullptr;

    // Initialize the global nesting level.
    p_symtabs[0] = &cx_global_symtab;

    initialize_builtin_types(p_symtabs[0]);

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

    // dont overwrite mains scope
    //if (current_nesting_level <= 1)++current_nesting_level;

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

