#include "exec.h"
#include "common.h"

void cx_executor::unary_negate(cx_type* expr) {
    switch (expr->type_code) {
        case cx_int:
        {
            int value = top()->basic_types.int__;
            pop();
            push((int) -(value));
        }
            break;
        case cx_char:
        {
            char value = top()->basic_types.char__;
            pop();
            push((char) -(value));
        }
            break;
        case cx_wchar:
        {
            wchar_t value = top()->basic_types.wchar__;
            pop();
            push((wchar_t) -(value));
        }
            break;
        case cx_float:
        {
            float value = top()->basic_types.float__;
            pop();
            push((float) -(value));
        }
            break;
        case cx_uint8:
        {
            uint8_t value = top()->basic_types.uint8__;
            pop();
            push((uint8_t) -(value));
        }
            break;
        default:break;
    }
}

void cx_executor::unary_bit_not(cx_type* expr) {
    switch (expr->type_code) {
        case cx_int:
        {
            int value = top()->basic_types.int__;
            pop();
            push((int) ~(value));
        }
            break;
        case cx_char:
        {
            char value = top()->basic_types.char__;
            pop();
            push((char) ~(value));
        }
            break;
        case cx_wchar:
        {
            wchar_t value = top()->basic_types.wchar__;
            pop();
            push((wchar_t) ~(value));
        }
            break;
        case cx_uint8:
        {
            uint8_t value = top()->basic_types.uint8__;
            pop();
            push((uint8_t) ~(value));
        }
            break;
        default:break;
    }
}
