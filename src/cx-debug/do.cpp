#include "cx-debug/exec.h"

/** execute_DO   Executes do/while statement while <expression> is true.
 *
 *      do
 *      <staement>;
 *      while(<expression>);
 *
 * @param p_function_id : routine ID this statement is apart of.
 */
void cx_executor::execute_DO (cx_symtab_node * p_function_id) {

    int break_point;
    int at_loop_start = current_location(); // location of loop start in icode;
    bool condition = false;

    do {

        get_token(); // do
        break_point = get_location_marker();
        get_token();

        execute_statement_list(p_function_id, tc_WHILE);

        if (break_loop) {
            go_to(break_point);
            get_token();
            break;
        }

        get_token(); //while
        execute_expression(); // (condition)

        condition = top()->basic_types.bool__;
        pop();

        if (condition) this->go_to(at_loop_start);
    } while (current_location() == at_loop_start);

    // reset break flag
    break_loop = false;

}