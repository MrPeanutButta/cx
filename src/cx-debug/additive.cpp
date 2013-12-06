#include "exec.h"
#include "common.h"

cx_type *cx_executor::execute_additive (cx_token_code op, cx_type* lhs, cx_type* rhs) {
    cx_type *p_result_type = rhs;

    switch (op) {
        case tc_plus:
            p_result_type = plus(lhs, rhs);
            break;
        case tc_minus:
            minus(lhs, rhs);
            break;
        case tc_bit_leftshift:
            bit_leftshift(lhs, rhs);
            break;
        case tc_bit_rightshift:
            bit_rightshift(lhs, rhs);
            break;
        case tc_bit_AND:
            bit_and(lhs, rhs);
            break;
        case tc_bit_XOR:
            bit_xor(lhs, rhs);
            break;
        case tc_bit_OR:
            bit_or(lhs, rhs);
            break;
        case tc_logic_OR:
            logic_or(lhs, rhs);
            return p_boolean_type;
            break;
        default:
            break;
    }

    return p_result_type;
}

cx_type *cx_executor::alloc_temp_rvalue (const cx_type* lhs,
                                         cx_type* rhs) {

    void *addr1 = nullptr;
    void *addr2 = nullptr;
    const int size = lhs->size + rhs->size;
    const int num_of_elements = size / rhs->base_type()->size;

    cx_type *temp_type = new cx_type(fc_array, size, nullptr);

	set_type(temp_type->array.p_element_type, rhs->base_type());
    temp_type->is_temp_value = true;
    temp_type->array.element_count = num_of_elements;
    temp_type->array.max_index = num_of_elements;
    temp_type->size = size;
	temp_type->type_code = rhs->base_type()->type_code;

    void *p_target_address = ::malloc(size + 1);

    if (p_target_address == nullptr) {
        perror("malloc");
        cx_runtime_error(rte_none);
    }
    
    memset(p_target_address, 0, size + 1);
    char *temp_val = (char *) p_target_address;

    if ((lhs->form == fc_array) && (rhs->form == fc_array)) {

        addr2 = top()->basic_types.addr__;
        pop();
        char *x = (char *) addr2;

        addr1 = top()->basic_types.addr__;
        pop();
        char *y = (char *) addr1;

        set_type(temp_type->array.p_element_type, rhs->base_type());

        memcpy(temp_val, addr1, lhs->size + 1);
        memcpy(&temp_val[lhs->size], addr2, rhs->size + 1);

    } else if (lhs->form != fc_array) {

        addr2 = top()->basic_types.addr__;
        pop();

        switch (lhs->type_code) {
            case cx_int:
            {
                int value1 = top()->basic_types.int__;
                pop();

                memcpy(temp_val, &value1, lhs->size);
                memcpy(&temp_val[lhs->size], addr2, rhs->size + 1);
            }
                break;
            case cx_char:
            {
                char value1 = top()->basic_types.char__;
                pop();

                memcpy(temp_val, &value1, lhs->size);
                memcpy(&temp_val[lhs->size], addr2, rhs->size + 1);
            }
                break;
            case cx_wchar:
            {
                wchar_t value1 = top()->basic_types.wchar__;
                pop();

                memcpy(temp_val, &value1, lhs->size);
                memcpy(&temp_val[lhs->size], addr2, rhs->size + 1);
            }
                break;
            case cx_float:
            {
                float value1 = top()->basic_types.float__;
                pop();

                memcpy(temp_val, &value1, lhs->size);
                memcpy(&temp_val[lhs->size], addr2, rhs->size + 1);
            }
                break;
            case cx_bool:
            {
                bool value1 = top()->basic_types.bool__;
                pop();

                memcpy(temp_val, &value1, lhs->size);
                memcpy(&temp_val[lhs->size], addr2, rhs->size + 1);
            }
                break;
            case cx_uint8:
            {
                uint8_t value1 = top()->basic_types.uint8__;
                pop();

                memcpy(temp_val, &value1, lhs->size);
                memcpy(&temp_val[lhs->size], addr2, rhs->size + 1);
            }
                break;
            default:break;
        }
    } else if (rhs->form != fc_array) {

        switch (rhs->type_code) {
            case cx_int:
            {
                int value2 = top()->basic_types.int__;
                pop();

                addr1 = top()->basic_types.addr__;
                pop();

                memcpy(temp_val, addr1, lhs->size);
                memcpy(&temp_val[lhs->size], &value2, rhs->size);
            }
                break;
            case cx_char:
            {
                char value2 = top()->basic_types.char__;
				char *u = (char *)top()->basic_types.addr__;
                pop();

                addr1 = top()->basic_types.addr__;
                pop();

                memcpy(temp_val, addr1, lhs->size);
                memcpy(&temp_val[lhs->size], &value2, rhs->size);
            }
                break;
            case cx_wchar:
            {
                wchar_t value2 = top()->basic_types.wchar__;
                pop();

                addr1 = top()->basic_types.addr__;
                pop();

                memcpy(temp_val, addr1, lhs->size);
                memcpy(&temp_val[lhs->size], &value2, rhs->size);
            }
                break;
            case cx_float:
            {
                float value2 = top()->basic_types.float__;
                pop();

                addr1 = top()->basic_types.addr__;
                pop();

                memcpy(temp_val, addr1, lhs->size);
                memcpy(&temp_val[lhs->size], &value2, rhs->size);
            }
                break;
            case cx_bool:
            {
                bool value2 = top()->basic_types.bool__;
                pop();

                addr1 = top()->basic_types.addr__;
                pop();

                memcpy(temp_val, addr1, lhs->size);
                memcpy(&temp_val[lhs->size], &value2, rhs->size);
            }
                break;
            case cx_uint8:
            {
                uint8_t value2 = top()->basic_types.uint8__;
                pop();

                addr1 = top()->basic_types.addr__;
                pop();

                memcpy(temp_val, addr1, lhs->size);
                memcpy(&temp_val[lhs->size], &value2, rhs->size);
            }
                break;
            default:break;
        }
    }

    if (rhs->is_temp_value) {
        remove_type(rhs);
    }

	temp_val[num_of_elements] = '\0';
    push((void *) p_target_address);

    return temp_type;
}

