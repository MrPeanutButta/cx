#include <cstdio>
#include <cstring>
#include <iostream>
#include "common.h"
#include "buffer.h"
#include "exec.h"

using namespace std;

/** trace_routine_entry   Trace the entry into a routine.
 * 
 * @param p_function_id : ptr to the routine name's symbol table node.
 */
void cx_executor::trace_routine_entry(const cx_symtab_node *p_function_id) {
    if (trace_routine_flag) {
        cout << ">> Entering routine " << p_function_id->string__() << endl;
    }
}

/** trace_routine_exit    Trace the exit from a routine.
 * 
 * @param p_function_id : ptr to the routine name's symbol table node
 */
void cx_executor::trace_routine_exit(const cx_symtab_node *p_function_id) {
    if (trace_routine_flag) {
        cout << ">> Exiting routine " << p_function_id->string__() << endl;
    }
}

/** trace_statement      Trace the execution of a statement.
 */
void cx_executor::trace_statement(void) {
    if (trace_statement_flag) cout << ">>  At " << current_line_number
            << endl;
}

/** trace_data_store      Trace the storing of data into a
 *                      variable or formal parameter.
 * 
 * @param p_target_id  : ptr to the target name's symbol table node.
 * @param p_data_value : ptr to the data value.
 * @param p_data_type  : ptr to the data's type object.
 */
void cx_executor::trace_data_store(const cx_symtab_node *p_target_id,
        const void *p_data_value,
        const cx_type *p_data_type) {
    if (trace_store_flag) {
        TFormCode form = p_target_id->p_type->form;

        cout << ">>   " << p_target_id->string__();
        if (form == fcArray) cout << "[*]";
        else if (form == fcComplex) cout << ".*";
        cout << " <== ";

        trace_data_value(p_data_value, p_data_type);
    }
}

/** trace_data_fetch      Trace the fetching of data from a
 *                      variable or formal parameter.
 * 
 * @param p_id        : ptr to the variable name's symbol table node.
 * @param p_data_value : ptr to the data value.
 * @param p_data_type  : ptr to the data's type object.
 */
void cx_executor::trace_data_fetch(const cx_symtab_node *p_id,
        const void *p_data_value,
        const cx_type *p_data_type) {
    if (trace_fetch_flag) {
        TFormCode form = p_id->p_type->form;

        cout << ">>   " << p_id->string__();
        if (form == fcArray) cout << "[*]";
        else if (form == fcComplex) cout << ".*";
        cout << ": ";

        trace_data_value(p_data_value, p_data_type);
    }
}

/** trace_data_value      Trace a data value.
 * 
 * @param p_data_value : ptr to the data value.
 * @param p_data_type  : ptr to the data's type object.
 */
void cx_executor::trace_data_value(const void *p_data_value,
        const cx_type *p_data_type) {

    if (p_data_type->form == fcStream) return;

    char text[max_input_buffer_size]; // text for value

    if (p_data_type == pFloatType) {
        sprintf(text, "%0.6g", ((cx_stack_item *) p_data_value)->float__);
    } else if (p_data_type == pCharType) {
        sprintf(text, "'%c'", ((cx_stack_item *) p_data_value)->char__);
    } else if (p_data_type == pBooleanType) {
        strcpy(text, ((cx_stack_item *) p_data_value)->int__ == 0
                ? "false" : "true");
    } else if (p_data_type->form == fcArray) {
        if (p_data_type->array.pElmtType == pCharType) {
            int length = p_data_type->array.elmtCount;
            memcpy(text + 1, p_data_value, length);
            text[0] = '\'';
            text[length + 1] = '\'';
            text[length + 2] = '\0';
        } else strcpy(text, "<array>");
    } else if (p_data_type->form == fcComplex) {
        strcpy(text, "<complex>");
    } else if (p_data_type->Base()->form == fcEnum) {
        int count = ((cx_stack_item *) p_data_value)->int__;
        cx_symtab_node *p_id = p_data_type->Base()->enumeration.pConstIds;
        while (--count >= 0) p_id = p_id->next__;
        strcpy(text, p_id->string__());
    } else {
        cx_stack_item *tmp = (cx_stack_item *) p_data_value;
        sprintf(text, "%d", tmp->int__);
    }

    cout << text << endl;
}