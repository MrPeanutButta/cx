#include "exec.h"
#include "common.h"
#include "types.h"

void copy_scalar (cx_stack_item* p_target,
                  const cx_type *p_type,
                  void *&address) {

    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    float *t_float = &p_target->basic_types.float__;
    bool *t_bool = &p_target->basic_types.bool__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;

    switch (p_type->type_code) {
        case cx_int:
        {
            memcpy(address, t_int, sizeof (int));
        }
            break;
        case cx_char:
        {
            memcpy(address, t_char, sizeof (char));
        }
            break;
        case cx_wchar:
        {
            memcpy(address, t_wchar, sizeof (wchar_t));
        }
            break;
        case cx_float:
        {
            memcpy(address, t_float, sizeof (float));
        }
            break;
        case cx_bool:
        {
            memcpy(address, t_bool, sizeof (bool));
        }
            break;
        case cx_uint8:
        {
            memcpy(address, t_uint8, sizeof (uint8_t));
        }
            break;
        default:break;
    }
}

void copy_array (const cx_type *p_type,
                 void *&p_target_address,
                 void *&p_source) {

    int size = p_type->total_size;

    memcpy(p_target_address, p_source, size + 1);

}

void *new_value (const cx_type *p_type, void *&address) {

    void *p_values = nullptr;
    const int size = p_type->total_size;

    p_values = (void *) realloc(address, size + 1);

    if (p_values == nullptr) {
        perror("cx_malloc");
        cx_runtime_error(rte_none);
    }

    memset(p_values, 0, size + 1);

    return p_values;
}

void cx_executor::cx_malloc (cx_symtab_node* p_target_id,
                             cx_type* p_target_type, cx_type* p_expr_type,
                             cx_stack_item* p_target, void* &p_target_address) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    const int size = p_expr_type->total_size;
    const int num_of_elements = size / p_expr_type->base_type()->size;

    void *p_source = nullptr;

    p_source = top()->basic_types.addr__;

    if (p_source != nullptr) {
        p_target_address = new_value(p_expr_type, p_target_address);
    }

    if (p_expr_type->is_scalar_type()) {
        copy_scalar(p_target, p_expr_type->array.p_element_type, p_target_address);
    } else {
        copy_array(p_expr_type, p_target_address, p_source);
    }

    set_type(p_target_id->p_type,p_expr_type); 
    //p_target_id->p_type->array.element_count = num_of_elements;
    //p_target_id->p_type->array.max_index = num_of_elements;
    //p_target_id->p_type->size = num_of_elements * p_target_id->p_type->base_type()->size;

    /*if (p_expr_type->is_temp_value) {
        memset(p_source, 0, size + 1);
        remove_type(p_expr_type);
    }*/

    if (p_target_id->defn.how == dc_function) {
        p_target->basic_types.addr__ = p_target_address;
    } else {
        p_target_id->runstack_item->basic_types.addr__ = p_target_address;
    }

}