cx_type *cx_executor::plus (cx_type *lhs, cx_type *rhs) {

    cx_type *p_result_type = rhs;

    if ((lhs->form != fc_array) && (rhs->form != fc_array)) {
        switch (rhs->type_code) {
            case cx_int:
            {
                switch (lhs->type_code) {
                    case cx_int:
                    {
                        int value2 = top()->basic_types.int__;
                        pop();
                        int value1 = top()->basic_types.int__;
                        pop();
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        int value2 = top()->basic_types.int__;
                        pop();
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        int value2 = top()->basic_types.int__;
                        pop();
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        int value2 = top()->basic_types.int__;
                        pop();
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        int value2 = top()->basic_types.int__;
                        pop();
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        int value2 = top()->basic_types.int__;
                        pop();
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
                    }
                        break;
                    case cx_address:
                        p_result_type = alloc_temp_rvalue(lhs, rhs);
                        break;
                    default:break;
                }
            }
                break;
            case cx_char:
            {
                switch (lhs->type_code) {
                    case cx_int:
                    {
                        char value2 = top()->basic_types.char__;
                        pop();
                        int value1 = top()->basic_types.int__;
                        pop();
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value2 = top()->basic_types.char__;
                        pop();
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        char value2 = top()->basic_types.char__;
                        pop();
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        char value2 = top()->basic_types.char__;
                        pop();
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        char value2 = top()->basic_types.char__;
                        pop();
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        char value2 = top()->basic_types.char__;
                        pop();
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
                    }
                        break;
                    case cx_address:
                        p_result_type = alloc_temp_rvalue(lhs, rhs);
                        break;
                    default:break;
                }
            }
                break;
            case cx_wchar:
            {
                switch (lhs->type_code) {
                    case cx_int:
                    {
                        wchar_t value2 = top()->basic_types.int__;
                        pop();
                        int value1 = top()->basic_types.int__;
                        pop();
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        wchar_t value2 = top()->basic_types.int__;
                        pop();
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value2 = top()->basic_types.int__;
                        pop();
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        wchar_t value2 = top()->basic_types.int__;
                        pop();
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        wchar_t value2 = top()->basic_types.int__;
                        pop();
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        wchar_t value2 = top()->basic_types.int__;
                        pop();
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
                    }
                        break;
                    case cx_address:
                        p_result_type = alloc_temp_rvalue(lhs, rhs);
                        break;
                    default:break;
                }
            }
                break;
            case cx_float:
            {
                switch (lhs->type_code) {
                    case cx_int:
                    {
                        float value2 = top()->basic_types.float__;
                        pop();
                        int value1 = top()->basic_types.int__;
                        pop();
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        float value2 = top()->basic_types.float__;
                        pop();
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        float value2 = top()->basic_types.float__;
                        pop();
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value2 = top()->basic_types.float__;
                        pop();
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        float value2 = top()->basic_types.float__;
                        pop();
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        float value2 = top()->basic_types.float__;
                        pop();
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
                    }
                        break;
                    case cx_address:
                        p_result_type = alloc_temp_rvalue(lhs, rhs);
                        break;
                    default:break;
                }
            }
                break;
            case cx_bool:
            {
                switch (lhs->type_code) {
                    case cx_int:
                    {
                        bool value2 = top()->basic_types.bool__;
                        pop();
                        int value1 = top()->basic_types.int__;
                        pop();
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        bool value2 = top()->basic_types.bool__;
                        pop();
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        bool value2 = top()->basic_types.bool__;
                        pop();
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        bool value2 = top()->basic_types.bool__;
                        pop();
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value2 = top()->basic_types.bool__;
                        pop();
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        bool value2 = top()->basic_types.bool__;
                        pop();
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
                    }
                        break;
                    case cx_address:
                        p_result_type = alloc_temp_rvalue(lhs, rhs);
                        break;
                    default:break;
                }
            }
                break;
            case cx_uint8:
            {
                switch (lhs->type_code) {
                    case cx_int:
                    {
                        uint8_t value2 = top()->basic_types.uint8__;
                        pop();
                        int value1 = top()->basic_types.int__;
                        pop();
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        uint8_t value2 = top()->basic_types.uint8__;
                        pop();
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        uint8_t value2 = top()->basic_types.uint8__;
                        pop();
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        uint8_t value2 = top()->basic_types.uint8__;
                        pop();
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        uint8_t value2 = top()->basic_types.uint8__;
                        pop();
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value2 = top()->basic_types.uint8__;
                        pop();
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
                    }
                        break;
                    case cx_address:
                        p_result_type = alloc_temp_rvalue(lhs, rhs);
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        p_result_type = alloc_temp_rvalue(lhs, rhs);
    }

    return p_result_type;
}

void cx_executor::minus (cx_type *lhs, cx_type *rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 - value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 - value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 - value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 - value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 - value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 - value2));
                }
                    break;
                default:break;
            }
        }
            break;
        case cx_char:
        {
            char value2 = top()->basic_types.char__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 - value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 - value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 - value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 - value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 - value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 - value2));
                }
                    break;
                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            wchar_t value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 - value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 - value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 - value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 - value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 - value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 - value2));
                }
                    break;
                default:break;
            }
        }
            break;
        case cx_float:
        {
            float value2 = top()->basic_types.float__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 - value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 - value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 - value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 - value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 - value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 - value2));
                }
                    break;
                default:break;
            }
        }
            break;
        case cx_bool:
        {
            bool value2 = top()->basic_types.bool__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 - value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 - value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 - value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 - value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 - value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 - value2));
                }
                    break;
                default:break;
            }
        }
            break;
        case cx_uint8:
        {
            uint8_t value2 = top()->basic_types.uint8__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 - value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 - value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 - value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 - value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 - value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 - value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::bit_leftshift (cx_type *lhs, cx_type *rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 << value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 << value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 << value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 << value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 << value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_char:
        {
            char value2 = top()->basic_types.char__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 << value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 << value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 << value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 << value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 << value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_wchar:
        {
            wchar_t value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 << value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 << value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 << value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 << value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 << value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_bool:
        {
            bool value2 = top()->basic_types.bool__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 << value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 << value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 << value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 << value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 << value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_uint8:
        {
            uint8_t value2 = top()->basic_types.uint8__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 << value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 << value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 << value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 << value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 << value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::bit_rightshift (cx_type *lhs, cx_type *rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 >> value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 >> value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 >> value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 >> value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 >> value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_char:
        {
            char value2 = top()->basic_types.char__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 >> value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 >> value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 >> value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 >> value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 >> value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_wchar:
        {
            wchar_t value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 >> value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 >> value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 >> value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 >> value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 >> value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_bool:
        {
            bool value2 = top()->basic_types.bool__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 >> value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 >> value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 >> value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 >> value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 >> value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_uint8:
        {
            uint8_t value2 = top()->basic_types.uint8__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 >> value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 >> value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 >> value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 >> value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 >> value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::bit_and (cx_type *lhs, cx_type *rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 & value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 & value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 & value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 & value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 & value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_char:
        {
            char value2 = top()->basic_types.char__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 & value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 & value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 & value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 & value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 & value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_wchar:
        {
            wchar_t value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 & value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 & value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 & value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 & value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 & value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_bool:
        {
            bool value2 = top()->basic_types.bool__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 & value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 & value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 & value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 & value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 & value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_uint8:
        {
            uint8_t value2 = top()->basic_types.uint8__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 & value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 & value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 & value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 & value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 & value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::bit_xor (cx_type *lhs, cx_type *rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 ^ value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 ^ value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 ^ value2));
                }
                    break;

                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 ^ value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 ^ value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_char:
        {
            char value2 = top()->basic_types.char__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 ^ value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 ^ value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 ^ value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 ^ value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 ^ value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_wchar:
        {
            wchar_t value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 ^ value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 ^ value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 ^ value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 ^ value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 ^ value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_bool:
        {
            bool value2 = top()->basic_types.bool__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 ^ value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 ^ value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 ^ value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 ^ value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 ^ value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_uint8:
        {
            uint8_t value2 = top()->basic_types.uint8__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 ^ value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 ^ value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 ^ value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 ^ value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 ^ value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::bit_or (cx_type *lhs, cx_type *rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 | value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 | value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 | value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 | value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 | value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_char:
        {
            char value2 = top()->basic_types.char__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 | value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 | value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 | value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 | value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 | value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_wchar:
        {
            wchar_t value2 = top()->basic_types.int__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 | value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 | value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 | value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 | value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 | value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_bool:
        {
            bool value2 = top()->basic_types.bool__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 | value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 | value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 | value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 | value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 | value2));
                }
                    break;
                default:break;
            }
        }
            break;

        case cx_uint8:
        {
            uint8_t value2 = top()->basic_types.uint8__;
            pop();
            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 | value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 | value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 | value2));
                }
                    break;

                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 | value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 | value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}