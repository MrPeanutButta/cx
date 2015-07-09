#include "exec.h"
#include "common.h"

cx_type *cx_executor::execute_multiplicative(cx_token_code op, cx_type *lhs, cx_type *rhs) {
    //        bool div_zero_flag = false;    
    cx_type *p_result_type = lhs;

    switch (op) {
        case tc_star:
            multiply(lhs, rhs);
            break;
        case tc_divide:
            divide(lhs, rhs);
            break;
        case tc_modulas:
            modulas(lhs, rhs);
            break;
        case tc_logic_AND:
            logic_and(lhs, rhs);
            p_result_type = p_boolean_type;
            break;
        default:
            break;
    }

    return p_result_type;
}

void cx_executor::multiply(cx_type* lhs, cx_type* rhs) {
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
                    push(int(value1 * value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 * value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 * value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 * value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 * value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 * value2));
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
                    push(int(value1 * value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 * value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 * value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 * value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 * value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 * value2));
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
                    push(int(value1 * value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 * value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 * value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 * value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 * value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 * value2));
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
                    push(int(value1 * value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 * value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 * value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 * value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 * value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 * value2));
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
                    push(int(value1 * value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 * value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 * value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 * value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 * value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 * value2));
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
                    push(int(value1 * value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 * value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 * value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 * value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 * value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 * value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::divide(cx_type* lhs, cx_type* rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 / value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 / value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 / value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 / value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 / value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 / value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 / value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 / value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 / value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 / value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 / value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 / value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 / value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 / value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 / value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 / value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 / value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 / value2));
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

            if (value2 == 0.0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 / value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 / value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 / value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 / value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 / value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 / value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 / value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 / value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 / value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 / value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 / value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 / value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 / value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 / value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 / value2));
                }
                    break;
                case cx_float:
                {
                    float value1 = top()->basic_types.float__;
                    pop();
                    push(float(value1 / value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 / value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 / value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}

void cx_executor::modulas(cx_type* lhs, cx_type* rhs) {
    switch (rhs->type_code) {
        case cx_int:
        {
            int value2 = top()->basic_types.int__;
            pop();

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 % value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 % value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 % value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 % value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 % value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 % value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 % value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 % value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 % value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 % value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 % value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 % value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 % value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 % value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 % value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 % value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 % value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 % value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 % value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 % value2));
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

            if (value2 == 0)cx_runtime_error(rte_division_by_zero);

            switch (lhs->type_code) {
                case cx_int:
                {
                    int value1 = top()->basic_types.int__;
                    pop();
                    push(int(value1 % value2));
                }
                    break;
                case cx_char:
                {
                    char value1 = top()->basic_types.char__;
                    pop();
                    push(char(value1 % value2));
                }
                    break;
                case cx_wchar:
                {
                    wchar_t value1 = top()->basic_types.wchar__;
                    pop();
                    push(wchar_t(value1 % value2));
                }
                    break;
                case cx_bool:
                {
                    bool value1 = top()->basic_types.bool__;
                    pop();
                    push(bool(value1 % value2));
                }
                    break;
                case cx_uint8:
                {
                    uint8_t value1 = top()->basic_types.uint8__;
                    pop();
                    push(uint8_t(value1 % value2));
                }
                    break;
                default:break;
            }
        }
            break;
        default:break;
    }
}