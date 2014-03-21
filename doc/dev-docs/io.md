Cx standard I/O design document

CX input/output is designed around C file streams because of their ease of use. Every node in the symbol table will have an instance of **class cx_define**; defined in **symtable.h**. The definition of the stream is:
```cpp
// in symtable.h
class cx_define{
// ...

	union{
// ...

        struct {
            FILE *stream;
        } io;

// ...
	};
// ...
}
```
The form of a stream is defined just as the form of an array is defined. A stream uses the form code of **fc_stream**, defined in **types.h**.
``` cpp
enum cx_type_form_code {
    fc_none,
    fc_scalar,
    fc_enum,
    fc_subrange,
    fc_array,
    fc_complex,
    fc_stream	// <-- defines a stream form
};
```
In Cx code, when a **file** type identifier is seen, the symtab node itself is pushed on to the stack to allow the standard library to pull the node off of the stack and access it's file stream.
``` cpp
// line 149 of factor.cpp
cx_type *cx_executor::execute_variable(const cx_symtab_node *p_id,
        bool address_flag) {

    cx_type *p_type = p_id->p_type;
    p_type->is_address = address_flag;

    if (p_type->form != fc_stream) {	// <-- if a stream push to stack

        // get the variable's runtime stack address.
        cx_stack_item *p_entry_id = run_stack.get_value_address(p_id);
        push((p_id->defn.how == dc_reference) || (!p_type->is_scalar_type())
                ? p_entry_id->basic_types.addr__ : p_entry_id);
    } else {
        push((void *) p_id);	// <-- here is a file stream node being pushed
        address_flag = true;
    }
```
After the stream node is TOS(top of stack) the IO library can access it. This is a simple example of how the library should perform stream operations.
``` cpp
// in stdio.cpp
STDIO_API
cx_type *cx_eof(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    // get node from TOS(top of stack)
    cx_symtab_node *p_snode = (cx_symtab_node *) p_stack->top()->basic_types.addr__;
    p_stack->pop();

    // push bool EOF state
    p_stack->push(new cx_stack_item((bool)(std::feof(p_snode->defn.io.stream) != 0)));

    return cx_function_id->p_type;
}
```
That's all there is to it. All file I/O is performed in **stdio** and is not a built in function of the Cx interpreter.