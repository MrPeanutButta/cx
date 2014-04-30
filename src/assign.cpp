#include "exec.h"
#include "common.h"
#include "types.h"
#include "members.h"

/** execute_assignment 	Execute assignment.
 *
 *      p_target_id =, +=, -=, ++, --, /=, *=, %=, ^=
 *                >>=, <<=, &=, |= <expression>;
 *
 * @param p_target_id : ptr to the symtab node being assigned some value
 *                    on the stack.
 */
void cx_executor::execute_assignment(cx_symtab_node *p_target_id) {

    cx_stack_item *p_target = nullptr; // runtime stack address of target
    cx_type *p_target_type = nullptr; // ptr to target type object
    cx_type *p_expr_type = nullptr; // ptr to expression type object

    void *p_target_address = nullptr;

    if (p_target_id->defn.how == dc_function) {
        p_target_type = p_target_id->p_type;
        if (p_target_type == p_void_type) return;
        p_target = run_stack.get_value_address(p_target_id);
    }/* Assignment to variable or formal parameter.
      * execute_variable leaves the target address on
      * top of the runtime stack. */
    else if ((p_target_id->defn.how != dc_type)) {

        if (!token_in(token, tokenlist_assign_ops) &&
                !token_in(token, tokenlist_subscript_or_field_start))get_token();

        p_target_type = execute_variable(p_target_id, true);

        if (p_target_type->form != fc_stream) {
            if (!p_target_type->is_scalar_type()) {
                p_target_address = top()->basic_types.addr__;
                pop();
            } else {
                p_target = (cx_stack_item *) top()->basic_types.addr__;
                pop();
            }
        }
    }

    switch (token) {
        case tc_RETURN:
        case tc_equal:
        {
            get_token();
            assign(p_target_id, p_target_type,
                    execute_expression(),
                    p_target, p_target_address);
            pop();
        }
            break;
        case tc_plus_plus:
        {
            get_token();
            plus_plus(p_target_type, p_target);
        }
            break;
        case tc_minus_minus:
        {
            get_token();
            minus_minus(p_target_type, p_target);
        }
            break;
        case tc_plus_equal:
        {
            get_token();
            plus_equal(p_target_id, p_target_type, execute_expression(),
                    p_target, p_target_address);
            pop();

        }
            break;
        case tc_minus_equal:
        {
            get_token();
            minus_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_star_equal:
        {
            get_token();
            star_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_divide_equal:
        {
            get_token();
            divide_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_modulas_equal:
        {
            get_token();
            modulas_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_bit_leftshift_equal:
        {
            get_token();
            leftshift_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_bit_rightshift_equal:
        {
            get_token();
            rightshift_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_bit_AND_equal:
        {
            get_token();
            and_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_bit_XOR_equal:
        {
            get_token();
            xor_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        case tc_bit_OR_equal:
        {
            get_token();
            or_equal(p_target_id, p_target_type, execute_expression(),
                    p_target);
            pop();
        }
            break;
        default:
            break;
    }

    if (p_target_id != nullptr) {
        if ((p_target_id->defn.how == dc_function) ||
                (p_target_id->defn.how == dc_namespace)) {
            trace_data_store(p_target_id, *p_target, p_target_type);
        } else if (p_target_id->runstack_item != nullptr) {
            trace_data_store(p_target_id, *p_target_id->runstack_item, p_target_type);
        }
    }
}

void cx_executor::assign(cx_symtab_node* p_target_id,
        cx_type* p_target_type, cx_type* p_expr_type, cx_stack_item* p_target,
        void* &p_target_address) {

    if (p_target_type->is_scalar_type()) {

        memcpy(&p_target->basic_types, &top()->basic_types, p_target_type->size);

    } else {
        cx_malloc(p_target_id, p_target_type, p_expr_type, p_target, p_target_address);
    }
}

void cx_executor::plus_plus(cx_type* p_target_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;

    if (p_target_type->is_scalar_type()) {
        switch (target_type) {
            case cx_int:
            {
                ++p_target->basic_types.int__;
            }
                break;
            case cx_char:
            {
                ++p_target->basic_types.char__;
            }
                break;
            case cx_wchar:
            {
                ++p_target->basic_types.wchar__;
            }
                break;
            case cx_float:
            {
                ++p_target->basic_types.float__;
            }
                break;
            case cx_bool:
            {
                p_target->basic_types.bool__ = true;
            }
                break;
            case cx_uint8:
            {
                ++p_target->basic_types.uint8__;
            }
                break;

            default:
                break;
        }
    }
}

void cx_executor::minus_minus(cx_type* p_target_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;

    if (p_target_type->is_scalar_type()) {
        switch (target_type) {
            case cx_int:
            {
                --p_target->basic_types.int__;
            }
                break;
            case cx_char:
            {
                --p_target->basic_types.char__;
            }
                break;
            case cx_wchar:
            {
                --p_target->basic_types.wchar__;
            }
                break;
            case cx_float:
            {
                --p_target->basic_types.float__;
            }
                break;
            case cx_bool:
            {
                p_target->basic_types.bool__ = false;
            }
                break;
            case cx_uint8:
            {
                --p_target->basic_types.uint8__;
            }
                break;

            default:
                break;
        }
    }
}

void
cx_executor::plus_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target, void*& p_target_address) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    float *t_float = &p_target->basic_types.float__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;

    if (p_target_type->is_scalar_type()) {

        switch (target_type) {

            case cx_int:
            {
                switch (expr_type) {
                    case cx_int:
                    {
                        *t_int += mem->int__;
                    }
                        break;
                    case cx_char:
                    {
                        *t_int += mem->char__;
                    }
                        break;
                    case cx_wchar:
                    {
                        *t_int += mem->wchar__;
                    }
                        break;
                    case cx_float:
                    {
                        *t_int += mem->float__;
                    }
                        break;
                    case cx_uint8:
                    {
                        *t_int += mem->uint8__;
                    }
                        break;

                    default:break;
                }
            }
                break;
            case cx_char:
            {
                switch (expr_type) {
                    case cx_int:
                    {
                        *t_char += mem->int__;
                    }
                        break;
                    case cx_char:
                    {
                        *t_char += mem->char__;
                    }
                        break;
                    case cx_wchar:
                    {
                        *t_char += mem->wchar__;
                    }
                        break;
                    case cx_float:
                    {
                        *t_char += mem->float__;
                    }
                        break;
                    case cx_uint8:
                    {
                        *t_char += mem->uint8__;
                    }
                        break;

                    default:break;
                }
            }
                break;
            case cx_wchar:
            {
                switch (expr_type) {

                    case cx_int:
                    {
                        *t_wchar += mem->int__;
                    }
                        break;
                    case cx_char:
                    {
                        *t_wchar += mem->char__;
                    }
                        break;
                    case cx_wchar:
                    {
                        *t_wchar += mem->wchar__;
                    }
                        break;
                    case cx_float:
                    {
                        *t_wchar += mem->float__;
                    }
                        break;
                    case cx_uint8:
                    {
                        *t_wchar += mem->uint8__;
                    }
                        break;

                    default:
                        break;
                }
            }
                break;
            case cx_float:
            {
                switch (expr_type) {

                    case cx_int:
                    {
                        *t_float += mem->int__;
                    }
                        break;
                    case cx_char:
                    {
                        *t_float += mem->char__;
                    }
                        break;
                    case cx_wchar:
                    {
                        *t_float += mem->wchar__;
                    }
                        break;
                    case cx_float:
                    {
                        *t_float += mem->float__;
                    }
                        break;
                    case cx_uint8:
                    {
                        *t_float += mem->uint8__;
                    }
                        break;

                    default:
                        break;
                }
            }
                break;
            case cx_uint8:
            {
                switch (expr_type) {

                    case cx_int:
                    {
                        *t_uint8 += mem->int__;
                    }
                        break;
                    case cx_char:
                    {
                        *t_uint8 += mem->char__;
                    }
                        break;
                    case cx_wchar:
                    {
                        *t_uint8 += mem->wchar__;
                    }
                        break;
                    case cx_float:
                    {
                        *t_uint8 += mem->float__;
                    }
                        break;
                    case cx_uint8:
                    {
                        *t_uint8 += mem->uint8__;
                    }
                        break;

                    default:
                        break;
                }
            }
                break;
            default:
                break;
        }
    } else {
        const int size = p_expr_type->size;
        const int old_size = p_target_type->size;
        const int num_of_elements = (old_size + size) / p_expr_type->base_type()->size;

        p_target_address = realloc(p_target_address, old_size + size + 1);

        if (p_target_address == nullptr) {
            perror("realloc");
            exit(0);
        }

        char *tmp = (char *) p_target_address;

        if (p_expr_type->is_scalar_type()) {
            switch (expr_type) {

                case cx_int:
                {
                    memcpy(&tmp[old_size], &mem->int__, size);
                }
                    break;
                case cx_char:
                {
                    memcpy(&tmp[old_size], &mem->char__, size);
                }
                    break;
                case cx_wchar:
                {
                    memcpy(&tmp[old_size], &mem->wchar__, size);
                }
                    break;
                case cx_float:
                {
                    memcpy(&tmp[old_size], &mem->float__, size);
                }
                    break;
                case cx_uint8:
                {
                    memcpy(&tmp[old_size], &mem->uint8__, size);
                }
                    break;
                default:
                    break;
            }
        } else {

            char *p_source = (char *) top()->basic_types.addr__;
            memcpy(&tmp[old_size], p_source, size + 1);

            if (p_expr_type->is_temp_value) {
                free(top()->basic_types.addr__);
            }
        }

        p_target_id->runstack_item->basic_types.addr__ = p_target_address;
        p_target_id->p_type->array.element_count = num_of_elements;
        p_target_id->p_type->array.max_index = num_of_elements;
        p_target_id->p_type->size = old_size + size;

        tmp[num_of_elements] = '\0';

    }
}

void
cx_executor::minus_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    float *t_float = &p_target->basic_types.float__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;


    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int -= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int -= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int -= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_int -= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int -= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char -= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char -= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char -= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_char -= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char -= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar -= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar -= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar -= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_wchar -= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar -= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_float:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_float -= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_float -= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_float -= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_float -= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_float -= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 -= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 -= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 -= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_uint8 -= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 -= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        default:
            break;
    }
}

void
cx_executor::star_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    float *t_float = &p_target->basic_types.float__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;


    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int *= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int *= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int *= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_int *= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int *= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char *= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char *= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char *= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_char *= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char *= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar *= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar *= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar *= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_wchar *= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar *= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_float:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_float *= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_float *= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_float *= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_float *= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_float *= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 *= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 *= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 *= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_uint8 *= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 *= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}

void
cx_executor::divide_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    float *t_float = &p_target->basic_types.float__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;

    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int /= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int /= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int /= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_int /= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int /= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char /= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char /= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char /= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_char /= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char /= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar /= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar /= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar /= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_wchar /= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar /= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_float:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_float /= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_float /= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_float /= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_float /= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_float /= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 /= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 /= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 /= mem->wchar__;
                }
                    break;
                case cx_float:
                {
                    *t_uint8 /= mem->float__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 /= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}

void
cx_executor::modulas_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    //float *t_float = &p_target->basic_types.float__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;

    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int %= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int %= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int %= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int %= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char %= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char %= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char %= mem->wchar__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar %= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar %= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar %= mem->wchar__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 %= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 %= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 %= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 %= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}

void
cx_executor::leftshift_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;


    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int <<= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int <<= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int <<= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int <<= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char <<= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char <<= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char <<= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char <<= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar <<= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar <<= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar <<= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar <<= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 <<= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 <<= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 <<= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 <<= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}

