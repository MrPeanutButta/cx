  //
        //            if (p_expr_type == p_integer_type) {
        //                int int__ = top()->basic_types.int__;
        //                pop();
        //                fprintf(p_target_id->p_type->stream.p_file_stream, "%i", int__);
        //            } else if (p_expr_type == p_float_type) {
        //                float float__ = top()->basic_types.float__;
        //                pop();
        //                fprintf(p_target_id->p_type->stream.p_file_stream, "%f", float__);
        //            } else if (p_expr_type == p_char_type) {
        //                char char__ = top()->basic_types.char__;
        //                pop();
        //                fprintf(p_target_id->p_type->stream.p_file_stream, "%c", char__);
        //            } else {
        //                void *p_source = top()->basic_types.addr__;
        //                pop();
        //                fprintf(p_target_id->p_type->stream.p_file_stream, "%s", (char *) p_source);
        //            }
        //        }
