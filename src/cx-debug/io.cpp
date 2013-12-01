#include "exec.h"
#include "common.h"
#include "types.h"

void cx_executor::file_out (const cx_symtab_node* p_target_id,
                            const cx_type* p_expr_type) {

    cx_type_code expr_type = p_expr_type->type_code;

    mem_block *mem = &top()->basic_types;

    switch (expr_type) {
        case cx_int:
        {
            fprintf(p_target_id->p_type->stream.p_file_stream, "%i",
                    mem->int__);
        }
            break;
        case cx_char:
        {
            fprintf(p_target_id->p_type->stream.p_file_stream, "%c",
                    mem->char__);
        }
            break;
        case cx_wchar:
        {
            fprintf(p_target_id->p_type->stream.p_file_stream, "%c",
                    mem->wchar__);
        }
            break;
        case cx_float:
        {
            fprintf(p_target_id->p_type->stream.p_file_stream, "%f",
                    mem->float__);
        }
            break;
        case cx_uint8:
        {
            fprintf(p_target_id->p_type->stream.p_file_stream, "%u",
                    mem->uint8__);
        }
            break;
        default:
        {
            fprintf(p_target_id->p_type->stream.p_file_stream, "%s",
                    (char*) mem->addr__);
        }
            break;
    }
}
