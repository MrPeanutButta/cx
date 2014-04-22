#include <utility>
#include <cstring>
#include <cstdio>
#include "misc.h"
#include "token.h"

/*************************
 *                       *
 *  Reserved Word Table  *
 *                       *
 *************************/

///  Reserved word lists
std::pair<std::string, cx_token_code> map_data[] = {
    std::make_pair("if", tc_IF),
    std::make_pair("return", tc_RETURN),
    std::make_pair("continue", tc_CONTINUE),
    std::make_pair("friend", tc_FRIEND),
    std::make_pair("goto", tc_GOTO),
    std::make_pair("try", tc_TRY),
    std::make_pair("delete", tc_DELETE),
    std::make_pair("typeid", tc_TYPEID),
    std::make_pair("do", tc_DO),
    std::make_pair("signed", tc_SIGNED),
    std::make_pair("typename", tc_TYPENAME),
    std::make_pair("break", tc_BREAK),
    std::make_pair("sizeof", tc_SIZEOF),
    std::make_pair("case", tc_CASE),
    std::make_pair("static", tc_STATIC),
    std::make_pair("unsigned", tc_UNSIGNED),
    std::make_pair("catch", tc_CATCH),
    std::make_pair("else", tc_ELSE),
    std::make_pair("namespace", tc_NAMESPACE),
    std::make_pair("using", tc_USING),
    std::make_pair("new", tc_NEW),
    std::make_pair("virtual", tc_VIRTUAL),
    std::make_pair("explicit", tc_EXPLICIT),
    std::make_pair("noexcept", tc_NOEXCEPT),
    std::make_pair("export", tc_EXPORT),
    std::make_pair("switch", tc_SWITCH),
    std::make_pair("extern", tc_EXTERN),
    std::make_pair("operator", tc_OPERATOR),
    std::make_pair("template", tc_TEMPLATE),
    std::make_pair("const", tc_CONST),
    std::make_pair("private", tc_PRIVATE),
    //std::make_pair("this", tc_THIS),
    std::make_pair("while", tc_WHILE),
    std::make_pair("protected", tc_PROTECTED),
    std::make_pair("thread_local", tc_THREADLOCAL),
    std::make_pair("for", tc_FOR),
    std::make_pair("public", tc_PUBLIC),
    std::make_pair("throw", tc_THROW),
    std::make_pair("default", tc_DEFAULT),
    std::make_pair("typedef", tc_TYPEDEF),
    std::make_pair("mutable", tc_MUTABLE),
    std::make_pair("include", tc_INCLUDE),
    std::make_pair("warn", tc_WARN),
    std::make_pair("import", tc_IMPORT),
};


token_map cx_reserved_words(map_data, map_data + sizeof map_data / sizeof map_data[0]);

/*****************
 *               *
 *  Word Tokens  *
 *               *
 *****************/

/** get     Extract a word token from the source and downshift
 *          its characters.  Check if it's a reserved word.
 *
 * @param buffer : ptr to text input buffer.
 */
void cx_word_token::get(cx_text_in_buffer &buffer) {
    extern cx_char_code_map char_code_map;

    char ch = buffer.current_char(); // char fetched from input
    char *ps = string;

    // get the word.
    do {
        *ps++ = ch;
        ch = buffer.get_char();
    } while ((char_code_map[ch] == cc_letter)
            || (char_code_map[ch] == cc_digit));

    *ps = '\0';

    check_for_reserved_word();
}

/** check_for_reserved_word    Is the word token a reserved word?
 *                          If yes, set the its token code to
 *                          the appropriate code.  If not, set
 *                          the token code to tc_identifier.
 */
void cx_word_token::check_for_reserved_word(void) {

    code__ = tc_identifier; // first assume it's an identifier

    /* from the reserved word table, check to see if the word
     * is in there. */
    cx_token_code c_ = cx_reserved_words[this->string__()];
    if (c_ > 0) code__ = c_;

}

/** print       print the token to the list file.
 */
void cx_word_token::print(void) const {
    if (code__ == tc_identifier) {
        sprintf(list.text, "\t%-18s %-s", ">> identifier:", string);
    } else {
        sprintf(list.text, "\t%-18s %-s", ">> reserved word:", string);
    }

    list.put_line();
}
