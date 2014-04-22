/*
 * File:   types.h
 * Author: aaro3965
 *
 * Created on June 24, 2013, 12:28 AM
 */

#ifndef types_h
#define	types_h

#include <cstdio>
#include <fstream>

#include "error.h"
#include "symtab.h"

class cx_type;
class cx_symtab_node;
class cx_symtab;

// Pointers to predefined types.
extern cx_symtab_node *p_main_function_id;

extern cx_type *p_integer_type;
extern cx_type *p_uint8_type;
extern cx_type *p_float_type;
extern cx_type *p_boolean_type;
extern cx_type *p_char_type;
extern cx_type *p_wchar_type;
extern cx_type *p_void_type;
//cx_type *p_class_type = nullptr;
extern cx_type *p_complex_type;
//extern  cx_type *p_file_type;
extern cx_type *p_dummy_type;

enum cx_type_form_code {
    fc_none,
    fc_scalar,
    fc_enum,
    fc_subrange,
    fc_array,
    fc_complex,
    fc_stream
};

enum cx_type_code {
    cx_int = 0,
    cx_char,
    cx_wchar,
    cx_float,
    cx_bool,
    cx_uint8,
    cx_void,
    cx_address,
    cx_complex,
    cx_file
};

// below is for external liraries
#include <map>
typedef std::map<cx_type_code, cx_type *> cx_datatype;

extern const char *form_strings[];

class cx_type {
    int reference_count;
    bool is_constant__;

public:

    cx_type() { //cx_type_form_code form_code){
        //this->form = form_code;
    }

    //    const cx_type_form_code form;
    cx_type_form_code form;

    bool is_address;
    // size of the first element
    int size;

    bool is_temp_value;
    cx_symtab_node *p_type_id;
    cx_type_code type_code;
    // if multidimension array point to next element
    cx_type *next;

    union {

        struct {
            cx_symtab_node *p_const_ids;
            int max;
        } enumeration;

        struct {
            const cx_type *p_index_type;
            cx_type *p_element_type;
            int min_index;
            int max_index;
            int element_count;
        } array;

        struct {
            /* used only for internal to class.
             * connects all scopes to a single table */
            cx_symtab *p_class_scope;
        } complex;
    };

    //struct {
    /* used only for internal to class.
     * connects all scopes to a single table */
    //cx_symtab *p_class_scope_symtab;

    // seperate public, private and protected tables
    //cx_scoped_symtab MemberTable;
    //} complex;

    cx_type(cx_type_form_code fc, int s, cx_symtab_node *p_id, cx_symtab *p_members = nullptr);
    cx_type(int length, bool constant = false);

    ~cx_type(); // {};

    bool is_scalar_type(void) const {
        return (form != fc_array) &&
                (form != fc_complex) &&
                (form != fc_stream);
    }

    bool is_constant(void) const {
        return is_constant__;
    }

    cx_type *base_type(void) const {
        return form == fc_array ? array.p_element_type : (cx_type *) this;
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

    friend void check_bitwise_integer(const cx_type *p_type1);
    friend void check_bitwise_integer(const cx_type *p_type1, const cx_type *p_type2);
};

void initialize_builtin_types(cx_symtab *p_symtab);
void remove_builtin_types(void);



#endif	/* TYPES_H */

