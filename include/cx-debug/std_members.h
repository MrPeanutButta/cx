/* 
 * File:   std_members.h
 * Author: aaro3965
 *
 * Created on December 8, 2013, 1:56 PM
 */

#ifndef STD_MEMBERS_H
#define	STD_MEMBERS_H

class cx_executor;
class cx_symtab_node;
class cx_type;

class cx_std_members {
public:
    
    static cx_type *size(cx_executor *cx, 
            const cx_symtab_node *cx_function_id, 
            const cx_type *p_type);
    
};

extern void init_std_members(void);

#endif	/* STD_MEMBERS_H */

