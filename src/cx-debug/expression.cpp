/** Executor (expressions)
 * exec_expression.cpp
 *
 * Executes Cx expressions
 */
#include "exec.h"
#include "common.h"

/** execute_expression   Execute an expression (binary relational
 *                      operators = < > <> <= and >= ).
 *
 * @return: ptr to expression's type object
 */
cx_type *cx_executor::execute_expression (void) {
    cx_type *p_operand1_type; // ptr to first  operand's type
    cx_type *p_operand2_type; // ptr to second operand's type
    cx_type *p_result_type; // ptr to result type
    cx_token_code op; // operator

    // Execute the first simple expression.
    p_result_type = execute_simple_expression();

    // If we now see a relational operator,
    // execute the second simple expression.
    if (token_in(token, tokenlist_relation_ops)) {
        op = token;
        p_operand1_type = p_result_type->base_type();
        p_result_type = p_boolean_type;

        get_token();
        p_operand2_type = execute_simple_expression()->base_type();

        execute_relational(op, p_operand1_type, p_operand2_type);
    }

    return p_result_type;
}

/** execute_simple_expression    Execute a simple expression
 *                             (unary operators + or -
 *                             and binary operators + -
 *                             and OR).
 *
 * @return: ptr to expression's type object
 */
cx_type *cx_executor::execute_simple_expression (void) {

    cx_type *p_operand_type; // ptr to operand's type
    cx_type *p_result_type; // ptr to result type
    cx_token_code op; // operator
    cx_token_code unary_op = tc_plus; // unary operator

    // Unary + or -
    if (token_in(token, tokenlist_unary_ops)) {
        unary_op = token;
        get_token();
    }

    // Execute the first term.
    p_result_type = execute_term();

    switch (unary_op) {
        case tc_minus:
            unary_negate(p_result_type);
            break;
        case tc_bit_NOT:
            unary_bit_not(p_result_type);
            break;
        default:
            break;
    }

    // Loop to execute subsequent additive operators and terms.
    while (token_in(token, tokenlist_add_ops)) {
        op = token;
        //p_result_type = p_result_type->base_type();

        get_token();
        p_operand_type = execute_expression();

        p_result_type = execute_additive(op, p_result_type, p_operand_type);

    }

    return p_result_type;
}

/** execute_term         Execute a term (binary operators * /
 *                      % and &&).
 *
 * @return: ptr to term's type object
 */
cx_type *cx_executor::execute_term (void) {
    cx_type *p_operand_type; // ptr to operand's type
    cx_type *p_result_type; // ptr to result type
    cx_token_code op; // operator

    // Execute the first factor.
    p_result_type = execute_factor();

    // Loop to execute subsequent multiplicative operators and factors.
    while (token_in(token, tokenlist_mul_ops)) {
        op = token;
        p_result_type = p_result_type->base_type();

        get_token();
        p_operand_type = execute_factor()->base_type();

        p_result_type = execute_multiplicative(op, p_result_type, p_operand_type);

    }

    return p_result_type;
}