#include <utility>
#include <memory>
#include "error.h"
//#include "buffer.h"
#include "symtab.h"
#include "types.h"

namespace cx{

	//int asm_label_index = 0; // assembly label index
	bool xreference_flag = false; // true = cross-referencing on, false = off

	// current scope level
	int current_nesting_level = 0;
	const int MAX_NESTING_LEVEL = 512;
	//symbol_table_node_ptr p_main_function_id;

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

		this->global_finish_location = 0;
		this->found_global_end = false;
		this->level = current_nesting_level;
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
	symbol_table_node_ptr symbol_table::search(std::wstring name) {
		for (auto &node : this->symbols){
			if (node.first == name) return node.second;
		}

		return nullptr;
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
		symbol_table_node_ptr p_node = symbols[name];

		if (p_node == nullptr) {
			return (symbols[name] = std::make_shared<symbol_table_node>(name, dc));
		}

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
	symbol_table_node_ptr symbol_table::enter_new(std::wstring name, define_code dc) {
		symbol_table_node_ptr p_node = symbols[name];

		if (p_node == nullptr) p_node = enter(name, dc);
		else cx_error(ERR_REDEFINED_IDENTIFIER);

		return p_node;
	}

	void symbol_table::copy(symbol_table *p_symtab){
		this->symbols.insert(p_symtab->symbols.begin(), p_symtab->symbols.end());
	}

	void symbol_table::enter(symbol_table_node *p_node){
		this->symbols[p_node->node_name] =  std::move(std::shared_ptr<symbol_table_node>(p_node));// this->node_pool.back().get();
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

		current_nesting_level = 0;
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
	symbol_table_node_ptr symbol_table_stack::search_all(std::wstring name) const {
		
		symbol_table_node_ptr p_node;

		for (int i = current_nesting_level; i >= 0; --i) {
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
	symbol_table_node_ptr symbol_table_stack::find(std::wstring name) const {

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

		if (++current_nesting_level > MAX_NESTING_LEVEL) {
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
		return p_symtabs[current_nesting_level--];
	}
}
