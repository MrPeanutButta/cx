#include <sstream>
#include "std_members.h"
#include "types.h"
#include "symtable.h"

cx_symtab *std_members = nullptr;

void init_std_members (void) {
    // initialize after basic types are init

    struct {
        cx_symtab_node *p_node;
        std::string name;
        cx_function_code code;
        m_call member_call;
        cx_type *p_type;
    } type_members[] = {
        { nullptr, "each", func_std_iterator, &cx_std_members::each, p_void_type},
        { nullptr, "size", func_std_member, &cx_std_members::size, p_integer_type},
        { nullptr, "length", func_std_member, &cx_std_members::length, p_integer_type},
        { nullptr, "to_str", func_std_member, &cx_std_members::to_str, new cx_type(fc_array, 0, nullptr)},
        { nullptr, "to_wstr", func_std_member, &cx_std_members::to_wstr, new cx_type(fc_array, 0, nullptr)},
        { nullptr, "to_int", func_std_member, &cx_std_members::to_int, p_integer_type},
        { nullptr, "to_chr", func_std_member, &cx_std_members::to_chr, p_char_type},
        { nullptr, "to_flt", func_std_member, &cx_std_members::to_flt, p_float_type},
        { nullptr, "to_bool", func_std_member, &cx_std_members::to_bool, p_boolean_type},
        { nullptr, "to_wchr", func_std_member, &cx_std_members::to_wchr, p_wchar_type},
        { nullptr, "to_byte", func_std_member, &cx_std_members::to_byte, p_uint8_type}
    };
    // allocate std member functions for basic types
    std_members = new cx_symtab;

    p_integer_type->complex.p_class_scope = std_members;
    p_uint8_type->complex.p_class_scope = std_members;
    p_float_type->complex.p_class_scope = std_members;
    p_boolean_type->complex.p_class_scope = std_members;
    p_char_type->complex.p_class_scope = std_members;
    p_wchar_type->complex.p_class_scope = std_members;

    for (auto &member : type_members) {
        member.p_node = std_members->enter(member.name.c_str(), dc_function);
        member.p_node->defn.routine.iterator.postfix = 0;
        member.p_node->defn.routine.std_member = member.member_call;
        member.p_node->defn.routine.which = member.code;
        member.p_node->defn.routine.parm_count = 0;
        member.p_node->defn.routine.total_parm_size = 0;
        member.p_node->defn.routine.locals.p_parms_ids = nullptr;
        member.p_node->defn.routine.locals.p_constant_ids = nullptr;
        member.p_node->defn.routine.locals.p_type_ids = nullptr;
        member.p_node->defn.routine.locals.p_variable_ids = nullptr;
        member.p_node->defn.routine.locals.p_function_ids = nullptr;

        set_type(member.p_node->p_type, member.p_type);

        if (member.name == "to_str") {
            set_type(member.p_node->p_type->array.p_element_type, p_char_type);
        } else if (member.name == "to_wstr") {
            set_type(member.p_node->p_type->array.p_element_type, p_wchar_type);
        }

    }
}

cx_type *cx_std_members::size (cx_executor* cx,
                               cx_symtab_node* cx_function_id,
                               const cx_type *p_type) {
    cx->pop();
    cx_stack_item *p_size_val = new cx_stack_item;
    p_size_val->basic_types.int__ = p_type->size;
    cx->push((void*) p_size_val);

    return cx_function_id->p_type;
}

cx_type *cx_std_members::length (cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    cx->pop();
    cx_stack_item *p_length_val = new cx_stack_item;

    p_type->form == fc_array ?
            p_length_val->basic_types.int__ = p_type->array.element_count :
            p_length_val->basic_types.int__ = 1;

    cx->push((void *) p_length_val);

    return cx_function_id->p_type;
}

/** @TODO - needs to be fixed for wide char
 */
cx_type *cx_std_members::to_str (cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    std::stringstream ss;
    ss.clear();

    cx_stack_item *mem = nullptr;

    ///if (p_type->.how == dc_reference) {
        mem = cx->top();
    //} else {
        mem = (cx_stack_item *) cx->top()->basic_types.addr__;
    //}

    switch (p_type->type_code) {
        case cx_int:
            ss << mem->basic_types.int__ << '\0';
            break;
        case cx_char:
            ss << mem->basic_types.char__ << '\0';
            break;
        case cx_wchar:
            ss << mem->basic_types.wchar__ << '\0';
            break;
        case cx_float:
            ss << mem->basic_types.float__ << '\0';
            break;
        case cx_bool:
            ss << (mem->basic_types.bool__ ? "true" : "false") << '\0';
            break;
        case cx_uint8:
            ss << (int) mem->basic_types.uint8__ << '\0';
            break;
        default:
            ss << mem->basic_types.addr__ << '\0';
            break;
    }

    cx->pop();

    const int size = ss.str().size() - 1;
    const int length = ss.str().length() - 1;

    cx_type *p_str = new cx_type(fc_array, size, nullptr);
    set_type(p_str->array.p_element_type, p_char_type);
    p_str->array.element_count = length;
    p_str->array.max_index = length;

    char *p_to_str = (char *) malloc(size + 1);
    memcpy(p_to_str, ss.str().c_str(), size + 1);

    cx->push((void *) p_to_str);

    return p_str;
}

cx_type *cx_std_members::to_wstr (cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_members::to_int (cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_members::to_chr (cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_members::to_flt (cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_members::to_bool (cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_members::to_wchr (cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_members::to_byte (cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_members::each (cx_executor *cx,
                               cx_symtab_node *cx_function_id,
                               const cx_type *p_type) {

    /*int *iteration = &cx_function_id->defn.routine.iterator.current_iteration;
    const int end = cx_function_id->defn.routine.iterator.p_node->p_type->array.element_count;

    int loop_start = cx_function_id->defn.routine.iterator.at_loop_start;*

    /*for (*iteration = 0; *iteration < end; ++*iteration) {
        cx->execute_variable(cx_function_id->defn.routine.iterator.p_node, true);*/
    cx->execute_iterator(cx_function_id);
    /*cx->goto_location(loop_start);
    cx->get_next_token();
}*/

    return cx_function_id->p_type;
}