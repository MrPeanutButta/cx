#ifndef error_h
#define error_h

extern int error_count;
extern bool error_arrow_flag;
extern int error_arrow_offset;

///  Abort codes for fatal translator errors.

enum cx_abort_code {
    abort_invalid_commandline_args = -1,
    abort_source_file_open_failed = -2,
    abort_iform_file_open_failed = -3,
    abort_assembly_file_open_failed = -4,
    abort_too_many_syntax_errors = -5,
    abort_stack_overflow = -6,
    abort_code_segment_overflow = -7,
    abort_nesting_too_deep = -8,
    abort_runtime_error = -9,
    abort_unimplemented_feature = -10,
};

void abort_translation(cx_abort_code ac);

///  cx_error codes for syntax errors.

enum cx_error_code {
    err_none,
    err_unrecognizable,
    err_too_many,
    err_unexpected_eof,
    err_invalid_number,
    err_invalid_fraction,
    err_invalid_exponent,
    err_too_many_digits,
    err_real_out_of_range,
    err_integer_out_of_range,
    err_missing_right_paren,
    err_invalid_expression,
    err_invalid_assignment,
    err_missing_identifier,
    err_missing_equal,
    err_undefined_identifier,
    err_stack_overflow,
    err_invalid_statement,
    err_unexpected_token,
    err_missing_semicolon,
    err_missing_comma,
    err_missing_DO,
    err_missing_WHILE,
    errMissingTHEN, // deprecated
    errInvalidFORControl, // deprecated
    errMissingOF, // deprecated
    err_invalid_constant,
    err_missing_constant,
    err_missing_colon,
    err_missing_left_subscript,
    err_missing_right_subscript,
    err_redefined_identifier,
    err_missing_equal_equal,
    err_invalid_type,
    err_not_a_type_identifier,
    err_invalid_subrange_type, // deprecated
    err_not_a_constant_identifier,
    errMissingDotDot, // deprecated
    err_incompatible_types,
    err_invalid_target,
    err_invalid_identifier_usage,
    err_incompatible_assignment,
    errMinGtMax, // deprecated
    err_missing_left_bracket,
    err_missing_right_bracket,
    err_invalid_index_type,
    errMissingBEGIN, // deprecated
    err_missing_return,
    err_too_many_subscripts,
    err_invalid_field,
    err_nesting_too_deep,
    errMissingPROGRAM, // deprecated
    err_already_forwarded,
    err_wrong_number_of_parms,
    err_invalid_reference,
    err_not_a_record_variable,
    err_missing_variable,
    err_code_segment_overflow,
    err_unimplemented_feature,
    err_missing_left_paren,
    err_missing_single_quote,
    err_invalid_escape_char,
	err_loading_library,
	err_library_no_init
};

void cx_error(cx_error_code ec);

enum cx_runtime_error_code {
    rte_none,
    rte_stack_overflow,
    rte_value_out_of_range,
    rte_invalid_case_value,
    rte_division_by_zero,
    rte_invalid_function_argument,
    rte_invalid_user_input,
    rte_unimplemented_runtime_feature
};

void cx_runtime_error(cx_runtime_error_code ec);


#endif
//endfig
