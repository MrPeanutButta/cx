/** Executor (expressions)
 * exec_expression.cpp
 *
 * Executes Cx expressions
 */
#include <iostream>
#include "exec.h"
#include "common.h"

using namespace std;

/** execute_expression   Execute an expression (binary relational
 *                      operators = < > <> <= and >= ).
 *
 * @return: ptr to expression's type object
 */
cx_type *cx_executor::execute_expression(void) {
    cx_type *pOperand1Type; // ptr to first  operand's type
    cx_type *pOperand2Type; // ptr to second operand's type
    cx_type *pResultType; // ptr to result type
    cx_token_code op; // operator

    // Execute the first simple expression.
    pResultType = execute_simple_expression();

    // If we now see a relational operator,
    // execute the second simple expression.
    if (token_in(token, tokenlist_relation_ops)) {
        op = token;
        pOperand1Type = pResultType->Base();
        pResultType = pBooleanType;

        get_token();
        pOperand2Type = execute_simple_expression()->Base();

        // Perform the operation, and push the resulting value
        // onto the stack.
        if (((pOperand1Type == pIntegerType) &&
                (pOperand2Type == pIntegerType))
                || ((pOperand1Type == pCharType) &&
                (pOperand2Type == pCharType))
                || (pOperand1Type->form == fcEnum)) {

            // integer <op> integer
            // boolean <op> boolean
            // char    <op> char
            // enum    <op> enum
            int value1, value2;
            if (pOperand1Type == pCharType) {
                value2 = pop()->char__;
                value1 = pop()->char__;
            } else {
                value2 = pop()->int__;
                value1 = pop()->int__;
            }

            switch (op) {

                case tc_equal_equal:
                    push(value1 == value2);
                    break;

                case tc_not_equal:
                    push(value1 != value2);
                    break;

                case tc_lessthan:
                    push(value1 < value2);
                    break;

                case tc_greaterthan:
                    push(value1 > value2);
                    break;

                case tc_lessthan_equal:
                    push(value1 <= value2);
                    break;

                case tc_greaterthan_equal:
                    push(value1 >= value2);
                    break;
            }
        } else if ((pOperand1Type == pFloatType) ||
                (pOperand2Type == pFloatType)) {

            // real    <op> real
            // real    <op> integer
            // integer <op> real
            float value2 = pOperand2Type == pFloatType ? pop()->float__
                    : pop()->int__;
            float value1 = pOperand1Type == pFloatType ? pop()->float__
                    : pop()->int__;

            switch (op) {

                case tc_equal_equal:
                    push(value1 == value2);
                    break;

                case tc_not_equal:
                    push(value1 != value2);
                    break;

                case tc_lessthan:
                    push(value1 < value2);
                    break;

                case tc_greaterthan:
                    push(value1 > value2);
                    break;

                case tc_lessthan_equal:
                    push(value1 <= value2);
                    break;

                case tc_greaterthan_equal:
                    push(value1 >= value2);
                    break;
            }
        } else {

            // string <op> string
            char *addr2 = (char *) pop()->addr__;
            char *addr1 = (char *) pop()->addr__;

            int cmp = strncmp(addr1, addr2, pOperand1Type->size);

            switch (op) {
                case tc_equal_equal: push(cmp == 0);
                    break;
                case tc_not_equal: push(cmp != 0);
                    break;
                case tc_lessthan: push(cmp < 0);
                    break;
                case tc_greaterthan: push(cmp > 0);
                    break;
                case tc_lessthan_equal: push(cmp <= 0);
                    break;
                case tc_greaterthan_equal: push(cmp >= 0);
                    break;
            }
        }
    }

    return pResultType;
}

/** execute_simple_expression    Execute a simple expression
 *                             (unary operators + or -
 *                             and binary operators + -
 *                             and OR).
 *
 * @return: ptr to expression's type object
 */
