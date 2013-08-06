#include <cstdio>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"
//#include "complist.h"

extern cx_symtab_node *pProgram_ptr;

/** parse       parse the source file.  After listing each
 *              source line, extract and list its tokens.
 * 
 * @return ptr to '__cx_global__' program Id.
 */
cx_symtab_node *cx_parser::parse(void) {

    extern bool cx_dev_debug_flag;

    cx_symtab_node *p_program_id = new cx_symtab_node("__cx_global__", dc_program);
    p_program_id->defn.routine.which = rc_declared;
    p_program_id->defn.routine.parm_count = 0;
    p_program_id->defn.routine.total_parm_size = 0;
    p_program_id->defn.routine.total_local_size = 0;
    p_program_id->defn.routine.locals.p_parms_ids = NULL;
    p_program_id->defn.routine.locals.p_constant_ids = NULL;
    p_program_id->defn.routine.locals.p_type_ids = NULL;
    p_program_id->defn.routine.locals.p_variable_ids = NULL;
    p_program_id->defn.routine.locals.p_function_ids = NULL;
    p_program_id->defn.routine.p_symtab = NULL;
    p_program_id->defn.routine.p_icode = NULL;
    SetType(p_program_id->p_type, pIntegerType);

    pProgram_ptr = p_program_id;
    icode.reset();

    current_nesting_level = 0;
    // enter the nesting level 1 and open a new scope for the program.
    symtab_stack.set_current_symtab(&cx_global_symtab);
    icode.put(tc_left_bracket);
    get_token_append();

    parse_statement_list(p_program_id, tc_end_of_file);
    fixup_location_marker(p_program_id->global_finish_location);
    p_program_id->defn.routine.return_marker = put_location_marker();

    get_token_append();

    p_program_id->defn.routine.p_symtab = &cx_global_symtab; //symtab_stack.exit_scope();

    resync(tokenlist_program_end);
    conditional_get_token_append(tc_end_of_file, err_missing_right_bracket);

    if (cx_dev_debug_flag) {
        list.put_line();
        sprintf(list.text, "%20d source lines.", current_line_number);
        list.put_line();
        sprintf(list.text, "%20d syntax errors.", error_count);
        list.put_line();
    }

    return p_program_id;

}

/** resync          Resynchronize the parser.  If the current
 *                  token is not in one of the token lists,
 *                  flag it as an error and then skip tokens
 *                  up to one that is in a list or end of file.
 * 
 * @param p_list1 : token list.
 * @param p_list2 : token list.
 * @param p_list3 : token list.
 */
void cx_parser::resync(const cx_token_code* p_list1,
        const cx_token_code* p_list2,
        const cx_token_code* p_list3) {

    bool errorFlag = (!token_in(token, p_list1)) &&
            (!token_in(token, p_list2)) &&
            (!token_in(token, p_list3));

    if (errorFlag) {
        cx_error_code errorCode = token == tc_end_of_file
                ? err_unexpected_eof
                : err_unexpected_token;

        cx_error(errorCode);

        while ((!token_in(token, p_list1)) &&
                (!token_in(token, p_list2)) &&
                (!token_in(token, p_list3)) &&
                (token != tc_RETURN) &&
                (token != tc_end_of_file)) {
            get_token_append();
        }

        if ((token == tc_end_of_file) &&
                (errorCode != err_unexpected_eof)) {
            cx_error(err_unexpected_eof);
        }
    }
}
