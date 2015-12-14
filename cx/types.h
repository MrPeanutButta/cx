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

#ifndef TYPES_H
#define	TYPES_H

#include <cstdio>
#include <vector>
#include <thread>
#include <mutex>
#include <map>
#include <memory>
#include "error.h"

namespace cx {

	typedef long long cx_int;
	typedef long double cx_real;
	typedef bool cx_bool;
	typedef wchar_t cx_char;
	typedef uint8_t cx_byte;

	enum type_code {
		T_BOOLEAN = 0x00,
		T_CHAR = 0x01,
		T_BYTE = 0x02,
		T_INT = 0x03,
		T_DOUBLE = 0x04,
		T_REFERENCE = 0x05,
		T_VOID = 0xA,
		T_DUMMY = 0xF
	};

	enum type_form {
		F_SCALAR,
		F_ENUM,
		F_ARRAY,
		F_REFERENCE,
		F_STREAM,
		F_NONE
	};

	typedef union value {
		cx_bool z_;
		cx_byte b_;
		cx_char c_;
		//cx_wchar w_;
		cx_int i_;
		cx_real d_;
		void *a_;

		value(cx_bool z) : z_(z) {}
		value(cx_byte b) : b_(b) {}
		value(cx_char c) : c_(c) {}
		value(cx_int i) : i_(i) {}
		value(int i) : i_(i) {}
		value(cx_real d) : d_(d) {}
		value(void *a) : a_(a) {}
		value() : a_(nullptr) {}

		std::mutex &lock(bool unlock = false) {
			// mutex not allowed in union, so static here
			static std::mutex _lock;

			// I called from unlock(), return our _lock
			if (unlock) return _lock;

			// Sleep while acquiring lock
			while (!_lock.try_lock()) {
				this->nano_sleep();
			}

			return *(std::mutex *)nullptr;
		}

		void nano_sleep(int nano_secs = 5) {
			std::this_thread::sleep_for(
				std::chrono::nanoseconds(nano_secs)
				);
		}

		inline void unlock(void) {
			this->lock(true).unlock();
		}
	} value;

	class symbol_table;
	class symbol_table_node;
	typedef std::shared_ptr<symbol_table> symbol_table_ptr;
	typedef std::shared_ptr<symbol_table_node> symbol_table_node_ptr;

	class cx_type {
		bool is_constant_;

	public:
		typedef std::shared_ptr <cx_type> type_ptr;			// Smart type pointer

		/* Mapped raw pointer to type data. Gets passed to library
		 * to allow the lib to decalre cx datatypes and functions */
		typedef std::map<type_code, cx_type *> cx_datatype;

		size_t size;					// The size of this type
		symbol_table_node_ptr p_type_id;	// TODO should be symbol_table_node_ptr
		type_code typecode;
		type_form typeform;

		symbol_table_ptr p_enum_ids; // TODO should be symbol_table_node_ptr

		struct {
			type_ptr p_index_type;
			type_ptr p_element_type;
			size_t min_index;
			size_t max_index;
			size_t element_count;

			type_ptr next;
		} array;

		struct {
			/* used only for internal to class.
			 * connects all scopes to a single table */
			symbol_table_ptr p_class_scope;
		} complex;

		cx_type(type_form form, type_code type, size_t size, symbol_table_node_ptr &p_id, symbol_table_ptr &p_members);
		cx_type(type_form form, type_code type);
		cx_type() {}

		~cx_type();

		bool is_scalar_type(void) const {
			return (typeform != F_ARRAY) &&
				(typeform != F_REFERENCE) &&
				(typeform != F_STREAM);
		}

		bool is_constant(void) const {
			return is_constant_;
		}

		const cx_type *base_type(void) const {
			return typeform == F_ARRAY ? array.p_element_type.get() : this;
		}

		friend void check_relational_op_operands(symbol_table_node_ptr &p_function_id, const type_ptr p_type1, const type_ptr p_type2);
		friend void check_integer_or_real(const type_ptr p_type1, const type_ptr p_type2);
		friend void check_boolean(const type_ptr p_type1, const type_ptr p_type2);
		friend void check_assignment_type_compatible(symbol_table_node_ptr &p_function_id, const type_ptr p_target_type, const type_ptr p_value_type, error_code ec);
		friend bool integer_operands(const type_ptr p_type1, const type_ptr p_type2);
		friend bool real_operands(const type_ptr p_type1, const type_ptr p_type2);

		friend void check_bitwise_integer(const type_ptr p_type1);
		friend void check_bitwise_integer(const type_ptr p_type1, const type_ptr p_type2);
	};

	void initialize_builtin_types(symbol_table_ptr &p_symtab);

	extern cx_type::type_ptr p_boolean_type;
	extern cx_type::type_ptr p_char_type;
	extern cx_type::type_ptr p_byte_type;
	//extern cx_type::type_ptr p_wchar_type;
	extern cx_type::type_ptr p_integer_type;
	extern cx_type::type_ptr p_double_type;
	extern cx_type::type_ptr p_reference_type;
	extern cx_type::type_ptr p_void_type;
	extern cx_type::type_ptr p_dummy_type;


}
#endif	/* TYPES_H */