void
cx_executor::rightshift_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;


    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int >>= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int >>= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int >>= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int >>= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char >>= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char >>= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char >>= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char >>= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar >>= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar >>= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar >>= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar >>= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 >>= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 >>= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 >>= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 >>= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}

void
cx_executor::and_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;

    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int &= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int &= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int &= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int &= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char &= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char &= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char &= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char &= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar &= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar &= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar &= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar &= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 &= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 &= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 &= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 &= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}

void
cx_executor::xor_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;


    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int ^= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int ^= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int ^= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int ^= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char ^= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char ^= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char ^= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char ^= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar ^= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar ^= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar ^= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar ^= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 ^= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 ^= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 ^= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 ^= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}

void
cx_executor::or_equal(const cx_symtab_node* p_target_id,
        cx_type* p_target_type, const cx_type* p_expr_type,
        cx_stack_item* p_target) {

    cx_type_code target_type = p_target_type->type_code;
    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;
    int *t_int = &p_target->basic_types.int__;
    char *t_char = &p_target->basic_types.char__;
    wchar_t *t_wchar = &p_target->basic_types.wchar__;
    uint8_t *t_uint8 = &p_target->basic_types.uint8__;


    switch (target_type) {

        case cx_int:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_int |= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_int |= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_int |= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_int |= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_char:
        {
            switch (expr_type) {
                case cx_int:
                {
                    *t_char |= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_char |= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_char |= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_char |= mem->uint8__;
                }
                    break;

                default:break;
            }
        }
            break;
        case cx_wchar:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_wchar |= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_wchar |= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_wchar |= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_wchar |= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;
        case cx_uint8:
        {
            switch (expr_type) {

                case cx_int:
                {
                    *t_uint8 |= mem->int__;
                }
                    break;
                case cx_char:
                {
                    *t_uint8 |= mem->char__;
                }
                    break;
                case cx_wchar:
                {
                    *t_uint8 |= mem->wchar__;
                }
                    break;
                case cx_uint8:
                {
                    *t_uint8 |= mem->uint8__;
                }
                    break;

                default:
                    break;
            }
        }
            break;

        default:
            break;
    }
}
