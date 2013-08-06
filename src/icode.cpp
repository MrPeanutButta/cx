#include <cstring>
#include "common.h"
#include "error.h"
#include "symtable.h"
#include "icode.h"

const char *cx_symbol_strings[] = {
    nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
    //operators and punctuation
    "^", "&", "|", "~", "^=", "&=", "|=", "<<",
    "<<=", ">>", ">>=", "-", "-=", "+", "+=", "=", "--",
    "++", "/", "/=", "*", "*=", "<", ">", "==", "<=", ">=",
    "!=", "%", "%=", "[", "]", "?", "#", ".*", "(", ")", "{",
    "}", ":", ";", ",", "...", ".", "::", "->", "->*", "||",
    "&&", "!", "\'", "\"",

    "if", "return", "continue", "friend", "go_to", "try",
    "delete", "typeid", "do", "singed",
    "typename", "break", "sizeof",
    "case", "static", "unsigned", "catch", "else", "namespace",
    "using", "new", "virtual", "explicit",
    "noexcept", "export", "nullptr", "switch",
    "extern", "operator", "template", "const",
    "private", "this", "while", "protected", "threadlocal",
    "for", "public", "throw", "default", "typedef", "mutable", "include"
};

/** Copy constructor    Make a copy of the icode.  Only copy as
 *                      many bytes of icode as necessary.
 * 
 * @param icode : icode to copy.
 */
cx_icode::cx_icode(const cx_icode &icode) {
    int length = int(icode.cursor - icode.p_code); // length of icode

    // Copy icode.
    p_code = cursor = new char[length];
    memcpy(p_code, icode.p_code, length);
}

/** check_bounds         Guard against code segment overflow.
 * 
 * @param size : number of bytes to append.
 */
void cx_icode::check_bounds(int size) {
    if (cursor + size >= &p_code[code_segment_size]) {
        cx_error(err_code_segment_overflow);
        abort_translation(abort_code_segment_overflow);
    }
}

/** put(cx_token_code)     Append a token code to the intermediate
 *                      code.
 * 
 * @param tc : token code.
 */
void cx_icode::put(cx_token_code tc) {
    if (error_count > 0) return;

    char code = tc;
    check_bounds(sizeof (char));
    memcpy((void *) cursor, (const void *) &code, sizeof (char));
    cursor += sizeof (char);
}

/** put(cx_symtab_node *)      Append a symbol table node's symbol
 *                          table and node indexes to the
 *                          intermediate code.
 * 
 * @param p_node : ptr to symtab node
 */
void cx_icode::put(const cx_symtab_node *p_node) {
    if (error_count > 0) return;

    short xsymtab = p_node->symtab_index();
    short xnode = p_node->node_index();

    check_bounds(2 * sizeof (short));
    memcpy((void *) cursor,
            (const void *) &xsymtab, sizeof (short));
    memcpy((void *) (cursor + sizeof (short)),
            (const void *) &xnode, sizeof (short));
    cursor += 2 * sizeof (short);
}

/** get         Extract the next__ token from the
 *              intermediate code.
 * 
 * @return ptr to the extracted token.
 */
cx_token *cx_icode::get(void) {
    cx_token *p_token; // ptr to token to return
    char code; // token code read from the file
    cx_token_code token;

    // Loop to process any line markers
    // and extract the next__ token code.
    do {
        // First read the token code.
        memcpy((void *) &code, (const void *) cursor, sizeof (char));
        cursor += sizeof (char);
        token = (code > 0) ? (cx_token_code) code : tc_dummy;

        // If it's a line marker, extract the line number.
        if (token == mc_line_marker) {
            short number;

            memcpy((void *) &number, (const void *) cursor,
                    sizeof (short));
            current_line_number = number;
            cursor += sizeof (short);
        }
    } while (token == mc_line_marker);

    // Determine the token class, based on the token code.
    switch (token) {
        case tc_number: p_token = &number_token;
            break;
        case tc_string: p_token = &string_token;
            break;

        case tc_identifier:
            p_token = &word_token;
            p_token->code__ = tc_identifier;
            break;

        case mc_location_marker:
            p_token = &special_token;
            p_token->code__ = token;
            break;
        default:
            if (token < tc_IF) {
                p_token = &special_token;
                p_token->code__ = token;
            } else {
                p_token = &word_token; // reserved word
                p_token->code__ = token;
            }
            break;
    }

    // Extract the symbol table node and set the token string.
    switch (token) {
        case tc_identifier:
        case tc_number:
        case tc_char:
        case tc_string:
            p_node = get_symtab_node();
            strcpy(p_token->string, p_node->string__());
            break;

        case mc_location_marker:
            p_node = nullptr;
            p_token->string[0] = '\0';
            break;
        case tc_end_of_file:
        case tc_dummy:
            break;
        default:

            p_node = nullptr;
            strcpy(p_token->string, cx_symbol_strings[code]);
            break;
    }

    return p_token;
}

