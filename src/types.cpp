
#include <cstdio>
#include "buffer.h"
#include "error.h"
#include "types.h"

const char *form_strings[] = {
    "*error*", "scalar", "enum", "subrange", "array", "complex", "pointer"
};

// Pointers to predefined types.
cx_symtab_node *p_main_function_id = nullptr;
cx_symtab_node *p_stdin = nullptr;
cx_symtab_node *p_stdout = nullptr;
cx_symtab_node *p_stderr = nullptr;

cx_type *p_integer_type = nullptr;
cx_type *p_short_type = nullptr;
cx_type *p_long_type = nullptr;

cx_type *p_uint8_type = nullptr;
cx_type *p_uint16_type = nullptr;
cx_type *p_uint32_type = nullptr;
cx_type *p_uint64_type = nullptr;

cx_type *p_float_type = nullptr;
cx_type *p_double_type = nullptr;

cx_type *p_boolean_type = nullptr;

cx_type *p_char_type = nullptr;
cx_type *p_wchar_type = nullptr;

//cx_type *p_class_type = nullptr;

cx_type *p_complex_type = nullptr;

cx_type *p_file_type = nullptr;

cx_type *p_dummy_type = nullptr;

/** Constructors    General.
 *
 * @param fc  : form code.
 * @param s   : byte size of type.
 * @param p_id : ptr to symbol table node of type identifier.
 */
cx_type::cx_type(cx_type_form_code fc, int s, cx_symtab_node* p_id)
: form(fc), size(s), p_type_id(p_id), reference_count(0) {

    switch (fc) {
        case fc_array:
            this->size = s;
            this->form = fc_array;
            array.p_index_type = array.p_element_type = nullptr;
            break;
        default:
            break;
    }

    std::string type_name = "dummy";
    
    if (p_type_id != nullptr)
        type_name = p_type_id->string__();

    if (type_name == "short") {
        type_code = cx_short;
    } else if (type_name == "int") {
        type_code = cx_int;
    } else if (type_name == "char") {
        type_code = cx_char;
    } else if (type_name == "wchar") {
        type_code = cx_wchar;
    } else if (type_name == "long") {
        type_code = cx_long;
    } else if (type_name == "float") {
        type_code = cx_float;
    } else if (type_name == "double") {
        type_code = cx_double;
    } else if (type_name == "bool") {
        type_code = cx_bool;
    } else if (type_name == "uint8") {
        type_code = cx_uint8;
    } else if (type_name == "uint16") {
        type_code = cx_uint16;
    } else if (type_name == "uint32") {
        type_code = cx_uint32;
    } else if (type_name == "uint64") {
        type_code = cx_uint64;
    } else if (type_name == "class") {
        type_code = cx_complex;
    } else if (type_name == "file") {
        type_code = cx_file;
    }
}

cx_type::cx_type(int length, bool constant)
: size(length), form(fc_array), reference_count(0), is_constant__(constant) {
    p_type_id = nullptr;

    // used for string constants only. can probably go away
    array.p_index_type = array.p_element_type = nullptr;
    set_type(array.p_index_type, p_integer_type);
    set_type(array.p_element_type, p_char_type);
    array.element_count = length;

}

/** Destructor      Delete the allocated objects according to
 *                  the form code.  Note that the objects
 *                  pointed to by enumeration.p_const_ids and by
 *                  subrange.p_base_type are deleted along with
 *                  the symbol tables that contain their
 *                  identifiers.
 */
cx_type::~cx_type() {
    switch (form) {
            /*  case fc_subrange:
                  remove_type(subrange.p_base_type);
                  break;*/
        case fc_array:
            remove_type(array.p_index_type);
            remove_type(array.p_element_type);
            break;
        case fc_complex:
            // delete complex.pSymtabPublic;
            break;

        default:
            break;
    }
}

/** print_type_spec       print information about a type
 *                      specification for the cross-reference.
 *
 * @param vc : vc_verbose or vc_terse to control the output.
 */
void cx_type::print_type_spec(cx_verbosity_code vc) {
    sprintf(list.text, "%s, size %d bytes. type id: ", form_strings[form], size);

    if (p_type_id) strcat(list.text, p_type_id->string__());
    else {
        strcat(list.text, "<unnamed>");
        vc = vc_verbose;
    }

    list.put_line();

    switch (form) {
        case fc_enum: print_enum_type(vc);
            break;
        case fc_subrange: print_subrange_type(vc);
            break;
        case fc_array: print_array_type(vc);
            break;
        case fc_complex: print_record_type(vc);
            break;
        default:
            break;
    }
}

