/** main
 * main.cpp
 *
 * main entry point for the Cx interpretor.
 */

#include <iostream>
#include <memory>
#include <locale>
#include <codecvt>

#ifdef __CX_PROFILE_EXECUTION__
#include <chrono>
/*
#ifdef _WIN32_LEAK_DETECT
#ifdef _WIN32
#define _CRTDBG_MAP_ALLOC // below for leak detection on windows
#include <stdlib.h>
#include <crtdbg.h>
#endif*/
//#endif

#endif

#include "error.h"
#include "buffer.h"
#include "parser.h"
#include "symtab.h"

namespace cx{
	extern bool	cx_dev_debug_flag;
}

void set_options(int argc, char **argv);

/** main        main entry point
 *
 * @param argc
 * @param argv
 * @return exit status.
 */
int main(int argc, char *argv[]) {
	using namespace cx;

    set_options(argc, argv);

    // Check the command line arguments.
    if (argc < 2) {
        std::cerr << "usage: " << argv[0] << " <source file>" << std::endl;
        abort_translation(ABORT_INVALID_COMMANDLINE_ARGS);
    }

    list_flag = cx_dev_debug_flag;
    error_arrow_flag = cx_dev_debug_flag;

	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::string args = argv[1];

    // Create the parser for the source file,
    // and then parse the file.
	std::shared_ptr<cx::parser> parser = std::make_shared<cx::parser>(new source_buffer(converter.from_bytes(args)));

#ifdef __CX_PROFILE_EXECUTION__
    using namespace std::chrono;
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
    symbol_table_node_ptr p_program_id = parser->parse();

	std::wofstream output("debug.cxvm");
	output << "function:\t" << p_program_id->node_name << "\naddress:\t" << p_program_id << std::endl;
	for (auto &code : p_program_id->defined.routine.program_code){
		output << opcode_string[code.op] << "\t\t" << code.arg0.a_ << " " << code.arg1.a_ << std::endl;
	}

	for (auto node : p_global_symbol_table->symbols){
		if ((node.second->defined.defined_how == DC_FUNCTION) || (node.second->defined.defined_how == DC_PROGRAM)){
			output << "\nfunction:\t" << node.second->node_name << "\naddress:\t" << node.second << std::endl;
			for (auto &code : node.second->defined.routine.program_code){
				output << opcode_string[code.op] << "\t\t" << code.arg0.a_ << " " << code.arg1.a_ << std::endl;
			}
		}
	}

	output.close();

#ifdef __CX_PROFILE_EXECUTION__
    high_resolution_clock::time_point t2 = high_resolution_clock::now();
    duration<double> time_span = duration_cast < duration<double >> (t2 - t1);
    std::cout << "finished parsing in: " << time_span.count() << "(secs)" << std::endl;
#endif

    if (error_count == 0) {
		std::unique_ptr<cxvm> cx = std::make_unique<cxvm>();

#ifdef __CX_PROFILE_EXECUTION__
        t1 = high_resolution_clock::now();
#endif
		p_program_id->runstack_item = cx->push();
		cx->enter_function(p_program_id.get());
		cx->go();

		std::wcout << p_program_id->node_name << " returned " << p_program_id->runstack_item->i_ << std::endl;

#ifdef __CX_PROFILE_EXECUTION__
        t2 = high_resolution_clock::now();
        time_span = duration_cast < duration<double >> (t2 - t1);
        std::cout << "finished executing in: " << time_span.count() << "(secs)" << std::endl;

        std::cin.get();
#endif

    }

	return static_cast<int>(p_program_id->runstack_item->i_);
}

void set_options(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp("-ddev", argv[i])) cx::cx_dev_debug_flag = true;
    }
}