# Extending the Cx Interpreter
## Minimum Requirements
Extending the language requires a dynamic library to be compiled against **libcxapi**. The header file **include/cx-debug/cx_api.h** will need to be included in the project also. The library must contain the entry point **cx_lib_init** for the import directive to be successful.

The entry point of the library will be passed the current scopes symbol table and a array of basic data types that can be used.

Minimum required code for a dynamic library.
``` cpp
#if defined _WIN32
#define LIBRARY_API __declspec(dllexport)
#elif defined __linux__
#define LIBRARY_API extern "C"
#endif

// required entry point for all dynamic libraries
LIBRARY_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct){

}
```

## Define Codes
When defining an identifier you must specify a **Define Code**. The recognized **Define Codes** are:
``` cpp
enum cx_define_code {
        dc_undefined, dc_constant, dc_type, dc_variable, dc_member,
        dc_value_parm, dc_reference, dc_pointer, dc_program, dc_function
};
```

## Defining Constants
Declaring a constant must be done by entering the identifier name into the symbol table and setting the data type.
``` cpp

// required entry point for all dynamic libraries
LIBRARY_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct){

	cx_symtab_node *p_eof = p_symtab->enter("EOF", dc_constant);	// Enter 'EOF' ID into the table with a define code of constant
    set_type(p_eof->p_type, (cx_type *) ct[cx_int]);				// Set data type to an integer
    p_eof->defn.constant.value.int__ = EOF;							// Set the constant value

}
```

## Defining Types and Variables
User defined types can be defined just like with **file** types being defined in **stdio**. Types are recognized by the define code **dc_type**

### Basic Types
How new types are defined externally. This is an example of defining an integer **type** named **number** and a integer **variable** named **num**.
``` cpp

// required entry point for all dynamic libraries
LIBRARY_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct){

	// Will allow user to declare a 'number' type.
    cx_symtab_node *p_number_id = p_symtab->enter("number", dc_type);
	set_type(p_number_id->p_type, (cx_type *)ct[cx_int])

	
	// Will declare a integer variable named 'num'
    cx_symtab_node *p_number_id = p_symtab->enter("num", dc_variable);
	set_type(p_number_id->p_type, (cx_type *)ct[cx_int]);
}
```

### Arrays
Arrays are defined by the **form code** and are considered a new type.
``` cpp

// required entry point for all dynamic libraries
LIBRARY_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct){

    cx_symtab_node *p_number_id = p_symtab->enter("number", dc_type);

	// creates a new array of size 0
	set_type(p_number_id->p_type, new cx_type(fc_array, 0, p_number_id));

	// set the elements of the array to integer
	set_type(p_number->p_type->array.p_element_type, (cx_type *) ct[cx_int]);

}
```

## Defining Functions
Declaring functions is a little more involved. We must consider the parameters (value or reference) and return type. Also, we must set a pointer to the function we are defining. We are responsible for maintaining the stack and return type.
``` cpp
LIBRARY_API
cx_type *cx_remove(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const char *filename = (const char *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    p_stack->push((bool)(std::remove(filename) == 0));

    return cx_function_id->p_type;
}

LIBRARY_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct){

	// enter symbol
    cx_symtab_node *p_remove = p_symtab->enter("remove", dc_function);

	// declared as standard function
    p_remove->defn.routine.which = func_standard;

	// set return type to boolean
    set_type(p_remove->p_type, (cx_type *) ct[cx_bool]);

	// parameter count
    p_remove->defn.routine.parm_count = 1;

	// char *filename parameter identifier
    p_remove->defn.routine.locals.p_parms_ids = new cx_symtab_node("filename", dc_value_parm);
    set_type(p_remove->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
    p_remove->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
    set_type(p_remove->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);

	// pointer to our function defined above
    p_remove->defn.routine.ext_function = cx_remove;

}
```

## Defining User Types with Members
Defining a user defined type with members is simple, and we just need to enter one ID into the current symbol table then (**p_symtab**) create a new symbol table for the ID we just created. This is example code of how the **file** type is defiend followed by defining **stdin**. 
``` cpp
LIBRARY_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct){

    cx_type *p_file_type = nullptr;
    cx_symtab *std_stream_members = nullptr;

    cx_symtab_node *p_file_id = p_symtab->enter("file", dc_type);

    if (!p_file_type) {
        set_type(p_file_type, new cx_type(fc_stream, sizeof (FILE), p_file_id));
        p_file_type->complex.p_class_scope = new cx_symtab;
        std_stream_members = p_file_type->complex.p_class_scope;
    }

    set_type(p_file_id->p_type, p_file_type);
   
	// allocate stdin
	cx_symtab_node *p_stdin = p_symtab->enter("stdin", ::dc_variable);
    set_type(p_stdin->p_type, p_file_type);
    // just point directly at the real 'stdin'
	p_stdin->defn.io.stream = stdin;

	/* Time to add members to 'file' type. Added members 
	 * to 'file' will be available to anyinstance of 
	 * 'file' including 'stdin'	*/
	 
	// create a 'puts' function
	 struct member {
        cx_symtab_node *p_node;
        std::string name;
        std::string symbol_name;
        const cx_type *p_type;
        int num_params;
        ext_call ext_f;
    } members[] = {
        // stream members
        { nullptr, "puts", "cx_puts", ct[cx_bool], 1, cx_puts}};

	for (auto &mbr : members) {
        mbr.p_node = std_stream_members->enter(mbr.name.c_str(), dc_function);

        mbr.p_node->defn.routine.which = func_std_member;
        mbr.p_node->defn.routine.ext_function = mbr.ext_f;
        mbr.p_node->defn.routine.parm_count = mbr.num_params;
        set_type(mbr.p_node->p_type, (cx_type *) mbr.p_type);

		if (mbr.name == "puts") {

            // char *str
    		mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("str", dc_value_parm);
    		mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
                    mbr.p_node->defn.routine.locals.p_parms_ids);

    		set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
           
		}
	}
}
```