#include <cstdio>
#include "buffer.h"
#include "error.h"
#include "types.h"
#include "members.h"

const char *form_strings[] = {
    "*error*", "scalar", "enum", "subrange", "array", "complex", "pointer"
};

// Pointers to predefined types.
cx_symtab_node *p_main_function_id = nullptr;
cx_type *p_integer_type = nullptr;
cx_type *p_uint8_type = nullptr;
cx_type *p_float_type = nullptr;
cx_type *p_boolean_type = nullptr;
cx_type *p_char_type = nullptr;
cx_type *p_wchar_type = nullptr;
//cx_type *p_class_type = nullptr;
cx_type *p_complex_type = nullptr;
//cx_type *p_file_type = nullptr;
cx_type *p_dummy_type = nullptr;
cx_type *p_void_type = nullptr;

/** Constructors    General.
 *
 * @param fc  : form code.
 * @param s   : byte size of type.
 * @param p_id : ptr to symbol table node of type identifier.
 */
cx_type::cx_type(cx_type_form_code fc, int s, cx_symtab_node* p_id, cx_symtab *p_members)
: form(fc), size(s), p_type_id(p_id), reference_count(0) {
    next = nullptr;

    switch (form) {
        case fc_array:
            this->size = s;
            array.p_element_type = nullptr;
            array.element_count = 0;
            array.min_index = 0;
            array.p_index_type = p_integer_type;
            break;
        default:
            break;
    }

    std::string type_name = "dummy";
    is_temp_value = false;

    if (p_type_id != nullptr)
        type_name = p_type_id->string__();

    if (form != fc_stream) {
        this->complex.p_class_scope = std_type_members;

        if (form != fc_array) {
            //this->complex.p_class_scope = std_type_members;

            if (type_name == "int") {
                type_code = cx_int;
            } else if (type_name == "char") {
                type_code = cx_char;
            } else if (type_name == "wchar") {
                type_code = cx_wchar;
            } else if (type_name == "float") {
                type_code = cx_float;
            } else if (type_name == "bool") {
                type_code = cx_bool;
            } else if (type_name == "byte") {
                type_code = cx_uint8;
            } else if (type_name == "class") {
                type_code = cx_complex;
            } else if (type_name == "void") {
                type_code = cx_void;
            }
        } else {
            type_code = cx_address;
        }
    } else {
        type_code = cx_file;
        //complex.p_class_scope = std_stream_members;
    }
}

cx_type::cx_type(int length, bool constant)
: size(length), form(fc_array), reference_count(0), is_constant__(constant) {
    p_type_id = nullptr;
    next = nullptr;

    complex.p_class_scope = std_type_members;
    // used for string constants only. can probably go away
    array.p_index_type = array.p_element_type = nullptr;
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
        case fc_array:
            remove_type(array.p_element_type);
            break;
        case fc_complex:
            // delete complex.pSymtabPublic;
            break;

        default:
            break;
    }
}

/** initialize_builtin_types   Initialize the predefined
 *                              types by entering their
 *                              identifiers into the symbol
 *                              table.
 *
 * @param p_symtab : ptr to symbol table.
 */
void initialize_builtin_types(cx_symtab *p_symtab) {

    // if main already exists this is not the parent instance
    // if (p_main_function_id != nullptr) return;

    p_main_function_id = p_symtab->enter("main", dc_function);
    p_main_function_id->defn.routine.which = func_forward;

    cx_symtab_node *p_integer_id = p_symtab->enter("int", dc_type);
    cx_symtab_node *p_uint8_id = p_symtab->enter("byte", dc_type);
    cx_symtab_node *p_float_id = p_symtab->enter("float", dc_type);
    cx_symtab_node *p_complex_id = p_symtab->enter("class", dc_type);
    cx_symtab_node *p_boolean_id = p_symtab->enter("bool", dc_type);
    cx_symtab_node *p_char_id = p_symtab->enter("char", dc_type);
    cx_symtab_node *p_wchar_id = p_symtab->enter("wchar", dc_type);
    cx_symtab_node *p_false_id = p_symtab->enter("false", dc_constant);
    cx_symtab_node *p_true_id = p_symtab->enter("true", dc_constant);
    cx_symtab_node *p_void_id = p_symtab->enter("void", dc_type);

    // only used for functions with no return value
    if (!p_void_type) {
        set_type(p_void_type, new cx_type(fc_none, 0, p_void_id));
    }

    if (!p_integer_type) {
        set_type(p_integer_type, new cx_type(fc_scalar, sizeof (int), p_integer_id));
    }

    if (!p_uint8_type) {
        set_type(p_uint8_type, new cx_type(fc_scalar, sizeof (uint8_t), p_uint8_id));
    }

    if (!p_float_type) {
        set_type(p_float_type, new cx_type(fc_scalar, sizeof (float), p_float_id));
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
        set_type(p_complex_type, new cx_type(fc_complex, 0, p_complex_id));
    }

    set_type(p_main_function_id->p_type, p_integer_type);

    // link each predefined type id's node to it's type object
    set_type(p_integer_id->p_type, p_integer_type);
    set_type(p_uint8_id->p_type, p_uint8_type);
    set_type(p_float_id->p_type, p_float_type);
    set_type(p_boolean_id->p_type, p_boolean_type);
    set_type(p_char_id->p_type, p_char_type);
    set_type(p_wchar_id->p_type, p_wchar_type);
    set_type(p_complex_id->p_type, p_complex_type);

    p_boolean_type->enumeration.max = 1;
    p_boolean_type->enumeration.p_const_ids = p_false_id;

    p_false_id->defn.constant.value.bool__ = false;
    p_true_id->defn.constant.value.bool__ = true;
    p_false_id->defn.how = dc_constant;
    p_true_id->defn.how = dc_constant;

    set_type(p_true_id->p_type, p_boolean_type);
    set_type(p_false_id->p_type, p_boolean_type);

    p_false_id->next__ = p_true_id;

    // initialize standard type members
    init_std_members();

    set_type(p_dummy_type, new cx_type(fc_none, 1, nullptr));
}

