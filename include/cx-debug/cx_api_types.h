#include "types.h"

// Pointers to predefined types.
cx_symtab_node *p_main_function_id = nullptr;
cx_symtab_node *p_stdin = nullptr;
cx_symtab_node *p_stdout = nullptr;
cx_symtab_node *p_stderr = nullptr;
cx_type *p_integer_type = nullptr;
cx_type *p_uint8_type = nullptr;
cx_type *p_float_type = nullptr;
cx_type *p_boolean_type = nullptr;
cx_type *p_char_type = nullptr;
cx_type *p_wchar_type = nullptr;
//cx_type *p_class_type = nullptr;
cx_type *p_complex_type = nullptr;
cx_type *p_file_type = nullptr;
cx_type *p_dummy_type = nullptr;
cx_type *p_void_type = nullptr;

/** set_type     Set the target type.  Increment the reference
*              count of the source type.
*
* @param p_target_type : ref to ptr to target type object.
* @param p_source_type : ptr to source type object.
* @return ptr to source type object.
*/
cx_type *set_type(cx_type *&p_target_type, cx_type *p_source_type) {
	if (!p_target_type) remove_type(p_target_type);

	++p_source_type->reference_count;

	p_target_type = p_source_type;

	return p_source_type;
}

/** remove_type  Decrement a type object's reference count, and
*              delete the object and set its pointer to nullptr
*              if the count becomes 0.
*
* @param p_type : ref to ptr to type object.
*/
void remove_type(cx_type *&p_type) {
	if (p_type == nullptr) return;

	if (--p_type->reference_count == 0) {

		delete p_type;
		p_type = nullptr;

	}
}