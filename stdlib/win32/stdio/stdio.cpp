// stdio.cpp : Defines the exported functions for the DLL application.
//

#include "cx_api.h"
#include <cstdio>

#if defined _WIN32
#define STDIO_API __declspec(dllexport)
#elif defined __linux__
#define STDIO_API extern "C"
#endif

STDIO_API
cx_type *cx_puts(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {
    // first param as string
    const char *_string = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // get output stream node from top of stack
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;

    bool ret_val = (p_snode->defn.io.stream != nullptr);

    if (ret_val) {
        ret_val = (bool)(std::fputs(_string, p_snode->defn.io.stream) != EOF);
    }

    p_stack->pop();

    p_stack->push((bool)ret_val);

    return cx_function_id->p_type;
}

/* Opens a file indicated by filename and returns a file stream associated
 * with that file. mode is used to determine the file access mode. */

STDIO_API
cx_type *cx_open(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // char *mode
    const char *mode = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // char *filename
    const char *filename = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // open stream
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;

    p_snode->defn.io.stream = std::fopen(filename, mode);

    p_stack->pop();
    p_stack->push((bool)(p_snode->defn.io.stream != nullptr));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_reopen(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // char *mode
    const char *mode = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // char *filename
    const char *filename = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // open stream
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;

    p_snode->defn.io.stream = std::freopen(filename, mode, p_snode->defn.io.stream);

    p_stack->pop();

    p_stack->push((bool)(p_snode->defn.io.stream != nullptr));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_close(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // attempt to close the nodes file stream
    p_stack->push((bool)(std::fclose(p_snode->defn.io.stream) != EOF));

    return cx_function_id->p_type;
}

/* Causes the output file stream to be synchronized with the actual contents of the file.
 *
 * The behavior is undefined if the given stream is of the input type or if the given stream
 * is of the update type, but the last I/O operation was not an output operation.
 *
 * If stream is NULL, all open streams are flushed. */

STDIO_API
cx_type *cx_flush(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // attempt to close the nodes file stream
    p_stack->push((bool)(std::fflush(p_snode->defn.io.stream) != EOF));

    return cx_function_id->p_type;
}

/* If mode > 0, attempts to make stream wide-oriented.
 * If mode < 0, attempts to make stream byte-oriented.
 * If mode==0, only queries the current orientation of the stream.
 */

STDIO_API
cx_type *cx_wide(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int mode
    int mode = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // attempt to widen the nodes file stream
    p_stack->push((int) std::fwide(p_snode->defn.io.stream, mode));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_read(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int count
    int count = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // int size
    int size = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // read buffer
	unsigned char *buffer = (unsigned char *)std::malloc((size * count) + 1);
    memset(buffer, 0, (size * count) + 1);

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    std::size_t s = std::fread(buffer, size, count, p_snode->defn.io.stream);
    p_stack->push((void *) buffer);

    cx_type *p_str = new cx_type(fc_array, s * size, nullptr);
    set_type(p_str->array.p_element_type, (cx_type *) cx_function_id->p_type->base_type());
    p_str->array.element_count = s;
    p_str->array.max_index = s;

    return p_str;
}

STDIO_API
cx_type *cx_write(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int count
    int count = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // int size
    int size = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // byte buffer
    const uint8_t *buffer = (const uint8_t *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((int) std::fwrite(buffer, size, count, p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_getc(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((char) std::getc(p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_gets(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int count
    int count = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // read buffer
    char *buffer = (char *) std::malloc(count + 1);
    memset(buffer, 0, count + 1);

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    std::fgets(buffer, count + 1, p_snode->defn.io.stream);
    p_stack->push((void *) buffer);

    cx_type *p_str = new cx_type(fc_array, count, nullptr);
    set_type(p_str->array.p_element_type, (cx_type *) cx_function_id->p_type->base_type());
    p_str->array.element_count = count;
    p_str->array.max_index = count;

    return p_str;
}

STDIO_API
cx_type *cx_putc(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int ch
    int ch = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((int) std::putc(ch, p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_ungetc(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int ch
    int ch = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((int) std::ungetc(ch, p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_ungetwc(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int ch
    wchar_t ch = p_stack->top()->basic_types.wchar__;
    p_stack->pop();

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((wchar_t)std::ungetwc(ch, p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_getwc(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((wchar_t)std::fgetwc(p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_getws(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int count
    int count = p_stack->top()->basic_types.int__;
    p_stack->pop();

    int size = (sizeof (wchar_t) * count) + 1;

    // read buffer
    wchar_t *buffer = (wchar_t *)std::malloc(size);
    memset(buffer, 0, size);

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    std::fgetws(buffer, count + 1, p_snode->defn.io.stream);
    p_stack->push((void *) buffer);

    cx_type *p_wstr = new cx_type(fc_array, count, nullptr);
    set_type(p_wstr->array.p_element_type, (cx_type *) cx_function_id->p_type->base_type());
    p_wstr->array.element_count = count;
    p_wstr->array.max_index = count;

    return p_wstr;
}

STDIO_API
cx_type *cx_putwc(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // int ch
    wchar_t ch = p_stack->top()->basic_types.wchar__;
    p_stack->pop();

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((wchar_t)std::putwc(ch, p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_putws(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // first param as string
    const wchar_t *_wstring = (wchar_t *)p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // get output stream node from top of stack
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;

    bool ret_val = (p_snode->defn.io.stream != nullptr);
    p_stack->pop();

    if (ret_val) {
        p_stack->push((int) std::fputws(_wstring, p_snode->defn.io.stream));
    } else {
        p_stack->push((int) EOF);
    }

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_tell(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // attempt to close the nodes file stream
    p_stack->push((int) std::ftell(p_snode->defn.io.stream));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_seek(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    int origin = p_stack->top()->basic_types.int__;
    p_stack->pop();

    int offset = p_stack->top()->basic_types.int__;
    p_stack->pop();

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // attempt to seek
    p_stack->push((bool)(std::fseek(p_snode->defn.io.stream, offset, origin) == 0));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_rewind(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // attempt to rewind
    std::rewind(p_snode->defn.io.stream);

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_clearerr(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // attempt to clear errors
    std::clearerr(p_snode->defn.io.stream);

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_eof(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // push bool EOF state
    p_stack->push((bool)(std::feof(p_snode->defn.io.stream) != 0));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_error_(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // push bool error state
    p_stack->push((bool)(std::ferror(p_snode->defn.io.stream) != 0));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_tmpfile(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_snode->defn.io.stream = std::tmpfile();

    // push bool state
    p_stack->push((bool)(p_snode->defn.io.stream != nullptr));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_tmpnam(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    char *buffer = (char *) std::malloc(L_tmpnam);
    std::string tmp_name = std::tmpnam(buffer);
    p_stack->push((void *) buffer);

    cx_type *p_str = new cx_type;
    p_str->form = fc_array;
    p_str->type_code = cx_address;
    p_str->size = tmp_name.size();
    set_type(p_str->array.p_element_type, (cx_type *) cx_function_id->p_type->base_type());
    p_str->array.element_count = tmp_name.length();
    p_str->array.max_index = tmp_name.length();

    return p_str;
}

STDIO_API
cx_type *cx_rename(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const char *new_filename = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    const char *old_filename = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((bool)(std::rename(old_filename, new_filename) == 0));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_remove(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const char *filename = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((bool)(std::remove(filename) == 0));

    return cx_function_id->p_type;
}

STDIO_API
cx_type *cx_perror(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const char *str = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    std::perror(str);

    return cx_function_id->p_type;
}

STDIO_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct) {

    cx_type *p_file_type = nullptr;
    cx_symtab *std_stream_members = nullptr;

    cx_symtab_node *p_file_id = p_symtab->enter("file", dc_type);

    if (!p_file_type) {
        set_type(p_file_type, new cx_type(fc_stream, sizeof (FILE), p_file_id));
        p_file_type->complex.p_class_scope = new cx_symtab;
        std_stream_members = p_file_type->complex.p_class_scope;
    }

    set_type(p_file_id->p_type, p_file_type);
    // allocate stream members

    cx_symtab_node *p_stdout = p_symtab->enter("stdout", ::dc_variable);
    set_type(p_stdout->p_type, p_file_type);
    p_stdout->defn.io.stream = stdout;

    cx_symtab_node *p_stdin = p_symtab->enter("stdin", ::dc_variable);
    set_type(p_stdin->p_type, p_file_type);
    p_stdin->defn.io.stream = stdin;

    cx_symtab_node *p_stderr = p_symtab->enter("stderr", ::dc_variable);
    set_type(p_stderr->p_type, p_file_type);
    p_stderr->defn.io.stream = stderr;

    struct member {
        cx_symtab_node *p_node;
        std::string name;
        std::string symbol_name;
        const cx_type *p_type;
        int num_params;
        ext_call ext_f;
    } members[] = {
        // stream members
        { nullptr, "puts", "cx_puts", ct[cx_bool], 1, cx_puts},
        { nullptr, "open", "cx_open", ct[cx_bool], 2, cx_open},
        { nullptr, "reopen", "cx_reopen", ct[cx_bool], 2, cx_reopen},
        { nullptr, "close", "cx_close", ct[cx_bool], 0, cx_close},
        { nullptr, "flush", "cx_flush", ct[cx_bool], 0, cx_flush},
        { nullptr, "wide", "cx_wide", ct[cx_int], 1, cx_wide},
        { nullptr, "read", "cx_read", new cx_type(fc_array, 0, nullptr), 2, cx_read},
        { nullptr, "write", "cx_write", ct[cx_int], 3, cx_write},
        { nullptr, "getc", "cx_getc", ct[cx_char], 0, cx_getc},
        { nullptr, "gets", "cx_gets", new cx_type(fc_array, 0, nullptr), 1, cx_gets},
        { nullptr, "putc", "cx_putc", ct[cx_int], 1, cx_putc},
        { nullptr, "ungetc", "cx_ungetc", ct[cx_int], 1, cx_ungetc},

        //wide char
        { nullptr, "getwc", "cx_getwc", ct[cx_wchar], 0, cx_getwc},
        { nullptr, "getws", "cx_getws", new cx_type(fc_array, 0, nullptr), 1, cx_getws},
        { nullptr, "putwc", "cx_putwc", ct[cx_wchar], 1, cx_putwc},
        { nullptr, "putws", "cx_putws", ct[cx_bool], 1, cx_putws},
        { nullptr, "ungetwc", "cx_ungetwc", ct[cx_wchar], 1, cx_ungetwc},

        // file positioning 
        { nullptr, "tell", "cx_tell", ct[cx_int], 0, cx_tell},
        { nullptr, "seek", "cx_seek", ct[cx_bool], 2, cx_seek},
        { nullptr, "rewind", "cx_rewind", ct[cx_void], 0, cx_rewind},

        // error handling
        { nullptr, "clearerr", "cx_clearerr", ct[cx_void], 0, cx_clearerr},
        { nullptr, "eof", "cx_eof", ct[cx_bool], 0, cx_eof},
        { nullptr, "error", "cx_error_", ct[cx_bool], 0, cx_error_},

        // operations on files 
        { nullptr, "tmpfile", "cx_tmpfile", ct[cx_bool], 0, cx_tmpfile}
    };

    for (auto &mbr : members) {
        mbr.p_node = std_stream_members->enter(mbr.name.c_str(), dc_function);

        mbr.p_node->defn.routine.which = func_std_member;
        mbr.p_node->defn.routine.ext_function = mbr.ext_f;
        mbr.p_node->defn.routine.parm_count = mbr.num_params;
        set_type(mbr.p_node->p_type, (cx_type *) mbr.p_type);

        if ((mbr.name == "puts") || ((mbr.name == "putws"))) {

            // char *str
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("str", dc_value_parm);
            mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
                    mbr.p_node->defn.routine.locals.p_parms_ids);

            if (mbr.name == "puts") {
                set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
            } else {
                set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_wchar]);
            }

        } else if ((mbr.name == "open") || (mbr.name == "reopen")) {

            // char *filename
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("filename", dc_value_parm);
            mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
                    mbr.p_node->defn.routine.locals.p_parms_ids);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);

            // char *mode
            mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("mode", dc_value_parm);
            mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type = new cx_type(fc_array, 0,
                    mbr.p_node->defn.routine.locals.p_parms_ids->next__);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
        } else if (mbr.name == "wide") {
            // int mode
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("mode", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
        } else if (mbr.name == "read") {
            // int size
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("size", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
            // int count
            mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("count", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_int]);

        } else if (mbr.name == "write") {

            // byte *buffer
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("buffer", dc_value_parm);
            mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
                    mbr.p_node->defn.routine.locals.p_parms_ids);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_uint8]);

            // int size
            mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("size", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_int]);

            // int count
            mbr.p_node->defn.routine.locals.p_parms_ids->next__->next__ = new cx_symtab_node("count", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->next__->p_type, (cx_type *) ct[cx_int]);
        } else if ((mbr.name == "gets") || (mbr.name == "getws")) {

            // int count
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("count", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
        } else if ((mbr.name == "putc") || (mbr.name == "putwc")) {

            // int ch or wchar_t
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("ch", dc_value_parm);

            if (mbr.name == "putc") {
                set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
            } else {
                set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_wchar]);
            }
        } else if ((mbr.name == "ungetc") || (mbr.name == "ungetwc")) {

            // int ch
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("ch", dc_value_parm);

            if (mbr.name == "ungetc") {
                set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
            } else {
                set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_wchar]);
            }
        } else if (mbr.name == "seek") {
            // int offset
            mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("offset", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
            // int origin
            mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("origin", dc_value_parm);
            set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_int]);
        }


        if (mbr.name == "read") {
            set_type(mbr.p_node->p_type->array.p_element_type, (cx_type *) ct[cx_uint8]);
        } else if (mbr.name == "gets") {
            set_type(mbr.p_node->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
        } else if (mbr.name == "getws") {
            set_type(mbr.p_node->p_type->array.p_element_type, (cx_type *) ct[cx_wchar]);
        }

    }

    cx_symtab_node *p_remove = p_symtab->enter("remove", dc_function);
    p_remove->defn.routine.which = func_standard;
    set_type(p_remove->p_type, (cx_type *) ct[cx_bool]);
    p_remove->defn.routine.parm_count = 1;
    p_remove->defn.routine.locals.p_parms_ids = new cx_symtab_node("filename", dc_value_parm);
    set_type(p_remove->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
    p_remove->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
    set_type(p_remove->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
    p_remove->defn.routine.ext_function = cx_remove;

    cx_symtab_node *p_perror = p_symtab->enter("perror", dc_function);
    p_perror->defn.routine.which = func_standard;
    set_type(p_perror->p_type, (cx_type *) ct[cx_void]);
    p_perror->defn.routine.parm_count = 1;
    p_perror->defn.routine.locals.p_parms_ids = new cx_symtab_node("str", dc_value_parm);
    set_type(p_perror->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
    p_perror->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
    set_type(p_perror->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
    p_perror->defn.routine.ext_function = cx_perror;

    cx_symtab_node *p_rename = p_symtab->enter("rename", dc_function);
    p_rename->defn.routine.which = func_standard;
    set_type(p_rename->p_type, (cx_type *) ct[cx_bool]);
    p_rename->defn.routine.parm_count = 2;
    p_rename->defn.routine.locals.p_parms_ids = new cx_symtab_node("old_filename", dc_value_parm);
    set_type(p_rename->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
    p_rename->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
    set_type(p_rename->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);

    p_rename->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("new_filename", dc_value_parm);
    set_type(p_rename->defn.routine.locals.p_parms_ids->next__->p_type, new cx_type(fc_array, 0, nullptr));
    p_rename->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_address;
    set_type(p_rename->defn.routine.locals.p_parms_ids->next__->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
    p_rename->defn.routine.ext_function = cx_rename;

    cx_symtab_node *p_tmpnam = p_symtab->enter("tmpnam", dc_function);
    p_tmpnam->defn.routine.which = func_standard;
    set_type(p_tmpnam->p_type, new cx_type(fc_array, 0, nullptr));
    set_type(p_tmpnam->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
    p_perror->defn.routine.parm_count = 0;
    p_tmpnam->defn.routine.ext_function = cx_tmpnam;

    // values

    cx_symtab_node *p_eof = p_symtab->enter("EOF", dc_constant);
    set_type(p_eof->p_type, (cx_type *) ct[cx_int]);
    p_eof->defn.constant.value.int__ = EOF;

    cx_symtab_node *p_fopen_max = p_symtab->enter("FOPEN_MAX", dc_constant);
    set_type(p_fopen_max->p_type, (cx_type *) ct[cx_int]);
    p_fopen_max->defn.constant.value.int__ = FOPEN_MAX;

    cx_symtab_node *p_filename_max = p_symtab->enter("FILENAME_MAX", dc_constant);
    set_type(p_filename_max->p_type, (cx_type *) ct[cx_int]);
    p_filename_max->defn.constant.value.int__ = FILENAME_MAX;

    cx_symtab_node *p_seek_set = p_symtab->enter("SEEK_SET", dc_constant);
    set_type(p_seek_set->p_type, (cx_type *) ct[cx_int]);
    p_seek_set->defn.constant.value.int__ = SEEK_SET;

    cx_symtab_node *p_seek_cur = p_symtab->enter("SEEK_CUR", dc_constant);
    set_type(p_seek_cur->p_type, (cx_type *) ct[cx_int]);
    p_seek_cur->defn.constant.value.int__ = SEEK_CUR;

    cx_symtab_node *p_seek_end = p_symtab->enter("SEEK_END", dc_constant);
    set_type(p_seek_end->p_type, (cx_type *) ct[cx_int]);
    p_seek_end->defn.constant.value.int__ = SEEK_END;

    cx_symtab_node *p_tmp_max = p_symtab->enter("TMP_MAX", dc_constant);
    set_type(p_tmp_max->p_type, (cx_type *) ct[cx_int]);
    p_tmp_max->defn.constant.value.int__ = TMP_MAX;

    cx_symtab_node *p_L_tmpnam = p_symtab->enter("L_tmpnam", dc_constant);
    set_type(p_L_tmpnam->p_type, (cx_type *) ct[cx_int]);
    p_L_tmpnam->defn.constant.value.int__ = L_tmpnam;
}