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

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "buffer.h"
#include "error.h"

namespace cx{
	namespace error {
		bool error_arrow_flag = true; // true if print arrows under syntax
		//   errors, false if not
		const int error_arrow_offset = 8; // offset for printing the error arrow
		int error_count = 0; // count of syntax errors
		int max_syntax_errors = 0;
	}

	///  Abort messages      Keyed to enumeration type cx_abort_code.
	const char *abort_message[] = {
		nullptr,
		"Invalid command line arguments",
		"Failed to open source file",
		"Failed to open intermediate form file",
		"Failed to open assembly file",
		"Too many syntax errors",
		"Stack overflow",
		"code segment overflow",
		"Nesting too deep",
		"Runtime error",
		"Unimplemented feature",
	};

	/** abort_translation    A fatal error occurred during the
	 *                     translation.  print the abort code
	 *                     to the error file and then exit.
	 *
	 * @param ac : abort code
	 */
	void abort_translation(abort_code ac) {
		std::cerr << "*** fatal translator error: " << abort_message[-ac] << std::endl;
		std::cin.get();
		exit(ac);
	}

	/* Syntax error messages       Keyed to enumeration type
	 *                             cx_error_code.
	 */
	const wchar_t *error_messages[] = {
		L"No error",
		L"Unrecognizable input",
		L"Too many syntax errors",
		L"Unexpected end of file",
		L"Invalid number",
		L"Invalid fraction",
		L"Invalid exponent",
		L"Too many digits",
		L"Real literal out of range",
		L"Integer literal out of range",
		L"Missing )",
		L"Invalid expression",
		L"Invalid assignment statement",
		L"Missing identifier",
		L"Missing =",
		L"Undefined identifier",
		L"Stack overflow",
		L"Invalid statement",
		L"Unexpected token",
		L"Missing ;",
		L"Missing ,",
		L"Missing DO",
		L"Missing while",
		L"Missing THEN",
		L"Invalid FOR control variable",
		L"Missing OF",
		L"Invalid constant",
		L"Missing constant",
		L"Missing :",
		L"Missing [",
		L"Missing ]",
		L"Redefined identifier",
		L"Missing ==",
		L"Invalid type",
		L"Not a type identifier",
		L"Invalid subrange type",
		L"Not a constant identifier",
		L"Missing ..",
		L"Incompatible types",
		L"Invalid assignment target",
		L"Invalid identifier usage",
		L"Incompatible assignment",
		L"Min limit greater than max limit",
		L"Missing {",
		L"Missing }",
		L"Invalid index type",
		L"Missing BEGIN",
		L"Missing return",
		L"Too many subscripts",
		L"Invalid field",
		L"Nesting too deep",
		L"Missing PROGRAM",
		L"Already specified in FORWARD",
		L"Wrong number of actual parameters",
		L"Invalid reference variable",
		L"Not a record variable",
		L"Missing variable",
		L"code segment overflow",
		L"Unimplemented feature",
		L"Missing (",
		L"Missing '",
		L"Invalid escape character",
		L"Could not load library",
		L"No cx_lib_init in library",
		L"Invalid VM opcode"
	};

	/** cx_error       print an arrow under the error and then
	 *              print the error message.
	 *
	 * @param ec : error code
	 */
	void cx_error(error_code ec) {
		int error_position = error::error_arrow_offset + buffer::input_position - 1;

		// print the arrow pointing to the token just scanned.
		buffer::list.put_line(); // print current line info
		_swprintf(buffer::list.text, L"%*s^", error_position, L" ");
		buffer::list.put_line();

		// print the error message.
		_swprintf(buffer::list.text, L"*** error: %s", error_messages[ec]);
		buffer::list.put_line();

		if (++error::error_count > error::max_syntax_errors) {
			buffer::list.put_line(L"Too many syntax errors.  Translation aborted.");
			std::cin.get();
			exit(ec);
			//abort_translation(abort_too_many_syntax_errors);
		}

	}

	const char *runtime_error_messages[] = {
		"No runtime error",
		"Runtime stack overflow",
		"value out of range",
		"Invalid CASE expression value",
		"Division by zero",
		"Invalid standard function argument",
		"Invalid user input",
		"Unimplemented runtime feature"
	};

	void cx_runtime_error(runtime_error_code ec) {
		std::cout << "\nruntime error in line <" << buffer::current_line_number << ">: "
			<< runtime_error_messages[ec] << std::endl;

		exit(ABORT_RUNTIME_ERROR);
	}
}