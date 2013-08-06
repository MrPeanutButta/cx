/** main
 * main.cpp
 * 
 * main entry point for the Cx interpretor.
 */
#include <iostream>

#ifdef __CX_PROFILE_EXECUTION__
#include <chrono>
#endif

#include "error.h"
#include "buffer.h"
#include "parser.h"
#include "symtable.h"
#include "common.h"
#include "icode.h"

// turn on to view Cx debugging
#ifdef __CX_DEBUG__
bool cx_dev_debug_flag = true;
#else
bool cx_dev_debug_flag = false;
#endif 

/** main        main entry point
 * 
 * @param argc
 * @param argv
 * @return exit status.
 */
int main(int argc, char *argv[]) {

    // Check the command line arguments.
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " <source file>" << endl;
        abort_translation(abort_invalid_commandline_args);
    }

    list_flag = cx_dev_debug_flag;
    error_arrow_flag = cx_dev_debug_flag;

    // Create the parser for the source file,
    // and then parse the file.
    cx_parser *parser = new cx_parser(new cx_source_buffer(argv[1]));


#ifdef __CX_PROFILE_EXECUTION__
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif

    cx_symtab_node *p_program_id = parser->parse();

#ifdef __CX_PROFILE_EXECUTION__
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast < duration<double >> (t2 - t1);
    std::cout << "finished parsing in: " << time_span.count() << "(secs)" << std::endl;
#endif

    delete parser;

    if (error_count == 0) {
        p_vector_symtabs = new cx_symtab *[symtab_count];
        for (cx_symtab *pSt = p_symtab_list; pSt; pSt = pSt->next()) {
            if (pSt != nullptr) {
                if (pSt->root() != nullptr)pSt->convert(p_vector_symtabs);
            }
        }

        cx_backend *pBackend = new cx_executor;

#ifdef __CX_PROFILE_EXECUTION__
        std::cin.get();
        t1 = high_resolution_clock::now();
#endif

        pBackend->go(p_program_id);

#ifdef __CX_PROFILE_EXECUTION__
        t2 = high_resolution_clock::now();
        time_span = duration_cast < duration<double >> (t2 - t1);
        std::cout << "finished executing in: " << time_span.count() << "(secs)" << std::endl;
#endif

        delete[] p_vector_symtabs;
        delete pBackend;
    }

    return 0;
}