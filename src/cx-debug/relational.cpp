#include "exec.h"
#include "common.h"

void cx_executor::execute_relational (cx_token_code op, cx_type* lhs, cx_type* rhs) {

    switch (op) {
        case tc_equal_equal:
            equal_equal(lhs, rhs);
            break;
        case tc_not_equal:
            not_equal(lhs, rhs);
            break;
        case tc_lessthan:
            lessthan(lhs, rhs);
            break;
        case tc_greaterthan:
            greaterthan(lhs, rhs);
            break;
        case tc_lessthan_equal:
            lessthan_equal(lhs, rhs);
            break;
        case tc_greaterthan_equal:
            greaterthan_equal(lhs, rhs);
            break;
        default:
            break;
    }
}

void cx_executor::equal_equal (cx_type *lhs, cx_type *rhs) {

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
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 == value2));
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
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 == value2));
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
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 == value2));
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
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 == value2));
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
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 == value2));
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
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 == value2));
                    }
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        void *addr2 = top()->basic_types.addr__;
        pop();

        void *addr1 = top()->basic_types.addr__;
        pop();

        push(bool(memcmp(addr1, addr2, lhs->size) == 0));
    }
}

void cx_executor::not_equal (cx_type *lhs, cx_type *rhs) {

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
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 != value2));
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
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 != value2));
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
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 != value2));
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
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 != value2));
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
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 != value2));
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
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 != value2));
                    }
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        void *addr2 = top()->basic_types.addr__;
        pop();

        void *addr1 = top()->basic_types.addr__;
        pop();

        push(bool(memcmp(addr1, addr2, lhs->size) != 0));
    }
}

void cx_executor::lessthan (cx_type *lhs, cx_type *rhs) {

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
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 < value2));
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
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 < value2));
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
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 < value2));
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
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 < value2));
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
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 < value2));
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
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 < value2));
                    }
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        void *addr2 = top()->basic_types.addr__;
        pop();

        void *addr1 = top()->basic_types.addr__;
        pop();

        push(bool(memcmp(addr1, addr2, lhs->size) < 0));
    }
}

void cx_executor::lessthan_equal (cx_type *lhs, cx_type *rhs) {
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
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 <= value2));
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
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 <= value2));
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
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 <= value2));
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
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 <= value2));
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
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 <= value2));
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
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 <= value2));
                    }
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        void *addr2 = top()->basic_types.addr__;
        pop();

        void *addr1 = top()->basic_types.addr__;
        pop();

        push(bool(memcmp(addr1, addr2, lhs->size) <= 0));
    }
}

void cx_executor::greaterthan (cx_type *lhs, cx_type *rhs) {
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
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 > value2));
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
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 > value2));
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
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 > value2));
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
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 > value2));
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
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 > value2));
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
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        void *addr2 = top()->basic_types.addr__;
        pop();

        void *addr1 = top()->basic_types.addr__;
        pop();

        push(bool(memcmp(addr1, addr2, lhs->size) > 0));
    }
}

void cx_executor::greaterthan_equal (cx_type *lhs, cx_type *rhs) {
    if (rhs->form != fc_array) {
        switch (rhs->type_code) {
            case cx_int:
            {
                int value2 = top()->basic_types.int__;
                pop();
                switch (rhs->type_code) {
                    case cx_int:
                    {
                        int value1 = top()->basic_types.int__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 > value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 >= value2));
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
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 >= value2));
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
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 >= value2));
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
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 >= value2));
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
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 >= value2));
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
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_char:
                    {
                        char value1 = top()->basic_types.char__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_wchar:
                    {
                        wchar_t value1 = top()->basic_types.wchar__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_float:
                    {
                        float value1 = top()->basic_types.float__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_bool:
                    {
                        bool value1 = top()->basic_types.bool__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    case cx_uint8:
                    {
                        uint8_t value1 = top()->basic_types.uint8__;
                        pop();
                        push(bool(value1 >= value2));
                    }
                        break;
                    default:break;
                }
            }
                break;
            default:break;
        }
    } else {
        void *addr2 = top()->basic_types.addr__;
        pop();

        void *addr1 = top()->basic_types.addr__;
        pop();

        push(bool(memcmp(addr1, addr2, lhs->size) >= 0));
    }
}

