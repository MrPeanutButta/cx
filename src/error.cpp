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
void abort_translation(cx_abort_code ac) {
    std::cerr << "*** fatal translator error: " << abort_message[-ac] << std::endl;
    exit(ac);
}

/* Syntax error messages       Keyed to enumeration type
 *                             cx_error_code.
 */
const char *error_messages[] = {
    "No error",
    "Unrecognizable input",
    "Too many syntax errors",
    "Unexpected end of file",
    "Invalid number",
    "Invalid fraction",
    "Invalid exponent",
    "Too many digits",
    "Real literal out of range",
    "Integer literal out of range",
    "Missing )",
    "Invalid expression",
    "Invalid assignment statement",
    "Missing identifier",
    "Missing =",
    "Undefined identifier",
    "Stack overflow",
    "Invalid statement",
    "Unexpected token",
    "Missing ;",
    "Missing ,",
    "Missing DO",
    "Missing while",
    "Missing THEN",
    "Invalid FOR control variable",
    "Missing OF",
    "Invalid constant",
    "Missing constant",
    "Missing :",
    "Missing [",
    "Missing ]",
    "Redefined identifier",
    "Missing ==",
    "Invalid type",
    "Not a type identifier",
    "Invalid subrange type",
    "Not a constant identifier",
    "Missing ..",
    "Incompatible types",
    "Invalid assignment target",
    "Invalid identifier usage",
    "Incompatible assignment",
    "Min limit greater than max limit",
    "Missing {",
    "Missing }",
    "Invalid index type",
    "Missing BEGIN",
    "Missing return",
    "Too many subscripts",
    "Invalid field",
    "Nesting too deep",
    "Missing PROGRAM",
    "Already specified in FORWARD",
    "Wrong number of actual parameters",
    "Invalid reference variable",
    "Not a record variable",
    "Missing variable",
    "code segment overflow",
    "Unimplemented feature",
    "Missing (",
    "Missing '",
    "Invalid escape character"
};

/** cx_error       print an arrow under the error and then
 *              print the error message.
 *
 * @param ec : error code
 */
void cx_error(cx_error_code ec) {
    const int max_syntax_errors = 0;

    int error_position = error_arrow_offset + input_position - 1;

    // print the arrow pointing to the token just scanned.
    if (error_arrow_flag) {
        sprintf(list.text, "%*s^", error_position, " ");
        list.put_line();
    }

    // print the error message.
    sprintf(list.text, "*** error: %s", error_messages[ec]);
    list.put_line();

    if (++error_count > max_syntax_errors) {
        list.put_line("Too many syntax errors.  Translation aborted.");
        abort_translation(abort_too_many_syntax_errors);
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

void cx_runtime_error(cx_runtime_error_code ec) {
    extern int current_line_number;

    std::cout << "\nruntime error in line <" << current_line_number << ">: "
            << runtime_error_messages[ec] << std::endl;

    exit(abort_runtime_error);
}