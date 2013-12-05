#include "exec.h"
#include "common.h"

cx_type *cx_executor::execute_initialization_list (void) {

    cx_type *p_result_type = nullptr;

    get_token();
    int num_of_elements = 0;
    int total_size = 0;
    int old_size = 0;
    bool comma = false;
    void *p_address = nullptr;
    char *tmp = (char *) p_address;
    cx_type *init_list = new cx_type(fc_array, 0, nullptr);

    do {
        p_result_type = execute_expression();
        total_size += p_result_type->size;
        p_address = realloc(p_address, old_size + p_result_type->size);

        if (p_address == nullptr) {
            perror("realloc");
            exit(0);
        }

        tmp = (char *) p_address;

        if (p_result_type->is_scalar_type()) {
            switch (p_result_type->type_code) {
                case cx_int:
                {
                    int value = top()->basic_types.int__;
                    memcpy(&tmp[old_size], &value, p_result_type->size);
                }
                    break;
                case cx_char:
                {
                    char value = top()->basic_types.char__;
                    memcpy(&tmp[old_size], &value, p_result_type->size);
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value = top()->basic_types.wchar__;
                    memcpy(&tmp[old_size], &value, p_result_type->size);
                }
                    break;
                case cx_float:
                {
                    float value = top()->basic_types.float__;
                    memcpy(&tmp[old_size], &value, p_result_type->size);
                }
                    break;
                case cx_bool:
                {
                    bool value = top()->basic_types.bool__;
                    memcpy(&tmp[old_size], &value, p_result_type->size);
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value = top()->basic_types.uint8__;
                    memcpy(&tmp[old_size], &value, p_result_type->size);
                }
                    break;
                default:break;
            }
        } else {
            void *p_source = top()->basic_types.addr__;
            memcpy(&tmp[old_size], p_source, p_result_type->size);
        }

        pop();
        ++num_of_elements;
        old_size = total_size;

        if (token == tc_comma) {
            comma = true;
            get_token();
        } else comma = false;

    } while (comma);

    // }
    get_token();
    push(p_address);
    init_list->array.element_count = num_of_elements;
    init_list->array.max_index = num_of_elements;
    init_list->size = total_size;
    set_type(init_list->array.p_element_type, p_result_type);

    return init_list;
}