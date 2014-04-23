#include <sstream>
#include "members.h"
#include "types.h"
#include "symtab.h"

cx_symtab *std_type_members = nullptr;

void init_std_members(void) {
    // initialize after basic types are init

    struct member {
        cx_symtab_node *p_node;
        std::string name;
        cx_function_code func_code;
        ext_call member_call;
        cx_type *p_type;
    } members[] = {
        { nullptr, "size", func_std_member, &cx_std_type_members::size, p_integer_type},
        { nullptr, "length", func_std_member, &cx_std_type_members::length, p_integer_type},
        { nullptr, "to_str", func_std_member, &cx_std_type_members::to_str, new cx_type(fc_array, 0, nullptr)},
        { nullptr, "to_wstr", func_std_member, &cx_std_type_members::to_wstr, new cx_type(fc_array, 0, nullptr)},
        { nullptr, "to_int", func_std_member, &cx_std_type_members::to_int, p_integer_type},
        { nullptr, "to_chr", func_std_member, &cx_std_type_members::to_chr, p_char_type},
        { nullptr, "to_flt", func_std_member, &cx_std_type_members::to_flt, p_float_type},
        { nullptr, "to_bool", func_std_member, &cx_std_type_members::to_bool, p_boolean_type},
        { nullptr, "to_wchr", func_std_member, &cx_std_type_members::to_wchr, p_wchar_type},
        { nullptr, "to_byte", func_std_member, &cx_std_type_members::to_byte, p_uint8_type}
    };

    // allocate std member functions for basic types
    std_type_members = new cx_symtab;

    p_integer_type->complex.p_class_scope = std_type_members;
    p_uint8_type->complex.p_class_scope = std_type_members;
    p_float_type->complex.p_class_scope = std_type_members;
    p_boolean_type->complex.p_class_scope = std_type_members;
    p_char_type->complex.p_class_scope = std_type_members;
    p_wchar_type->complex.p_class_scope = std_type_members;

    for (auto &mbr : members) {

        mbr.p_node = std_type_members->enter(mbr.name.c_str(), dc_function);
        mbr.p_node->defn.routine.parm_count = 0;
        mbr.p_node->defn.routine.total_parm_size = 0;

        set_type(mbr.p_node->p_type, mbr.p_type);
        mbr.p_node->defn.routine.ext_function = mbr.member_call;
        mbr.p_node->defn.routine.which = mbr.func_code;

        if (mbr.name == "to_str") {
            set_type(mbr.p_node->p_type->array.p_element_type, p_char_type);
        } else if (mbr.name == "to_wstr") {
            set_type(mbr.p_node->p_type->array.p_element_type, p_wchar_type);
        }
    }
}

cx_type *cx_std_type_members::size(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    p_stack->pop();
    p_stack->push(new cx_stack_item((int) p_type->size));

    return p_node->p_type;
}

cx_type *cx_std_type_members::length(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    p_stack->pop();
    p_stack->push(new cx_stack_item((int) p_type->form == fc_array ? p_type->array.element_count : 1));

    return p_node->p_type;
}

/** @TODO - needs to be fixed for wide char
 */
cx_type *cx_std_type_members::to_str(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    std::stringstream ss;
    ss.clear();

    cx_stack_item *mem = (cx_stack_item *) p_stack->top()->basic_types.addr__;

    if (p_type->is_scalar_type()) {
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
            default:break;
        }
    }

    p_stack->pop();

    const int size = ss.str().size() - 1;
    const int length = ss.str().length() - 1;

    cx_type *p_str = new cx_type(fc_array, size, nullptr);
    set_type(p_str->array.p_element_type, p_char_type);
    p_str->array.element_count = length;
    p_str->array.max_index = length;
    char *p_to_str = (char *) malloc(size + 1);
    memcpy(p_to_str, ss.str().c_str(), size + 1);
    p_stack->push((void *) p_to_str);

    return p_str;
}

cx_type *cx_std_type_members::to_wstr(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    std::wstringstream ss;
    ss.clear();

    cx_stack_item *mem = nullptr;

    mem = (cx_stack_item *) p_stack->top()->basic_types.addr__;

    if (p_type->is_scalar_type()) {
        switch (p_type->type_code) {
            case cx_int:
                ss << mem->basic_types.int__ << L'\0';
                break;
            case cx_char:
                ss << mem->basic_types.char__ << L'\0';
                break;
            case cx_wchar:
                ss << mem->basic_types.wchar__ << L'\0';
                break;
            case cx_float:
                ss << mem->basic_types.float__ << L'\0';
                break;
            case cx_bool:
                ss << (mem->basic_types.bool__ ? L"true" : L"false") << L'\0';
                break;
            case cx_uint8:
                ss << (int) mem->basic_types.uint8__ << L'\0';
                break;
            default:break;
        }
    }

    p_stack->pop();

    const int size = ss.str().size() - 1;
    const int length = ss.str().length() - 1;

    cx_type *p_wstr = new cx_type(fc_array, size, nullptr);
    set_type(p_wstr->array.p_element_type, p_wchar_type);
    p_wstr->array.element_count = length;
    p_wstr->array.max_index = length;
    wchar_t *p_to_wstr = (wchar_t *)malloc((sizeof (wchar_t) * length) + 1);
    memset(p_to_wstr, 0, (sizeof (wchar_t) * length) + 1);
    memcpy(p_to_wstr, ss.str().c_str(), (sizeof (wchar_t) * length) + 1);
    p_stack->push((void *) p_to_wstr);

    return p_wstr;
}

cx_type *cx_std_type_members::to_int(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    std::stringstream ss;
    ss.clear();

    cx_stack_item *mem = (cx_stack_item *) p_stack->top()->basic_types.addr__;

    if (p_type->is_scalar_type()) {
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
                ss << mem->basic_types.bool__ << '\0';
                break;
            case cx_uint8:
                ss << (int) mem->basic_types.uint8__ << '\0';
                break;
            default:break;
        }
    }

    return p_node->p_type;
}

cx_type *cx_std_type_members::to_chr(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    return p_node->p_type;
}

cx_type *cx_std_type_members::to_flt(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    return p_node->p_type;
}

cx_type *cx_std_type_members::to_bool(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    return p_node->p_type;
}

cx_type *cx_std_type_members::to_wchr(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    return p_node->p_type;
}

cx_type *cx_std_type_members::to_byte(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type) {

    return p_node->p_type;
}