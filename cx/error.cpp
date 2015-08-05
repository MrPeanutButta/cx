/** Errors
 * error.cpp
 *
 * Routines to handle translation-time and runtime errors.
 */

#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include "buffer.h"
#include "error.h"

namespace cx{
	int error_count = 0; // count of syntax errors
	bool error_arrow_flag = true; // true if print arrows under syntax

	//   errors, false if not
	int error_arrow_offset = 8; // offset for printing the error arrow

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
		L"Missing \'",
		L"Missing \"",
		L"Invalid escape character",
		L"Could not load library",
		L"No cx_lib_init in library"
	};

	/** cx_error       print an arrow under the error and then
	 *              print the error message.
	 *
	 * @param ec : error code
	 */
	void cx_error(error_code ec) {
		const int max_syntax_errors = 0;

		int error_position = error_arrow_offset + input_position - 1;

		// print the arrow pointing to the token just scanned.
		list.put_line(); // print current line info
		swprintf(list.text, L"%*s^", error_position, L" ");
		list.put_line();

		// print the error message.
		swprintf(list.text, L"*** error: %s", error_messages[ec]);
		list.put_line();

		if (++error_count > max_syntax_errors) {
			list.put_line(L"Too many syntax errors.  Translation aborted.");
			std::cin.get();
			exit(ec);
			//abort_translation(abort_too_many_syntax_errors);
		}

	}

	const wchar_t *runtime_error_messages[] = {
		L"No runtime error",
		L"Runtime stack overflow",
		L"value out of range",
		L"Invalid CASE expression value",
		L"Division by zero",
		L"Invalid standard function argument",
		L"Invalid user input",
		L"Unimplemented runtime feature"
	};

	void cx_runtime_error(runtime_error_code ec) {
		extern int current_line_number;

		std::cout << "\nruntime error in line <" << current_line_number << ">: "
			<< runtime_error_messages[ec] << std::endl;

		exit(ABORT_RUNTIME_ERROR);
	}
}