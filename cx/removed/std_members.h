/* 
 * File:   std_members.h
 * Author: aaro3965
 *
 * Created on December 8, 2013, 1:56 PM
 */

#ifndef STD_MEMBERS_H
#define	STD_MEMBERS_H

class cx_executor;
class cx_symtab;
class cx_symtab_node;
class cx_type;
class cx_runtime_stack;

class cx_std_type_members {
public:

    static cx_type *size(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *length(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_str(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_wstr(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_int(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_chr(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_flt(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_bool(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_wchr(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
    static cx_type *to_byte(cx_runtime_stack *p_stack, cx_symtab_node *p_node, const cx_type *p_type);
};

extern void init_std_members(void);
extern cx_symtab *std_type_members;

#endif	/* STD_MEMBERS_H */

