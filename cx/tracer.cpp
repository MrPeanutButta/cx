#include <cstdio>
#include <cstring>
#include <iostream>
#include <sstream>
#include "common.h"
#include "buffer.h"

namespace cx{
	namespace tracer{
		/** trace_routine_entry   Trace the entry into a routine.
		 *
		 * @param p_function_id : ptr to the routine name's symbol table node.
		 */
		void cx_executor::trace_routine_entry(const cx_symtab_node *p_function_id) {
			if (trace_routine_flag) {
				std::cout << ">> Entering routine " << p_function_id->string__() << std::endl;
			}
		}

		/** trace_routine_exit    Trace the exit from a routine.
		 *
		 * @param p_function_id : ptr to the routine name's symbol table node
		 */
		void cx_executor::trace_routine_exit(const cx_symtab_node *p_function_id) {
			if (trace_routine_flag) {
				std::cout << ">> Exiting routine " << p_function_id->string__() << std::endl;
			}
		}

		/** trace_statement      Trace the execution of a statement.
		 */
		void cx_executor::trace_statement(void) {
			if (trace_statement_flag) std::cout << ">>  At " << current_line_number
				<< std::endl;
		}

		/** trace_data_store      Trace the storing of data into a
		 *                      variable or formal parameter.
		 *
		 * @param p_target_id  : ptr to the target name's symbol table node.
		 * @param p_data_value : ptr to the data value.
		 * @param p_data_type  : ptr to the data's type object.
		 */
		void cx_executor::trace_data_store(const cx_symtab_node *p_target_id,
			const cx_stack_item &p_data_value,
			const cx_type *p_data_type) {
			if (trace_store_flag) {
				cx_type_form_code form = p_target_id->p_type->form;

				std::cout << ">>   " << p_target_id->string__();
				if (form == fc_array) std::cout << "[*]";
				else if (form == fc_complex) std::cout << ".*";
				std::cout << " <== ";

				trace_data_value(p_data_value, p_data_type);
			}
		}

		/** trace_data_fetch      Trace the fetching of data from a
		 *                      variable or formal parameter.
		 *
		 * @param p_id        : ptr to the variable name's symbol table node.
		 * @param p_data_value : ptr to the data value.
		 * @param p_data_type  : ptr to the data's type object.
		 */
		void cx_executor::trace_data_fetch(const cx_symtab_node *p_id,
			const cx_stack_item &p_data_value,
			const cx_type *p_data_type) {
			if (trace_fetch_flag) {
				cx_type_form_code form = p_id->p_type->form;

				std::cout << ">>   " << p_id->string__();
				if (form == fc_array) std::cout << "[*]";
				else if (form == fc_complex) std::cout << ".*";
				std::cout << ": ";

				trace_data_value(p_data_value, p_data_type);
			}
		}

		/** trace_data_value      Trace a data value.
		 *
		 * @param p_data_value : ptr to the data value.
		 * @param p_data_type  : ptr to the data's type object.
		 */
		void cx_executor::trace_data_value(const cx_stack_item &p_data_value,
			const cx_type *p_data_type) {

			if (p_data_type->form == fc_stream) return;

			std::stringstream text;

			if (p_data_type == p_float_type) {
				text << p_data_value.basic_types.float__;
			}
			else if (p_data_type == p_char_type) {
				text << '\'' << p_data_value.basic_types.char__ << '\'';
			}
			else if (p_data_type == p_integer_type) {
				text << p_data_value.basic_types.int__;
			}
			else if (p_data_type == p_wchar_type) {
				text << '\'' << p_data_value.basic_types.wchar__ << '\'';
			}
			else if (p_data_type == p_uint8_type) {
				text << std::oct << (unsigned short)p_data_value.basic_types.uint8__;
			}
			else if (p_data_type == p_boolean_type) {
				text << std::boolalpha << p_data_value.basic_types.bool__;
			}
			else if (p_data_type->form == fc_array) {
				if (p_data_type->array.p_element_type == p_char_type) {
					if (p_data_value.basic_types.addr__ != nullptr) {
						text << '\"' << (char *)p_data_value.basic_types.addr__ << '\"';
					}
					else {
						text << "<null>";
					}
				}
				else text << "<array>";
			}
			else if (p_data_type->form == fc_complex) {
				text << "<complex>";
			}
			else if (p_data_type->base_type()->form == fc_enum) {
				int count = p_data_value.basic_types.int__;
				cx_symtab_node *p_id = p_data_type->base_type()->enumeration.p_const_ids;
				while (--count >= 0) p_id = p_id->next__;
				text << p_id->string__();
			}
			else {
				text << p_data_value.basic_types.addr__;
			}

			std::cout << text.str() << std::endl;
		}
	}
}