/** main
 * main.cpp
 *
 * main entry point for the Cx interpretor.
 */

#include <iostream>
#include <memory>

#ifdef __CX_PROFILE_EXECUTION__
#include <chrono>

#ifdef _WIN32_LEAK_DETECT
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC // below for leak detection on windows
#include <stdlib.h>
#include <crtdbg.h>
#endif
#endif

#endif

#include "error.h"
#include "buffer.h"
#include "parser.h"
#include "symtab.h"
#include "common.h"
#include "icode.h"

// turn on to view Cx debugging
#ifdef __CX_DEBUG__
bool cx_dev_debug_flag = true;
#else
bool cx_dev_debug_flag = false;
#endif

void set_options(int argc, char **argv);

/** main        main entry point
 *
 * @param argc
 * @param argv
 * @return exit status.
 */
int main(int argc, char *argv[]) {

    set_options(argc, argv);

    // Check the command line arguments.
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <source file>" << std::endl;
        abort_translation(abort_invalid_commandline_args);
    }

    list_flag = cx_dev_debug_flag;
    error_arrow_flag = cx_dev_debug_flag;

    // Create the parser for the source file,
    // and then parse the file.
    std::unique_ptr<cx_parser> parser(new cx_parser(new cx_source_buffer(argv[1])));

#ifdef __CX_PROFILE_EXECUTION__
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
    std::unique_ptr<cx_symtab_node> program_id(parser->parse());

#ifdef __CX_PROFILE_EXECUTION__
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast < duration<double >> (t2 - t1);
    std::cout << "finished parsing in: " << time_span.count() << "(secs)" << std::endl;
#endif

    if (error_count == 0) {

        std::unique_ptr<cx_backend> backend(new cx_executor);

#ifdef __CX_PROFILE_EXECUTION__
        std::cin.get();
        t1 = high_resolution_clock::now();
#endif

        backend->go(program_id.get());

#ifdef __CX_PROFILE_EXECUTION__
        t2 = high_resolution_clock::now();
        time_span = duration_cast < duration<double >> (t2 - t1);
        std::cout << "finished executing in: " << time_span.count() << "(secs)" << std::endl;

#ifdef _WIN32_LEAK_DETECT
#ifdef _WIN32
        _CrtDumpMemoryLeaks();
#endif
#endif
        std::cin.get();
#endif

    }

    return 0;
}

void set_options(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp("-ddev", argv[i])) cx_dev_debug_flag = true;
    }
}