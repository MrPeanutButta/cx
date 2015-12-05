/*
The MIT License (MIT)

Copyright (c) 2015 Aaron Hebert <aaron.hebert@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#ifdef __CX_PROFILE_EXECUTION__
#include <chrono>
#endif

#include <iostream>
#include <memory>
#include <locale>
#include <codecvt>
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
	int return_value = 0;

	try {
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

		if (cx::cx_dev_debug_flag) {
			std::wstring asm_file = parser->code_filename() + L".i";
			std::wofstream output(asm_file);
			output << "function: " << p_program_id->node_name << " address: " << p_program_id << std::endl;
			int label_number = 0;
			for (auto &code : p_program_id->defined.routine.program_code) {
				output << std::setw(5) << label_number++ << ":" << std::setw(10) << opcode_string[code.op] << "\t\t" << code.arg0.a_ << " " << code.arg1.a_ << std::endl;
			}

			for (auto node : p_global_symbol_table->symbols) {
				label_number = 0;
				if ((node.second->defined.defined_how == DC_FUNCTION) || (node.second->defined.defined_how == DC_PROGRAM)) {
					output << "\nfunction: " << node.second->node_name << " address: " << node.second << std::endl;
					for (auto &code : node.second->defined.routine.program_code) {
						output << std::setw(5) << label_number++ << ":" << std::setw(10) << opcode_string[code.op] << "\t\t" << code.arg0.a_ << " " << code.arg1.a_ << std::endl;
					}
				}
			}

			output.close();
		}

#ifdef __CX_PROFILE_EXECUTION__
		high_resolution_clock::time_point t2 = high_resolution_clock::now();
		duration<double> time_span = duration_cast <duration<double >> (t2 - t1);
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
			time_span = duration_cast <duration<double >> (t2 - t1);
			std::cout << "finished executing in: " << time_span.count() << "(secs)" << std::endl;

			std::cin.get();
#endif
			return_value = static_cast<int>(p_program_id->runstack_item->i_);
		}
	}

	catch (std::exception ex) {
		std::cerr << ex.what();
		std::cin.get();
	}

	return return_value;
}

void set_options(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (!strcmp("-ddev", argv[i])) cx::cx_dev_debug_flag = true;
    }
}