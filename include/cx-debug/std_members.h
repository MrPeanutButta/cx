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

class cx_std_members {
public:
    
    static cx_type *size(cx_executor *cx, 
             cx_symtab_node *cx_function_id, 
            const cx_type *p_type);

	static cx_type *length(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_str(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_wstr(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_int(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_chr(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_flt(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_bool(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_wchr(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *to_byte(cx_executor *cx,
		 cx_symtab_node *cx_function_id,
		const cx_type *p_type);

	static cx_type *each(cx_executor *cx,
		cx_symtab_node *cx_function_id,
		const cx_type *p_type);
};

extern void init_std_members(void);
extern cx_symtab *std_members;

#endif	/* STD_MEMBERS_H */