/** remove_builtin_types       Remove the predefined types.
 */
void remove_builtin_types(void) {
    remove_type(p_complex_type);
    remove_type(p_float_type);
    remove_type(p_boolean_type);
    remove_type(p_char_type);
    remove_type(p_wchar_type);
    remove_type(p_integer_type);
    remove_type(p_uint8_type);
    remove_type(p_dummy_type);
}

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
    check_assignment_type_compatible(p_type1, p_type2, err_incompatible_types);
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

void check_bitwise_integer(const cx_type *p_type1, const cx_type *p_type2) {
    cx_type_code target_type = p_type1->base_type()->type_code;
    cx_type_code value_type = p_type2->base_type()->type_code;

    switch (target_type) {
        case cx_int:
        case cx_char:
        case cx_bool:
        case cx_wchar:
        case cx_uint8:
            switch (value_type) {
                case cx_int:
                case cx_char:
                case cx_bool:
                case cx_wchar:
                case cx_uint8:
                    return;
                    break;
                default:break;
            }
            break;
        default:
            break;
    }

    cx_error(err_incompatible_types);
}

void check_bitwise_integer(const cx_type *p_type1) {
    cx_type_code target_type = p_type1->base_type()->type_code;

    switch (target_type) {
        case cx_int:
        case cx_char:
        case cx_bool:
        case cx_wchar:
        case cx_uint8:
            return;
            break;
        default:
            break;
    }

    cx_error(err_incompatible_types);
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

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code value_type = p_value_type->type_code;

    if (target_type == value_type) return;
    if ((value_type == cx_file) && (target_type != cx_file)) return;
    if ((target_type == cx_file) && (value_type != cx_file)) return;

    if (p_target_type->is_scalar_type()) {
        switch (target_type) {
            case cx_uint8:
                switch (value_type) {
                    case cx_int:
                    case cx_char:
                    case cx_bool:
                    case cx_float:
                    case cx_wchar:
                    case cx_uint8:
                        return;
                        break;
                    default:
                        break;
                }
                break;
            case cx_int:
                switch (value_type) {
                    case cx_int:
                    case cx_char:
                    case cx_bool:
                    case cx_float:
                    case cx_wchar:
                    case cx_uint8:
                        return;
                        break;
                    default:
                        break;
                }
                break;
            case cx_char:
                switch (value_type) {
                    case cx_int:
                    case cx_char:
                    case cx_float:
                    case cx_wchar:
                    case cx_bool:
                    case cx_uint8:
                    case cx_address:
                        return;
                        break;
                    default:
                        break;
                }
                break;
            case cx_wchar:
                switch (value_type) {
                    case cx_int:
                    case cx_char:
                    case cx_float:
                    case cx_wchar:
                    case cx_bool:
                    case cx_uint8:
                        return;
                        break;
                    default:
                        break;
                }
                break;
            case cx_float:
                switch (value_type) {
                    case cx_int:
                    case cx_char:
                    case cx_float:
                    case cx_bool:
                    case cx_wchar:
                    case cx_uint8:
                        return;
                        break;
                    default:
                        break;
                }
                break;
            case cx_bool:
                switch (value_type) {
                    case cx_int:
                    case cx_char:
                    case cx_float:
                    case cx_bool:
                    case cx_wchar:
                    case cx_uint8:
                        return;
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    } else if (p_target_type->form == fc_array) {
        //        if (p_target_type->base_type() ==
        //            p_value_type->array.p_element_type->type_code) return;
        return;
    } else {
        if (p_target_type == p_value_type) return;
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

    cx_type_code target_type = p_type1->base_type()->type_code;
    cx_type_code value_type = p_type2->base_type()->type_code;

    switch (target_type) {
        case cx_uint8:
            switch (value_type) {
                case cx_int:
                case cx_char:
                case cx_bool:
                case cx_wchar:
                case cx_uint8:
                    return true;
                    break;
                default:
                    break;
            }
            break;
        case cx_int:
            switch (value_type) {
                case cx_int:
                case cx_char:
                case cx_bool:
                case cx_float:
                case cx_wchar:
                case cx_uint8:
                    return true;
                    break;
                default:
                    break;
            }
            break;
        case cx_char:
            switch (value_type) {
                case cx_int:
                case cx_char:
                case cx_wchar:
                case cx_bool:
                case cx_uint8:
                    return true;
                    break;
                default:
                    break;
            }
            break;
        case cx_wchar:
            switch (value_type) {
                case cx_int:
                case cx_char:
                case cx_wchar:
                case cx_bool:
                case cx_uint8:
                    return true;
                    break;
                default:
                    break;
            }
            break;
        case cx_bool:
            switch (value_type) {
                case cx_int:
                case cx_char:
                case cx_bool:
                case cx_wchar:
                case cx_uint8:
                    return true;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }

    return false;
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
            || ((p_type2 == p_float_type) && (p_type1 == p_integer_type)));

}