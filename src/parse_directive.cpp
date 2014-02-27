/** parse Directives
 * parse_directive.cpp
 *
 */

#include <cstdio>
#include <cstdlib>
#include <string>
#include "common.h"
#include "buffer.h"
#include "error.h"
#include "parser.h"
#include "cx_api.h"

extern bool cx_dev_debug_flag;

#if defined _WIN32
#include <windows.h>
std::vector<HINSTANCE> windows_libs;
#elif defined __linux__
#include <dlfcn.h>
std::vector<void *> linux_libs;
#elif defined __APPLE__

#endif

void load_lib(const char *lib, cx_symtab *p_symtab) {

    const cx_type * cx_types_[] = {
        p_integer_type,
        p_char_type,
        p_wchar_type,
        p_float_type,
        p_boolean_type,
        p_uint8_type,
        p_void_type
    };

    typedef void (*lib_init)(cx_symtab *, const cx_type **);
    lib_init init_call = nullptr;
    bool f_runtime_linksuccess = false;

#ifdef _WIN32
    HINSTANCE hinst_lib = nullptr;
#elif defined __linux__
    void *lib_handle = nullptr;
#endif

    std::string lib_path;
    char *env_path = getenv(__CX_STDLIB__);

    if (env_path != nullptr) {
        lib_path = env_path;
#ifdef _WIN32
        lib_path += "\\";
#elif defined __linux__
        lib_path += "/";
#endif
    }

#if defined _WIN32
    std::string dll = std::string(lib) + ".dll";
    lib_path += dll;

    // Get a handle to the DLL module.
    hinst_lib = LoadLibrary(lib_path.c_str());
    // If the handle is valid, try to get the function address.
    if (hinst_lib != nullptr) {
        init_call = (lib_init) GetProcAddress(hinst_lib, "cx_lib_init");

        windows_libs.push_back(hinst_lib);

        // If the function address is valid, call the function.
        if (nullptr != init_call) {
            f_runtime_linksuccess = true;
            //pass our symbol table to the DLL with a map of allowed data types.
            (init_call) (p_symtab, cx_types_);
        } else {
            cx_error(err_library_no_init);
        }

    } else {
        cx_error(err_loading_library);
    }

#elif defined __linux__
    std::string so = std::string(lib) + ".so";
    lib_path += so;

    lib_handle = dlopen(lib_path.c_str(), RTLD_LAZY);
    // If the handle is valid, try to get the function address.
    if (lib_handle != nullptr) {
        init_call = (lib_init) dlsym(lib_handle, "cx_lib_init");

        linux_libs.push_back(lib_handle);

        // If the function address is valid, call the function.
        if (nullptr != init_call) {
            f_runtime_linksuccess = true;
            //pass our symbol table to the DLL with a map of allowed data types.
            (init_call) (p_symtab, cx_types_);
        } else {
            cx_error(err_library_no_init);
        }

    } else {
        cx_error(err_loading_library);
    }

#elif defined __APPLE__
#endif

}

/** parse_execute_directive      Opens an external script module
 *                      for parsing.
 *
 *      #include <identifier>
 *
 * @param p_function_id : ptr to the routine owning this directive call.
 */
void cx_parser::parse_execute_directive(cx_symtab_node *p_function_id) {

    extern cx_symtab_node *p_program_ptr_id;

    switch (token) {
        case tc_INCLUDE:
        {
            std::string lib_path;
            char *env_path = getenv(__CX_STDLIB__);

            if (env_path != nullptr) {
                lib_path = env_path;
#ifdef _WIN32
                lib_path += "\\";
#else
                lib_path += "/";
#endif
            }

            get_token();

            lib_path += p_token->string__();
            p_program_ptr_id->found_global_end = true;

            // save current line number
            int old_line_num = current_line_number;
            current_line_number = 0;

            /* true : stdlib module
             * returns nullptr */
            cx_parser *parser = new cx_parser
                    (new cx_source_buffer(lib_path.c_str()), true);

            parser->parse();

            current_line_number = old_line_num;

            delete parser;

            icode.reset();
            icode.put(tc_left_bracket);
            p_program_ptr_id->found_global_end = false;
            get_token_append();
        }
            break;
        case tc_WARN:
        {
            get_token();

            if (token != tc_string) {
                cx_error(err_unexpected_token);
            }

            {
                std::string msg = p_token->string__();
                msg[0] = ' ';
                msg[msg.length() - 1] = '\0';
                std::cerr << "warning:" << msg << std::endl;
            }

            get_token_append();
        }
            break;
        case tc_IMPORT:
        {
            get_token();

            load_lib(p_token->string__(), symtab_stack.get_current_symtab());

            get_token_append();
        }
            break;
        default:
            break;
    }
}