/** print_enum_type       print information about an enumeration
 *                      type for the cross-reference.
 *
 * @param vc : vc_verbose or vc_terse to control the output.
 */
void cx_type::print_enum_type(cx_verbosity_code vc) const {
    if (vc == vc_terse) return;

    list.put_line("---enum constant identifiers (value = name)---");

    for (cx_symtab_node *p_const_id = enumeration.p_const_ids;
            p_const_id; p_const_id = p_const_id->next__) {
        sprintf(list.text, "%d = %s",
                p_const_id->defn.constant.value.int__,
                p_const_id->string__());

        list.put_line();
    }
}

/** print_subrange_type   print information about a subrange
 *                      type for the cross-reference.
 *
 * @param vc : vc_verbose or vc_terse to control the output.
 */
void cx_type::print_subrange_type(cx_verbosity_code vc) const {
    if (vc == vc_terse) return;
    /*
        sprintf(list.text, "min value = %d, max value = %d",
                subrange.min, subrange.max);

        list.put_line();

        if (subrange.p_base_type) {
            list.put_line("---base type---");
            subrange.p_base_type->print_type_spec(vc_terse);
        }*/
}

/** print_array_type      print information about an array
 *                      type for the cross-reference.
 *
 * @param vc : vc_verbose or vc_terse to control the output.
 */
void cx_type::print_array_type(cx_verbosity_code vc) const {
    if (vc == vc_terse) return;

    sprintf(list.text, "%d elements", array.element_count);
    list.put_line();

    if (array.p_index_type) {
        list.put_line("---index type---");
        array.p_index_type->print_type_spec(vc_terse);
    }

    if (array.p_element_type) {
        list.put_line("---element type---");
        array.p_element_type->print_type_spec(vc_terse);
    }
}

/** print_record_type     print information about a record
 *                      type for the cross-reference.
 *
 * @param vc : vc_verbose or vc_terse to control the output.
 */
void cx_type::print_record_type(cx_verbosity_code vc) {
    if (vc == vc_terse) return;

    list.put_line("member identifiers (offset : name)---");
    list.put_line();

    //    if (complex.MemberTable.empty()) {
    //        list.put_line("empty type");
    //        return;
    //    }

    //    for (cx_symtab_node *p_field_id = complex.p_class_scope_symtab->root();
    //            p_field_id; p_field_id = p_field_id->next__) {
    //        sprintf(list.text, "\t%d : %s",
    //                p_field_id->defn.data.offset,
    //                p_field_id->string__());
    //        list.put_line();
    //        p_field_id->print_var_or_field();
    //    }
    //
    //    list.put_line("public:\n");
    //
    //    for (cx_symtab_node *p_field_id = complex.MemberTable[tc_PUBLIC]->root();
    //            p_field_id; p_field_id = p_field_id->next__) {
    //        sprintf(list.text, "\t%d : %s",
    //                p_field_id->defn.data.offset,
    //                p_field_id->string__());
    //        list.put_line();
    //        p_field_id->print_var_or_field();
    //    }
    //
    //    list.put_line();
    //    list.put_line("private:\n");
    //
    //    if (!complex.MemberTable[tc_PRIVATE]) {
    //        list.put_line("empty scope");
    //    } else {
    //
    //        for (cx_symtab_node *p_field_id = complex.MemberTable[tc_PRIVATE]->root();
    //                p_field_id; p_field_id = p_field_id->next__) {
    //            sprintf(list.text, "\t%d : %s",
    //                    p_field_id->defn.data.offset,
    //                    p_field_id->string__());
    //            list.put_line();
    //            p_field_id->print_var_or_field();
    //        }
    //    }
    //
    //    list.put_line();
    //    list.put_line("protected:\n");
    //
    //    if (!complex.MemberTable[tc_PROTECTED]) {
    //        list.put_line("empty scope");
    //    } else {
    //        for (cx_symtab_node *p_field_id = complex.MemberTable[tc_PROTECTED]->root();
    //                p_field_id; p_field_id = p_field_id->next__) {
    //            sprintf(list.text, "\t%d : %s",
    //                    p_field_id->defn.data.offset,
    //                    p_field_id->string__());
    //            list.put_line();
    //            p_field_id->print_var_or_field();
    //        }
    //    }
}

