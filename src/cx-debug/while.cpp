#include "exec.h"

/** execute_WHILE        Executes while statement while <expression> is true.
 *
 *      while(<expression>)
 *            <statement>;
 *
 * @param p_function_id : routine ID this statement is apart of.
 */
void cx_executor::execute_WHILE (cx_symtab_node * p_function_id) {

    int break_point;
    int at_loop_start = current_location();
    int condition = 0;

    do {

        get_token(); // while
        break_point = get_location_marker();
        get_token();

        get_token(); //  (
        execute_expression();
        condition = top()->basic_types.int__;
        pop();
        get_token(); //  )
        if (condition != 0) {
            execute_statement(p_function_id);

            if (break_loop) {
                go_to(break_point);
                get_token();
                break;
            }

            go_to(at_loop_start);
        } else go_to(break_point);



    } while (current_location() == at_loop_start);

    // reset break
    break_loop = false;
}