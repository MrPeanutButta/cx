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

int errorCount = 0; // count of syntax errors
bool errorArrowFlag = true; // true if print arrows under syntax

//   errors, false if not
int errorArrowOffset = 8; // offset for printing the error arrow

///  Abort messages      Keyed to enumeration type TAbortCode.
const char *abortMsg[] = {
    NULL,
    "Invalid command line arguments",
    "Failed to open source file",
    "Failed to open intermediate form file",
    "Failed to open assembly file",
    "Too many syntax errors",
    "Stack overflow",
    "Code segment overflow",
    "Nesting too deep",
    "Runtime error",
    "Unimplemented feature",
};

/**AbortTranslation    A fatal error occurred during the
 *                     translation.  Print the abort code
 *                     to the error file and then exit.
 *
 *     ac : abort code
 */
void AbortTranslation(TAbortCode ac) {
    cerr << "*** fatal translator error: " << abortMsg[-ac] << endl;
    exit(ac);
}



/*  Syntax error messages       Keyed to enumeration type
 *                              TErrorCode.
 */
const char *errorMessages[] = {
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
    "Code segment overflow",
    "Unimplemented feature",
    "Missing (",
    "Missing '"
};

/**Error       Print an arrow under the error and then
 *             print the error message.
 *
 *     ec : error code
 */
void Error(TErrorCode ec) {
    const int maxSyntaxErrors = 0;

    int errorPosition = errorArrowOffset + inputPosition - 1;

    //--Print the arrow pointing to the token just scanned.
    if (errorArrowFlag) {
        sprintf(list.text, "%*s^", errorPosition, " ");
        list.PutLine();
    }

    //--Print the error message.
    sprintf(list.text, "*** error: %s", errorMessages[ec]);
    list.PutLine();

    if (++errorCount > maxSyntaxErrors) {
        list.PutLine("Too many syntax errors.  Translation aborted.");
        AbortTranslation(abortTooManySyntaxErrors);
    }
}

const char *runtimeErrorMessages[] = {
    "No runtime error",
    "Runtime stack overflow",
    "Value out of range",
    "Invalid CASE expression value",
    "Division by zero",
    "Invalid standard function argument",
    "Invalid user input",
    "Unimplemented runtime feature"
};

void RuntimeError(TRuntimeErrorCode ec) {
    extern int currentLineNumber;

    cout << "\nruntime error in line <" << currentLineNumber << ">: "
            << runtimeErrorMessages[ec] << endl;

    exit(abortRuntimeError);
}