/** initialize_builtin_types   Initialize the predefined
 *                              types by entering their
 *                              identifiers into the symbol
 *                              table.
 *
 * @param p_symtab : ptr to symbol table.
 */
void initialize_builtin_types(cx_symtab *p_symtab) {

    p_main_function_id = p_symtab->enter("main", dc_function);
    p_main_function_id->defn.routine.which = rc_forward;

    // signed int
    cx_symtab_node *p_integer_id = p_symtab->enter("int", dc_type);
    cx_symtab_node *p_short_id = p_symtab->enter("short", dc_type);
    cx_symtab_node *p_long_id = p_symtab->enter("long", dc_type);

    // unsigned int
    cx_symtab_node *p_uint8_id = p_symtab->enter("uint8", dc_type);
    cx_symtab_node *p_uint16_id = p_symtab->enter("uint16", dc_type);
    cx_symtab_node *p_uint32_id = p_symtab->enter("uint32", dc_type);
    cx_symtab_node *p_uint64_id = p_symtab->enter("uint64", dc_type);

    cx_symtab_node *p_float_id = p_symtab->enter("float", dc_type);
    cx_symtab_node *p_double_id = p_symtab->enter("double", dc_type);

    cx_symtab_node *p_complex_id = p_symtab->enter("class", dc_type);

    cx_symtab_node *p_boolean_id = p_symtab->enter("bool", dc_type);

    cx_symtab_node *p_char_id = p_symtab->enter("char", dc_type);
    cx_symtab_node *p_wchar_id = p_symtab->enter("wchar", dc_type);

    cx_symtab_node *p_false_id = p_symtab->enter("false", dc_constant);
    cx_symtab_node *p_true_id = p_symtab->enter("true", dc_constant);

    cx_symtab_node *p_fileId = p_symtab->enter("file", dc_type);

    if (!p_integer_type) {
        set_type(p_integer_type, new cx_type(fc_scalar, sizeof (int), p_integer_id));
    }

    if (!p_short_type) {
        set_type(p_short_type, new cx_type(fc_scalar, sizeof (short), p_short_id));
    }

    if (!p_long_type) {
        set_type(p_long_type, new cx_type(fc_scalar, sizeof (long), p_long_id));
    }

    if (!p_uint8_type) {
        set_type(p_uint8_type, new cx_type(fc_scalar, sizeof (uint8_t), p_uint8_id));
    }

    if (!p_uint16_type) {
        set_type(p_uint16_type, new cx_type(fc_scalar, sizeof (uint16_t), p_uint16_id));
    }

    if (!p_uint32_type) {
        set_type(p_uint32_type, new cx_type(fc_scalar, sizeof (uint32_t), p_uint32_id));
    }

    if (!p_uint64_type) {
        set_type(p_uint64_type, new cx_type(fc_scalar, sizeof (uint64_t), p_uint64_id));
    }

    if (!p_float_type) {
        set_type(p_float_type, new cx_type(fc_scalar, sizeof (float), p_float_id));
    }

    if (!p_double_type) {
        set_type(p_double_type, new cx_type(fc_scalar, sizeof (double), p_double_id));
    }

    if (!p_boolean_type) {
        set_type(p_boolean_type, new cx_type(fc_enum, sizeof (bool), p_boolean_id));
    }

    if (!p_char_type) {
        set_type(p_char_type, new cx_type(fc_scalar, sizeof (char), p_char_id));
    }

    if (!p_wchar_type) {
        set_type(p_wchar_type, new cx_type(fc_scalar, sizeof (wchar_t), p_wchar_id));
    }

    if (!p_complex_type) {
        set_type(p_complex_type, new cx_type(fc_complex, sizeof (cx_type), p_complex_id));
    }

    if (!p_file_type) {
        set_type(p_file_type, new cx_type(fc_stream, sizeof (FILE), p_fileId));
    }

    set_type(p_main_function_id->p_type, p_integer_type);

    // link each predefined type id's node to it's type object
    set_type(p_integer_id->p_type, p_integer_type);
    set_type(p_short_id->p_type, p_short_type);
    set_type(p_long_id->p_type, p_long_type);

    set_type(p_uint8_id->p_type, p_uint8_type);
    set_type(p_uint16_id->p_type, p_uint16_type);
    set_type(p_uint32_id->p_type, p_uint32_type);
    set_type(p_uint64_id->p_type, p_uint64_type);

    set_type(p_float_id->p_type, p_float_type);
    set_type(p_double_id->p_type, p_double_type);

    set_type(p_boolean_id->p_type, p_boolean_type);

    set_type(p_char_id->p_type, p_char_type);
    set_type(p_wchar_id->p_type, p_wchar_type);

    set_type(p_complex_id->p_type, p_complex_type);

    set_type(p_fileId->p_type, p_file_type);

    p_boolean_type->enumeration.max = 1;
    p_boolean_type->enumeration.p_const_ids = p_false_id;

    p_false_id->defn.constant.value.int__ = 0;
    p_true_id->defn.constant.value.int__ = 1;

    set_type(p_true_id->p_type, p_boolean_type);
    set_type(p_false_id->p_type, p_boolean_type);

    p_false_id->next__ = p_true_id;

    p_stdout = p_symtab->enter("stdout", ::dc_variable);
    set_type(p_stdout->p_type, p_file_type);
    p_stdout->p_type->stream.p_file_stream = stdout;

    p_stdin = p_symtab->enter("stdin", ::dc_variable);
    set_type(p_stdin->p_type, p_file_type);
    p_stdin->p_type->stream.p_file_stream = stdin;

    p_stderr = p_symtab->enter("stderr", ::dc_variable);
    set_type(p_stderr->p_type, p_file_type);
    p_stderr->p_type->stream.p_file_stream = stderr;

    set_type(p_dummy_type, new cx_type(fc_none, 1, nullptr));
}

