/*
 * File:   types.h
 * Author: aaro3965
 *
 * Created on June 24, 2013, 12:28 AM
 */

#ifndef types_h
#define	types_h

#include <cstdio>
#include "error.h"
#include "symtable.h"



extern cx_type *p_integer_type, *p_float_type, *p_boolean_type, *p_char_type,
        *p_dummy_type, *p_complex_type, *p_file_type;

extern cx_symtab_node *p_main_function_id;

enum cx_type_form_code {
    fc_none, fc_scalar, fc_enum, fc_subrange, fc_array, fc_complex, fc_stream
};

extern const char *form_strings[];

class cx_type {
    int reference_count;
public:
    cx_type_form_code form;
    int size;
    cx_symtab_node *p_type_id;

    union {

        struct {
            cx_symtab_node *p_const_ids;
            int max;
        } enumeration;

        struct {
            cx_type *p_base_type;
            int min, max;
        } subrange;

        struct {
            cx_type *p_index_type;
            cx_type *p_element_type;
            int min_index, maxIndex;
            int element_count;
        } array;

        struct {
            /* used only for internal to class.
             * connects all scopes to a single table */
            cx_symtab *p_class_scope_symtab;

            // seperate public, private and protected tables
            //cx_scoped_symtab MemberTable;
        } complex;

        struct {
            // file stream
            FILE *p_file_stream;
        } stream;
    };

    //struct {
    /* used only for internal to class.
     * connects all scopes to a single table */
    //cx_symtab *p_class_scope_symtab;

    // seperate public, private and protected tables
    //cx_scoped_symtab MemberTable;
    //} complex;

    cx_type(cx_type_form_code fc, int s, cx_symtab_node *p_id);
    cx_type(int length);

    ~cx_type();

    bool is_scalar_type(void) const {
        return (form != fc_array) &&
                (form != fc_complex) &&
                (form != fc_stream);
    }

    cx_type *base_type(void) const {
        return form == fc_subrange ? subrange.p_base_type : (cx_type *) this;
    }

    enum cx_verbosity_code {
        vc_verbose, vc_terse
    };

    void print_type_spec(cx_verbosity_code vc);
    void print_enum_type(cx_verbosity_code vc) const;
    void print_subrange_type(cx_verbosity_code vc) const;
    void print_array_type(cx_verbosity_code vc) const;
    void print_record_type(cx_verbosity_code vc);

    friend cx_type *set_type(cx_type *&p_target_type, cx_type *p_source_type);
    friend void remove_type(cx_type *&p_type);

    friend void check_relational_op_operands(const cx_type *p_type1,
            const cx_type *p_type2);
    friend void check_integer_or_real(const cx_type *p_type1,
            const cx_type *p_type2 = nullptr);
    friend void check_boolean(const cx_type *p_type1,
            const cx_type *p_type2 = nullptr);
    friend void check_assignment_type_compatible(const cx_type *p_target_type,
            const cx_type *p_value_type,
            cx_error_code ec);
    friend bool integer_operands(const cx_type *p_type1,
            const cx_type *p_type2);
    friend bool real_operands(const cx_type *p_type1,
            const cx_type *p_type2);
};

void initialize_builtin_types(cx_symtab *p_symtab);
void remove_builtin_types(void);



#endif	/* TYPES_H */