cx_type *cx_executor::execute_simple_expression(void) {

    cx_type *pOperandType; // ptr to operand's type
    cx_type *pResultType; // ptr to result type
    cx_token_code op; // operator
    cx_token_code unaryOp = tc_plus; // unary operator

    // Unary + or -
    if (token_in(token, tokenlist_unary_ops)) {
        unaryOp = token;
        get_token();
    }

    // Execute the first term.
    pResultType = execute_term();

    switch (unaryOp) {
        case tc_minus:
            if (pResultType == pFloatType) push(-pop()->float__);
            else push(-pop()->int__);
            break;
        case tc_bit_NOT:
            push(~(pop()->int__));
            break;
    }

    // Loop to execute subsequent additive operators and terms.
    while (token_in(token, tokenlist_add_ops)) {
        op = token;
        pResultType = pResultType->Base();

        get_token();
        pOperandType = execute_term()->Base();

        switch (op) {
            case tc_plus:
            case tc_minus:
            {
                if ((pResultType == pIntegerType) &&
                        (pOperandType == pIntegerType)) {

                    // integer +|- integer
                    int value2 = pop()->int__;
                    int value1 = pop()->int__;

                    push(op == tc_plus ? value1 + value2
                            : value1 - value2);
                    pResultType = pIntegerType;
                } else {

                    // real    +|- real
                    // real    +|- integer
                    // integer +|- real
                    float value2 = pOperandType == pFloatType ? pop()->float__
                            : pop()->int__;
                    float value1 = pResultType == pFloatType ? pop()->float__
                            : pop()->int__;

                    push(op == tc_plus ? value1 + value2
                            : value1 - value2);
                    pResultType = pFloatType;
                }

            }
                break;
            case tc_bit_leftshift:
            {
                // bit left shift
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                push(value1 << value2);
                pResultType = pIntegerType;
            }
                break;
            case tc_bit_rightshift:
            {
                // bit right shift
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                push(value1 >> value2);
                pResultType = pIntegerType;
            }
                break;
            case tc_bit_AND:
            {
                // bit and
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                push(value1 & value2);
                pResultType = pIntegerType;
            }
                break;
            case tc_bit_XOR:
            {
                // bit XOR
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                push(value1 ^ value2);
                pResultType = pIntegerType;
            }
                break;
            case tc_bit_OR:
            {
                // bit OR
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                push(value1 | value2);
                pResultType = pIntegerType;
            }
                break;
            case tc_logic_OR:
            {
                // boolean OR boolean
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                push(value1 || value2);
                pResultType = pBooleanType;
            }
                break;
        }
    }

    return pResultType;
}

/** execute_term         Execute a term (binary operators * /
 *                      % and &&).
 *
 * @return: ptr to term's type object
 */
cx_type *cx_executor::execute_term(void) {
    cx_type *pOperandType; // ptr to operand's type
    cx_type *pResultType; // ptr to result type
    cx_token_code op; // operator

    // Execute the first factor.
    pResultType = execute_factor();

    // Loop to execute subsequent multiplicative operators and factors.
    while (token_in(token, tokenlist_mul_ops)) {
        op = token;
        pResultType = pResultType->Base();

        get_token();
        pOperandType = execute_factor()->Base();

        bool divZeroFlag = false;

        switch (op) {
            case tc_star:
                if ((pResultType == pIntegerType) &&
                        (pOperandType == pIntegerType)) {

                    // integer * integer
                    int value2 = pop()->int__;
                    int value1 = pop()->int__;

                    push(value1 * value2);
                    pResultType = pIntegerType;
                } else {

                    // real    * real
                    // real    * integer
                    // integer * real
                    float value2 = pOperandType == pFloatType
                            ? pop()->float__
                            : pop()->int__;
                    float value1 = pResultType == pFloatType
                            ? pop()->float__
                            : pop()->int__;

                    push(value1 * value2);
                    pResultType = pFloatType;
                }
                break;
            case tc_divide:
            {

                // real    / real
                // real    / integer
                // integer / real
                // integer / integer

                float value2 = pOperandType == pFloatType
                        ? pop()->float__
                        : pop()->int__;
                float value1 = pResultType == pFloatType
                        ? pop()->float__
                        : pop()->int__;

                if (value2 == 0.0f) cx_runtime_error(rte_division_by_zero);

                push(value1 / value2);
                pResultType = pFloatType;
            }
                break;
            case tc_modulas:
            {
                // integer MOD integer
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                if (value2 == 0) cx_runtime_error(rte_division_by_zero);

                push(value1 % value2);
                pResultType = pIntegerType;
            }
                break;
            case tc_logic_AND:
            {
                // boolean AND boolean
                int value2 = pop()->int__;
                int value1 = pop()->int__;

                push(value1 && value2);
                pResultType = pBooleanType;
            }
                break;
        }
    }

    return pResultType;
}

/** execute_factor       Execute a factor (identifier, number,
 *                      string, NOT <factor>, or parenthesized
 *                      subexpression).  An identifier can be
 *                      a function, constant, or variable.
 *
 * @return: ptr to factor's type object
 */
cx_type *cx_executor::execute_factor(void) {
    cx_type *pResultType = nullptr; // ptr to result type
    cx_symtab_node *p_id = nullptr;

    switch (token) {
        case tc_identifier:
        {
            switch (p_node->defn.how) {

                case dc_function:
                    pResultType = execute_subroutine_call(p_node);
                    break;

                case dc_constant:
                    pResultType = execute_constant(p_node);
                    break;
                case dc_type:
                    pResultType = p_node->p_type;
                    get_token();
                    break;
                default:
                    p_id = p_node;
                    get_token();

                    if (token_in(token, tokenlist_assign_ops)) {
                        execute_assignment(p_id);
                        pResultType = execute_variable(p_id, false);
                    } else {
                        pResultType = execute_variable(p_id, false);
                    }

                    break;
            }
        }
            break;

        case tc_number:
        {
            // push the number's integer or real value onto the stack.
            if (p_node->p_type == pIntegerType) {
                push(p_node->defn.constant.value.int__);
            } else {
                push(p_node->defn.constant.value.float__);
            }
            pResultType = p_node->p_type;
            get_token();
        }
            break;
        case tc_char:
        case tc_string:
        {
            //get_token();
            // push either a character or a string address onto the
            // runtime stack, depending on the string length.
            //int length = strlen(p_node->string__()) - 2; // skip quotes
            if (p_node->string_length == 1) {

                // Character
                push(p_node->defn.constant.value.char__);
                pResultType = pCharType;
            } else {

                // string__ address
                push(p_node->defn.constant.value.p_string);
                pResultType = p_node->p_type;
            }

            get_token();
        }
            break;
        case tc_logic_NOT:
            // Execute boolean factor and invert its value.
            get_token();
            execute_factor();

            push(1 - pop()->int__);
            pResultType = pBooleanType;
            break;
        case tc_left_paren:
        {

            // Parenthesized subexpression:  Call execute_expression
            //                               recursively.
            get_token(); // first token after (

            pResultType = execute_expression();

            get_token(); // first token after )
        }
            break;
        case tc_semicolon:
            break;
    }

    return pResultType;
}

