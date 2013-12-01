#include "exec.h"
#include "common.h"

void cx_executor::logic_or (cx_type *lhs, cx_type *rhs) {
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
                    push(int(value1 || value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 || value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 || value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 || value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 || value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 || value2));
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
                    push(int(value1 || value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 || value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 || value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 || value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 || value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 || value2));
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
                    push(int(value1 || value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 || value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 || value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 || value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 || value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 || value2));
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
                    push(int(value1 || value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 || value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 || value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 || value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 || value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 || value2));
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
                    push(int(value1 || value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 || value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 || value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 || value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 || value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 || value2));
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
                    push(int(value1 || value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 || value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 || value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 || value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 || value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 || value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::logic_and (cx_type* lhs, cx_type* rhs) {
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
                    push(int(value1 && value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 && value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 && value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 && value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 && value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 && value2));
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
                    push(int(value1 && value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 && value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 && value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 && value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 && value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 && value2));
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
                    push(int(value1 && value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 && value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 && value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 && value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 && value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 && value2));
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
                    push(int(value1 && value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 && value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 && value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 && value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 && value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 && value2));
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
                    push(int(value1 && value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 && value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 && value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 && value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 && value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 && value2));
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
                    push(int(value1 && value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 && value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 && value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 && value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 && value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 && value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::logic_not (void) {
    get_token();
    cx_type *p_result_type = execute_expression();

    switch (p_result_type->type_code) {
        case cx_int:
        {
            int value = top()->basic_types.int__;
            pop();
            push(bool(!value));
        }
            break;
        case cx_char:
        {
            char value = top()->basic_types.char__;
            pop();
            push(bool(!value));
        }
            break;
        case cx_wchar:
        {
            wchar_t value = top()->basic_types.wchar__;
            pop();
            push(bool(!value));
        }
            break;
        case cx_float:
        {
            float value = top()->basic_types.float__;
            pop();
            push(bool(!value));
        }
            break;
        case cx_bool:
        {
            bool value = top()->basic_types.bool__;
            pop();
            push(bool(!value));
        }
            break;
        case cx_uint8:
        {
            uint8_t value = top()->basic_types.uint8__;
            pop();
            push(bool(!value));
        }
            break;
        default:
        {
            void *value = top()->basic_types.addr__;
            pop();
            push(bool(!value));
        }
            break;
    }
}