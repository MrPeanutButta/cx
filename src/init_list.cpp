#include "exec.h"
#include "members.h"
#include "common.h"

cx_type *cx_executor::execute_initialization_list(void) {

    cx_type *p_result_type = nullptr;

    get_token();
    int total_size = 0;
    int old_size = 0;
    int elem_count = 0;
    bool comma = false;
    void *p_address = nullptr;
    char *tmp = nullptr;

    cx_type *p_prev_type = nullptr;

    do {

        p_result_type = execute_expression();
        p_prev_type = p_result_type;
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
            memcpy(&tmp[old_size], p_source, total_size);
            tmp[total_size] = '\0';
        }

        pop();
        old_size = total_size;

        if (token == tc_comma) {
            comma = true;
            get_token();
        } else comma = false;

        ++elem_count;
    } while (comma);

    // }
    get_token();
    push((void *) p_address);

    cx_type *p_array_type = new cx_type(fc_array, total_size, nullptr);
    set_type(p_array_type->array.p_element_type, p_result_type);
    p_array_type->array.element_count = elem_count;
    p_array_type->array.max_index = elem_count;

    return p_array_type;
}