/** remove_builtin_types       Remove the predefined types.
 */
void remove_builtin_types(void) {
    remove_type(p_complex_type);

    remove_type(p_float_type);
    remove_type(p_double_type);

    remove_type(p_boolean_type);

    remove_type(p_char_type);
    remove_type(p_wchar_type);

    remove_type(p_short_type);
    remove_type(p_integer_type);
    remove_type(p_long_type);

    remove_type(p_uint8_type);
    remove_type(p_uint16_type);
    remove_type(p_uint32_type);
    remove_type(p_uint64_type);

    remove_type(p_dummy_type);
    remove_type(p_file_type);
}

void remove_type(cx_type *&p_type);

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

/************************
 *                      *
 *  type Compatibility  *
 *                      *
 ************************/

/** check_relational_op_operands  Check that the types of the two operands
 *                      of a relational operator are compatible.
 *                      Flag an incompatible type error if not.
 *
 * @param p_type1 : ptr to the first  operand's type object.
 * @param p_type2 : ptr to the second operand's type object.
 */
void check_relational_op_operands(const cx_type *p_type1, const cx_type *p_type2) {
    p_type1 = p_type1->base_type();
    p_type2 = p_type2->base_type();

    if ((p_type1 == p_type2) &&
            ((p_type1->form == fc_scalar) || (p_type1->form == fc_enum))) {
        return;
    }

    if (((p_type1 == p_integer_type) && (p_type2 == p_float_type))
            || ((p_type2 == p_integer_type) && (p_type2 == p_float_type))) {
        return;
    }

    if ((p_type1 == p_char_type) && (p_type2 == p_integer_type) &&
            (p_type1->form == fc_scalar)) return;

    if ((p_type1 == p_integer_type) && (p_type2 == p_char_type) &&
            (p_type2->form == fc_scalar)) return;

    if ((p_type1->form == fc_array)
            && (p_type2->form == fc_array)
            && (p_type1->array.p_element_type == p_char_type)
            && (p_type2->array.p_element_type == p_char_type)
            && (p_type1->array.element_count == p_type2->array.element_count)) {
        return;
    }

    cx_error(err_incompatible_types);
}

/** check_integer_or_real  Check that the type of each operand is
 *                      either integer or real.  Flag an
 *                      incompatible type error if not.
 *
 * @param p_type1 : ptr to the first  operand's type object.
 * @param p_type2 : ptr to the second operand's type object or nullptr.
 */