/** execute_constant     push a constant onto the runtime stack.
 *
 * @param p_id : ptr to constant identifier's symbol table node
 *
 * @return: ptr to constant's type object
 */
cx_type *cx_executor::execute_constant(const cx_symtab_node *p_id) {
    cx_type *p_type = p_id->p_type;
    const cx_data_value *value = &p_id->defn.constant.value;

    if (p_type == pFloatType) push(value->float__);
    else if (p_type == pCharType) push(value->char__);
    else if (p_type->form == fcArray) push(value->p_string);
    else push(value->int__);

    get_token();
    trace_data_fetch(p_id, top_of_stack(), p_type);
    return p_type;
}

/**  execute_variable     push a variable's value or address onto
 *                        the runtime stack.
 *
 * @param p_id         : ptr to variable's symbol table node
 * @param address_flag : true to push address, false to push value
 *
 * @return: ptr to variable's type object
 */
cx_type *cx_executor::execute_variable(const cx_symtab_node *p_id,
        bool address_flag) {

    cx_type *p_type = p_id->p_type;

    if (p_type->form == fcStream) return p_type;

    // get the variable's runtime stack address.
    cx_stack_item *pEntry = run_stack.get_value_address(p_id);
    push((p_id->defn.how == dc_reference) || (!p_type->IsScalar())
            ? pEntry->addr__ : pEntry);

    // Loop to execute any subscripts and field designators,
    // which will modify the data address at the top of the stack.
    int doneFlag = false;
    do {
        switch (token) {

            case tc_left_subscript:

                p_type = execute_subscripts(p_type);

                break;

            case tc_dot:
                p_type = execute_field();
                break;

            default: doneFlag = true;
        }
    } while (!doneFlag);

    //if (!token_in(token, tokenlist_assign_ops))get_token();

    // If address_flag is false, and the data is not an array
    // or a record, replace the address at the top of the stack
    // with the data value.
    if ((!address_flag) && (p_type->IsScalar())) {
        if (p_type == pFloatType) {
            push(((cx_stack_item *) pop()->addr__)->float__);
        } else if (p_type->Base() == pCharType) {
            push(((cx_stack_item *) pop()->addr__)->char__);
        } else {
            push(((cx_stack_item *) pop()->addr__)->int__);
        }
    }

    if (!address_flag) {
        void *p_data_value = p_type->IsScalar() ? top_of_stack() : top_of_stack()->addr__;

        trace_data_fetch(p_id, p_data_value, p_type);
    }

    return p_type;
}

/**  execute_subscripts   Execute each subscript expression to
 *                      modify the data address at the top of
 *                      the runtime stack.
 *
 * @param p_type : ptr to array type object
 *
 * @return: ptr to subscripted variable's type object
 */
cx_type *cx_executor::execute_subscripts(const cx_type *p_type) {
    // Loop to executed subscript lists enclosed in brackets.
    while (token == tc_left_subscript) {

        // Loop to execute comma-separated subscript expressions
        // within a subscript list.
        do {
            get_token(); // index
            execute_expression();

            // Evaluate and range check the subscript.
            int value = pop()->int__;
            range_check(p_type, value);


            // Modify the data address at the top of the stack.
            push(((char *) pop()->addr__) +
                    p_type->array.pElmtType->size * (value - p_type->array.minIndex));

            // Prepare for another subscript in this list.
            if (token == tc_comma) p_type = p_type->array.pElmtType;

        } while (token == tc_comma);

        // Prepare for another subscript list.
        get_token(); // ]
        if (token == tc_left_subscript) p_type = p_type->array.pElmtType;
    }

    return p_type->array.pElmtType;
}

/** execute_field         Execute a field designator to modify the
 *                       data address at the top of the runtime
 *                       stack
 *
 * @return: ptr to record field's type object
 */
cx_type *cx_executor::execute_field(void) {
    get_token();
    cx_symtab_node *pFieldId = p_node;

    push(((char *) (pop()->addr__)) + pFieldId->defn.data.offset);

    get_token();
    return pFieldId->p_type;
}
