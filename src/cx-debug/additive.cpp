#include "exec.h"
#include "common.h"

cx_type *cx_executor::execute_additive(cx_token_code op, cx_type* lhs, cx_type* rhs) {
    cx_type *p_result_type = lhs;

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
            p_result_type = p_boolean_type;
            break;
        default:
            break;
    }

    return p_result_type;
}

cx_type *cx_executor::plus(cx_type *lhs, cx_type *rhs) {

    cx_type *p_result_type = lhs;

    if (rhs->form != fc_array) {
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
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
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
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
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
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
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
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
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
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
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
                        push(int(value1 + value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(char(value1 + value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(wchar_t(value1 + value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(float(value1 + value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 + value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(uint8_t(value1 + value2));
                    }
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        const int size = lhs->size + rhs->size;
        const int num_of_elements = size / lhs->base_type()->size;

        void *addr2 = top()->basic_types.addr__;
        pop();

        void *addr1 = top()->basic_types.addr__;
        pop();

        cx_type *temp_type = new cx_type(fc_array, size, nullptr);
        temp_type->is_temp_value = true;

        //temp_type->type_code = lhs->type_code;
        temp_type->array.element_count = num_of_elements;
        temp_type->array.max_index = num_of_elements;
        temp_type->size = size;

        void *p_target_address = malloc(size);

        if (p_target_address == nullptr) {
            perror("malloc");
        }

        memcpy(p_target_address, addr1, lhs->size);
        char *t = (char *) p_target_address;
        memcpy(&t[lhs->size], addr2, rhs->size);

        push((void *) p_target_address);
        p_result_type = temp_type;

        if (rhs->is_temp_value) {
            //delete rhs;
            //free(addr2);
        }
    }

    return p_result_type;
}

void cx_executor::minus(cx_type *lhs, cx_type *rhs) {
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

void cx_executor::bit_leftshift(cx_type *lhs, cx_type *rhs) {
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

void cx_executor::bit_rightshift(cx_type *lhs, cx_type *rhs) {
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

void cx_executor::bit_and(cx_type *lhs, cx_type *rhs) {
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

void cx_executor::bit_xor(cx_type *lhs, cx_type *rhs) {
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

void cx_executor::bit_or(cx_type *lhs, cx_type *rhs) {
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