/** get_symtab_node       Extract a symbol table node pointer
 *                      from the intermediate code.
 * 
 * @return ptr to the symbol table node
 */
cx_symtab_node *cx_icode::get_symtab_node(void) {
    extern cx_symtab **p_vector_symtabs;
    short xsymtab, xnode; // symbol table and node indexes

    memcpy((void *) &xsymtab, cursor,
            sizeof (short));
    memcpy((void *) &xnode, (const void *) (cursor + sizeof (short)),
            sizeof (short));
    cursor += 2 * sizeof (short);

    return p_vector_symtabs[xsymtab]->get(xnode);
}

/** insert_line_marker    Insert a line marker into the
 *                      intermediate code just before the
 *                      last appended token code. 
 */
void cx_icode::insert_line_marker(void) {
    if (error_count > 0) return;

    // Remember the last appended token code;
    char last_code;
    cursor -= sizeof (char);
    memcpy((void *) &last_code, (const void *) cursor, sizeof (char));

    // Insert a statement marker code
    // followed by the current line number.
    char code = mc_line_marker;
    short number = current_line_number;
    check_bounds(sizeof (char) + sizeof (short));
    memcpy((void *) cursor, (const void *) &code, sizeof (char));
    cursor += sizeof (char);
    memcpy((void *) cursor, (const void *) &number, sizeof (short));
    cursor += sizeof (short);

    // Re-append the last token code;
    memcpy((void *) cursor, (const void *) &last_code, sizeof (char));
    cursor += sizeof (char);
}

/** put_location_marker   Append a location marker to the
 *                      intermediate code.
 * 
 * @return location of the location marker's offset.
 */
int cx_icode::put_location_marker(void) {
    if (error_count > 0) return 0;

    // Append the location marker code.
    char code = mc_location_marker;
    check_bounds(sizeof (char));
    memcpy((void *) cursor, (const void *) &code, sizeof (char));
    cursor += sizeof (char);

    // Append 0 as a placeholder for the location offset.
    // Remember the current location of the offset itself.
    short offset = 0;
    int at_location = current_location();
    check_bounds(sizeof (short));
    memcpy((void *) cursor, (const void *) &offset, sizeof (short));
    cursor += sizeof (short);

    return at_location;
}

/** fixup_location_marker     Fixup a location marker in the
 *                          intermediate code by patching in the
 *                          offset of the current token's
 *                          location.
 * 
 * @param location : location of the offset to fix up.
 */
void cx_icode::fixup_location_marker(int location) {
    // Patch in the offset of the current token's location.
    short offset = current_location() - 1;
    memcpy((void *) (p_code + location), (const void *) &offset,
            sizeof (short));
}

/** get_location_marker       Extract a location marker from the
 *                          intermediate code.
 * 
 * @return location offset.
 */
int cx_icode::get_location_marker(void) {
    short offset; // location offset

    // Extract the offset from the location marker.
    memcpy((void *) &offset, (const void *) cursor, sizeof (short));
    cursor += sizeof (short);

    return int(offset);
}

/** put_case_item         Append a CASE item to the intermediate
 *                      code.
 * 
 * @param value   : CASE label value
 * @param location: location of CASE branch statement
 */
void cx_icode::put_case_item(int value, int location) {
    if (error_count > 0) return;

    short offset = location & 0xffff;

    check_bounds(sizeof (int) + sizeof (short));
    memcpy((void *) cursor, (const void *) &value, sizeof (int));
    cursor += sizeof (int);
    memcpy((void *) cursor, (const void *) &offset, sizeof (short));
    cursor += sizeof (short);
}

/** get_case_item         Extract a CASE item from the
 *                      intermediate code.
 * 
 * @param value   : ref to CASE label value
 * @param location: ref to location of CASE branch statement
 */
void cx_icode::get_case_item(int &value, int &location) {
    int val;
    short offset;

    memcpy((void *) &val, (const void *) cursor, sizeof (int));
    cursor += sizeof (int);
    memcpy((void *) &offset, (const void *) cursor, sizeof (short));
    cursor += sizeof (short);

    value = val;
    location = offset;
}

