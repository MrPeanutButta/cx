#ifndef misc_h
#define misc_h

#include <map>
#include <string>

///  cx_char_code           Character codes.

enum cx_char_code {
    cc_letter, cc_digit, cc_special, cc_quote, cc_double_quote, cc_white_space,
    cc_end_of_file, cc_error
};

///  cx_token_code          Token codes.

enum cx_token_code {
    tc_dummy = 0,
    tc_identifier, tc_number, tc_string, tc_char, tc_end_of_file, tc_error,

    //operators and punctuation
    //bitwise
    tc_bit_XOR, // ^
    tc_bit_AND, // &
    tc_bit_OR, // |
    tc_bit_NOT, // ~
    tc_bit_XOR_equal, // ^=
    tc_bit_AND_equal, // &=
    tc_bit_OR_equal, // |=
    tc_bit_leftshift, // <<
    tc_bit_leftshift_equal, // <<=
    tc_bit_rightshift, // >>
    tc_bit_rightshift_equal, // >>=

    tc_minus, // -
    tc_minus_equal, // -=
    tc_plus, // +
    tc_plus_equal, // +=
    tc_equal, // =
    tc_minus_minus, // --
    tc_plus_plus, // ++
    tc_divide, // /
    tc_divide_equal, // /=
    tc_star, // *
    tc_star_equal, // *=
    tc_lessthan, // <
    tc_greaterthan, // >
    tc_equal_equal, // ==
    tc_lessthan_equal, // <=
    tc_greaterthan_equal, // >=
    tc_not_equal, // !=
    tc_modulas, // %
    tc_modulas_equal, // %=

    //punctuation
    tc_left_subscript, // [
    tc_right_subscript, // ]
    tc_questionmark, // ?
    tc_pound, // #
    tc_dot_star, // .*
    tc_left_paren, // (
    tc_right_paren, // )

    tc_left_bracket, // {
    tc_right_bracket, // }
    tc_colon, // :
    tc_semicolon, // ;

    tc_comma, // ,
    tc_dot_dot_dot, // ...

    // member access
    tc_dot, // .
    tc_dot_dot, // ..
    tc_colon_colon, // ::
    tc_pointer_member, // ->
    tc_member_pointer, // ->*

    //logic
    tc_logic_OR, // ||
    tc_logic_AND, // &&
    tc_logic_NOT, // !
    tc_single_quote, // '
    tc_double_quote, // "

    tc_IF, tc_RETURN, tc_CONTINUE, tc_FRIEND, tc_GOTO, tc_TRY, tc_DELETE,
    tc_TYPEID, tc_DO, tc_SIGNED, tc_TYPENAME,
    tc_BREAK, tc_SIZEOF, tc_CASE, tc_STATIC,
    tc_UNSIGNED, tc_CATCH, tc_ELSE, tc_NAMESPACE, tc_USING,
    tc_NEW, tc_VIRTUAL, tc_EXPLICIT, tc_NOEXCEPT,
    tc_EXPORT, tc_SWITCH,
    tc_EXTERN, tc_OPERATOR, tc_TEMPLATE, tc_CONST,
    tc_PRIVATE, tc_THIS, tc_WHILE, tc_PROTECTED, tc_THREADLOCAL,
    tc_FOR, tc_PUBLIC, tc_THROW, tc_DEFAULT, tc_TYPEDEF, tc_MUTABLE, tc_INCLUDE,

    mc_location_marker = 126,
    mc_line_marker = 127
};

///  cx_data_type           Data type.

enum cx_data_type {
    ty_dummy, ty_integer, ty_real, ty_character, ty_string,
};

///  cx_data_value          Data value.

union cx_data_value {
    int int__;
    unsigned int u_int__;
    float float__;
    double double__;
    char char__;
    unsigned char u_char__;

    bool bool__;

    //needed???
    char *p_string;
};

typedef std::map<char, cx_char_code> cx_char_code_map;
typedef std::map<std::string, cx_token_code> token_map;

#endif