void check_integer_or_real(const cx_type *p_type1, const cx_type *p_type2) {
    p_type1 = p_type1->base_type();

    if ((p_type1 != p_integer_type) && (p_type1 != p_float_type)) {
        cx_error(err_incompatible_types);
    }

    if (p_type2) {
        p_type2 = p_type2->base_type();

        if ((p_type2 != p_integer_type) && (p_type2 != p_float_type)) {
            cx_error(err_incompatible_types);
        }
    }
}

/** check_boolean        Check that the type of each operand is
 *                      boolean.  Flag an incompatible type
 *                      error if not.
 *
 * @param p_type1 : ptr to the first  operand's type object.
 * @param p_type2 : ptr to the second operand's type object or nullptr.
 */
void check_boolean(const cx_type *p_type1, const cx_type *p_type2) {
    if ((p_type1->base_type() != p_boolean_type)
            || (p_type2 && (p_type2->base_type() != p_boolean_type))) {
        cx_error(err_incompatible_types);
    }
}

/** check_assignment_type_compatible   Check that a value's type is
 *                                  assignment compatible with
 *                                  the target's type.  Flag an
 *                                  error if not.
 *
 * @param p_target_type : ptr to the target's type object.
 * @param p_value_type  : ptr to the value's  type object.
 * @param ec          : error code.
 */
void check_assignment_type_compatible(const cx_type *p_target_type,
        const cx_type *p_value_type, cx_error_code ec) {

    p_target_type = p_target_type->base_type();
    p_value_type = p_value_type->base_type();

    if ((p_target_type == p_file_type) ||
            (p_value_type == p_file_type)) return;

    if (p_target_type == p_value_type) return;

    if ((p_target_type == p_float_type)
            && (p_value_type == p_integer_type)) return;

    if ((p_target_type == p_integer_type)
            && (p_value_type == p_char_type)) return;

    if ((p_target_type == p_char_type)
            && (p_value_type == p_integer_type)) return;

    if ((p_target_type == p_float_type)
            && (p_value_type == p_double_type)) return;

    if ((p_target_type == p_double_type)
            && (p_value_type == p_integer_type)) return;

    if ((p_target_type == p_double_type)
            && (p_value_type == p_float_type)) return;

    if ((p_target_type == p_integer_type)
            && (p_value_type == p_float_type)) return;

    if ((p_target_type == p_integer_type)
            && (p_value_type == p_double_type)) return;

    if ((p_target_type->form == fc_array)
            && (p_value_type->form == fc_array)
            && (p_target_type->array.p_element_type == p_char_type)
            && (p_value_type->array.p_element_type == p_char_type)) {
        return;
    }

    cx_error(ec);
}

/** integer_operands     Check that the types of both operands
 *                      are integer.
 *
 * @param p_type1 : ptr to the first  operand's type object.
 * @param p_type2 : ptr to the second operand's type object.
 * @return true if yes, false if no.
 */
bool integer_operands(const cx_type *p_type1, const cx_type *p_type2) {
    p_type1 = p_type1->base_type();
    p_type2 = p_type2->base_type();

    return (((p_type1 == p_integer_type) && (p_type2 == p_integer_type)) ||
            ((p_type1 == p_char_type) && (p_type2 == p_integer_type)) ||
            ((p_type1 == p_integer_type) && (p_type2 == p_char_type)) ||
            ((p_type1 == p_char_type) && (p_type2 == p_char_type)));
}

/** real_operands        Check that the types of both operands
 *                      are real, or that one is real and the
 *                      other is integer.
 *
 * @param p_type1 : ptr to the first  operand's type object.
 * @param p_type2 : ptr to the second operand's type object.
 * @return true if yes, false if no.
 */
bool real_operands(const cx_type *p_type1, const cx_type *p_type2) {
    p_type1 = p_type1->base_type();
    p_type2 = p_type2->base_type();

    return (((p_type1 == p_float_type) && (p_type2 == p_float_type))
            || ((p_type1 == p_float_type) && (p_type2 == p_integer_type))
            || ((p_type2 == p_float_type) && (p_type1 == p_integer_type))
            || ((p_type1 == p_float_type) && (p_type2 == p_double_type))
            || ((p_type2 == p_float_type) && (p_type1 == p_double_type))
            || ((p_type1 == p_double_type) && (p_type2 == p_integer_type))
            || ((p_type2 == p_double_type) && (p_type1 == p_integer_type))
            || ((p_type1 == p_double_type) && (p_type2 == p_float_type))
            || ((p_type2 == p_double_type) && (p_type1 == p_float_type)));
}