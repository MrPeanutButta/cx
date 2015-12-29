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

#ifndef SYMTABLE_H
#define	SYMTABLE_H

#include <map>
#include <cstring>
#include <string>
#include <memory>
#include <vector>
#include "types.h"
#include "cxvm.h"

namespace cx{
	namespace scoping {
		extern int current_nesting_level;
		extern const int MAX_NESTING_LEVEL;
	}

	class symbol_table_node;
	class symbol_table;

	typedef std::map<std::wstring, std::shared_ptr<symbol_table_node>> local;
	typedef std::shared_ptr<symbol_table> symbol_table_ptr;
	typedef std::shared_ptr<symbol_table_node> symbol_table_node_ptr;

	extern symbol_table_ptr p_global_symbol_table;
	extern symbol_table_ptr p_std_type_members;
	
	symbol_table_ptr new_symtab(void);

	enum define_code {
		DC_UNDEFINED, DC_CONSTANT, DC_TYPE, DC_VARIABLE, DC_MEMBER,
		DC_VALUE_PARM, DC_REFERENCE, DC_POINTER, DC_THIS,
		DC_PROGRAM, DC_FUNCTION, DC_NAMESPACE
	};

	enum function_code {
		FUNC_DECLARED, FUNC_FORWARD, FUNC_STANDARD, FUNC_STD_MEMBER, FUNC_ITERATOR
	};

	class symbol_table {
	public:
		local symbols;

		symbol_table() {}
		~symbol_table() {}

		void copy(symbol_table *p_symtab);
		void enter(symbol_table_node *p_node);
		void enter(local &params);
		symbol_table_node_ptr search(std::wstring);
		symbol_table_node_ptr enter(std::wstring, define_code dc = DC_UNDEFINED);
		symbol_table_node_ptr enter_new(std::wstring, define_code dc = DC_UNDEFINED);

		size_t node_count(void) const {return symbols.size();}
	};

	union value;
	class cxvm;
	struct inst;

	// Program instructions
	typedef std::vector<inst> program;

	class define {
	public:

		define_code defined_how;

		define() {}

		define(define_code dc) {
			defined_how = dc;
			io.stream = nullptr;
			this_ptr.p_stack_item = nullptr;
			this_ptr.is_this_ptr = false;
		}

		~define();
		
		value constant_value;

		struct {
			bool is_this_ptr;
			symbol_table_node_ptr p_node;
			value *p_stack_item;
		} this_ptr;

		struct {
			function_code function_type;
			int return_marker; // used for globals return location

			struct {
				int loop_start; // icode positions
				int loop_end;
				int current_iteration; // each indexer to determine which each call
				int postfix;
			} iterator;

			typedef cx_type *(*native_call)(cxvm *, symbol_table_node *, const cx_type *);
			native_call native_function; // extended/native function call

			std::vector<std::shared_ptr<symbol_table_node>> p_parameter_ids;
			std::vector<std::shared_ptr<symbol_table_node>> p_variable_ids;
			std::vector<std::shared_ptr<symbol_table_node>> p_type_ids;
			std::vector<std::shared_ptr<symbol_table_node>> p_function_ids;
			std::vector<std::shared_ptr<symbol_table_node>> p_constant_ids;

			symbol_table_ptr p_symtab;
			program program_code;
		} routine;

		struct {
			FILE *stream;
		} io;
	};

	class symbol_table_node {

		friend class symbol_table;
	private:
		int level;
	public:

		cx_type::type_ptr p_type;
		std::wstring node_name;
		define defined;


		// pointer to runstack item
		value *runstack_item;
		symbol_table_node(std::wstring name, define_code dc = DC_UNDEFINED);
		~symbol_table_node();
	};

	///  symbol_table_stack      Symbol table stack class.

	class symbol_table_stack {

		symbol_table *p_symtabs[512];

	public:
		symbol_table_stack(void);
		~symbol_table_stack(void);

		symbol_table_node_ptr search_local(std::wstring name) {
			return p_symtabs[scoping::current_nesting_level]->search(name);
		}

		symbol_table_node_ptr enter_local(std::wstring name,
			define_code dc = DC_UNDEFINED) {
			return p_symtabs[scoping::current_nesting_level]->enter(name, dc);
		}

		symbol_table_node_ptr enter_new_local(std::wstring name,
			define_code dc = DC_UNDEFINED) {
			return p_symtabs[scoping::current_nesting_level]->enter_new(name, dc);
		}

		symbol_table *get_current_symtab(void) const {
			return p_symtabs[scoping::current_nesting_level];
		}

		void set_current_symtab(symbol_table *p_symtab) {
			p_symtabs[scoping::current_nesting_level] = p_symtab;
		}

		void set_scope(int scopeLevel) {
			scoping::current_nesting_level = scopeLevel;
		}

		symbol_table_node_ptr search_available_scopes(std::wstring name) const;
		symbol_table_node_ptr search_all(std::wstring name) const;
		symbol_table_node_ptr find(std::wstring name) const;
		void enter_scope(void);
		symbol_table *exit_scope(void);
	};

	// mains scope level on the symtab stack
#define __MAIN_ENTRY__ symtab_stack.find(L"__main__")
}
#endif	/* SYMTABLE_H */

