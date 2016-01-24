/*
The MIT License (MIT)

Copyright (c) 2015 Aaron Hebert <aaron.hebert@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include <utility>
#include <memory>
#include "error.h"
#include "symtab.h"
#include "types.h"

namespace cx{
	namespace scoping {
		int current_nesting_level = 0;
		const int MAX_NESTING_LEVEL = 512;
	}

   /** Global symbols
	*
	* NOTE:
	*      when writing the Cx stdlib, this global symtab should be visible
	*      to the dynamic library to allow a library to add it's own symbols. - aaron
	*/
	symbol_table_ptr p_global_symbol_table = std::make_shared<symbol_table>();
	symbol_table_ptr p_std_type_members;

	symbol_table_ptr new_symtab(void){
		return std::make_shared<symbol_table>();
	}

	/** Destructor      Delete the local symbol table and icode of a
	 *                  program, procedure or function definition.
	 *                  Note that the parameter and local identifier
	 *                  chains are deleted along with the local
	 *                  symbol table.
	 *
	 */
	define::~define(void) {}

	/** Constructor     Construct a symbol table node by initial-
	 *                  izing its subtree pointers and the pointer
	 *                  to its symbol string.
	 *
	 * @param p_str : ptr to the symbol string.
	 * @param dc    : definition code.
	 */
	symbol_table_node::symbol_table_node(std::wstring name, define_code dc)
		: node_name(name) {
		this->level = scoping::current_nesting_level;
		this->defined.defined_how = dc;
		this->runstack_item = nullptr;
	}

	//symbol_table_node::symbol_table_node() {}

	/** Destructor      Deallocate a symbol table node.
	 *
	 */
	symbol_table_node::~symbol_table_node(void) {}

	/** search      search the symbol table for the node with a
	 *              given name string.
	 *
	 * @param p_string : ptr to the name string to search for.
	 * @return ptr to the node if found, else nullptr.
	 */
	//symbol_table_node_ptr return_null;
	symbol_table_node_ptr symbol_table::search(std::wstring name) {
		//for (auto &node : this->symbols){
		//	if (node.first == name) return node.second;
		//}

		auto p_node = this->symbols.find(name);
		if (p_node == this->symbols.end()) return nullptr;

		return p_node->second;
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
	symbol_table_node_ptr symbol_table::enter(std::wstring name, define_code dc) {
		auto p_node = symbols.find(name);

		if (p_node == symbols.end()) {
			return (symbols.insert(std::make_pair(name, std::make_shared<symbol_table_node>(name, dc))))->second;
		}

		return p_node->second; // return a ptr to it
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
	symbol_table_node_ptr symbol_table::enter_new(std::wstring name, define_code dc) {
		auto p_node = symbols.find(name);

		if (p_node == symbols.end()) return enter(name, dc);
		else cx_error(ERR_REDEFINED_IDENTIFIER);

		return p_node->second;
	}

	void symbol_table::enter_new(symbol_table_node_ptr &p_new_id) {
		auto p_node = symbols.find(p_new_id->node_name);

		if (p_node == symbols.end()) enter(p_new_id);
		else cx_error(ERR_REDEFINED_IDENTIFIER);
	}

	void symbol_table::enter_new_function(symbol_table_node_ptr &p_new_id) {
		enter(p_new_id);
	}

	std::pair<local::iterator, local::iterator> symbol_table::find_functions(std::wstring name) {
		return symbols.equal_range(name);
	}

	void symbol_table::copy(symbol_table *p_symtab){
		this->symbols.insert(p_symtab->symbols.begin(), p_symtab->symbols.end());
	}

	/*void symbol_table::enter(symbol_table_node *p_node){
		this->symbols[p_node->node_name].reset(p_node);// this->node_pool.back().get();
	}*/

	void symbol_table::enter(symbol_table_node_ptr &p_new_id) {
		this->symbols.insert(std::make_pair(p_new_id->node_name, p_new_id));
	}

	void symbol_table::enter(local &params){
		this->symbols.insert(params.begin(), params.end());
	}


	/** Constructor	    Initialize the global (level 0) symbol
	 *		    table, and set the others to nullptr.
	 *
	 */
	symbol_table_stack::symbol_table_stack(void) {

		void initialize_std_functions(symbol_table *p_symtab);

		scoping::current_nesting_level = 0;
		//for (int i = 1; i < MAX_NESTING_LEVEL; ++i) p_symtabs[i] = nullptr;

		// Initialize the global nesting level.
		p_symtabs[0] = p_global_symbol_table.get();//emplace_back(p_global_symbol_table);

//		if (p_main_function_id == nullptr) {

			// TODO: initialize_builtin_types move this to another file.
			initialize_builtin_types(p_global_symbol_table);
	//	}
	}

	/** Destructor	    Remove the predefined types.
	 *
	 */
	symbol_table_stack::~symbol_table_stack(void) {}

	/** search_all   search the symbol table stack for the given
	 *              name string.
	 *
	 * @param p_string : ptr to name string to find.
	 * @return ptr to symbol table node if found, else nullptr.
	 */
	symbol_table_node_ptr symbol_table_stack::search_all(std::wstring name) {
		
		symbol_table_node_ptr p_node;

		for (int i = scoping::current_nesting_level; i >= 0; --i) {
			p_node = p_symtabs[i]->search(name);
			if (p_node != nullptr) return p_node;
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
	symbol_table_node_ptr symbol_table_stack::find(std::wstring name) {

		symbol_table_node_ptr p_node = search_all(name);

		if (p_node == nullptr) {
			cx_error(ERR_REDEFINED_IDENTIFIER);
		}

		return p_node;
	}

	/** enter_scope	enter a new nested scope.  Increment the nesting
	 *		level.  push new scope's symbol table onto the
	 *		stack.
	 *
	 */
	void symbol_table_stack::enter_scope(void) {

		if (++scoping::current_nesting_level >= scoping::MAX_NESTING_LEVEL) {
			cx_error(ERR_NESTING_TOO_DEEP);
			abort_translation(ABORT_NESTING_TOO_DEEP);
		}

		//symbol_table_ptr p_sym = std::make_shared<symbol_table>();
		set_current_symtab(new symbol_table());
	}

	/** exit_scope	Exit the current scope and return to the
	 *		enclosing scope.  Decrement the nesting level.
	 *		pop the closed scope's symbol table off the
	 *		stack and return a pointer to it.
	 *
	 * @return ptr to closed scope's symbol table.
	 */
	symbol_table *symbol_table_stack::exit_scope(void) {
		return p_symtabs[scoping::current_nesting_level--];
	}
}
