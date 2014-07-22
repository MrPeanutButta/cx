// std.cpp : Defines the exported functions for the DLL application.
//

#include "cx_api.h"
#include <cstdlib>

#if defined _WIN32

bool cx_dev_debug_flag;

BOOL APIENTRY DllMain(HMODULE hModule,
        DWORD ul_reason_for_call,
        LPVOID lpReserved
        ) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH:
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }
    return TRUE;
}

#define STDLIB_API __declspec(dllexport)
#elif defined __linux__
#define STDLIB_API extern "C"
#endif

STDLIB_API
cx_type *cx_rand(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

	int t = rand();
    p_stack->push((int) t);

    return cx_function_id->p_type;
}

STDLIB_API
cx_type *cx_srand(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    srand((unsigned int) p_stack->top()->basic_types.int__);
    p_stack->pop();

    return cx_function_id->p_type;
}

STDLIB_API
cx_type *cx_free(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    free(p_stack->top()->basic_types.addr__);
    p_stack->top()->basic_types.addr__ = nullptr;
    p_stack->pop();

    return cx_function_id->p_type;
}

STDLIB_API
cx_type *cx_abort(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    abort();

    return cx_function_id->p_type;
}

STDLIB_API
cx_type *cx_exit(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    exit(p_stack->top()->basic_types.int__);

    return cx_function_id->p_type;
}

STDLIB_API
cx_type *cx_getenv(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const char *name = (const char *)p_stack->top()->basic_types.addr__;

    char *p_env = getenv(name);

    p_stack->pop();

    p_stack->push((void *)p_env);

    cx_type *p_str = new cx_type;
    p_str->form = fc_array;
    p_str->type_code = cx_address;
    p_str->size = (p_env != nullptr ? strlen(p_env) : 0);
    set_type(p_str->array.p_element_type, (cx_type *) cx_function_id->p_type->base_type());
    p_str->array.element_count = p_str->size;
    p_str->array.max_index = p_str->size;

    return p_str;
}

STDLIB_API
cx_type *cx_system(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const char *p_command = (const char *)p_stack->top()->basic_types.addr__;
        p_stack->pop();

    p_stack->push((int)system(p_command));

    return cx_function_id->p_type;
}


STDLIB_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct) {

    cx_symtab_node *p_rand = p_symtab->enter("rand", dc_function);
    p_rand->defn.routine.which = func_standard;
    set_type(p_rand->p_type, (cx_type *) ct[cx_int]);
    p_rand->p_type->complex.p_class_scope = (cx_symtab *)ct[cx_std_member];
    p_rand->defn.routine.parm_count = 0;
    p_rand->defn.routine.ext_function = cx_rand;

    cx_symtab_node *p_srand = p_symtab->enter("srand", dc_function);
    p_srand->defn.routine.which = func_standard;
    set_type(p_srand->p_type, (cx_type *) ct[cx_void]);
    p_srand->p_type->complex.p_class_scope = (cx_symtab *)ct[cx_std_member];
    p_srand->defn.routine.parm_count = 1;
    p_srand->defn.routine.locals.p_parms_ids = new cx_symtab_node("seed", dc_value_parm);
    set_type(p_srand->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
    p_srand->defn.routine.locals.p_parms_ids->p_type->type_code = cx_int;
    p_srand->defn.routine.ext_function = cx_srand;

    cx_symtab_node *p_free = p_symtab->enter("free", dc_function);
    p_free->defn.routine.which = func_standard;
    set_type(p_free->p_type, (cx_type *) ct[cx_void]);
    p_free->defn.routine.parm_count = 1;
    p_free->defn.routine.locals.p_parms_ids = new cx_symtab_node("ptr", dc_reference);
    set_type(p_free->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_void]);
    p_free->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
    p_free->defn.routine.ext_function = cx_free;

    cx_symtab_node *p_abort = p_symtab->enter("abort", dc_function);
    p_abort->defn.routine.which = func_standard;
    set_type(p_abort->p_type, (cx_type *) ct[cx_void]);
    p_abort->defn.routine.parm_count = 0;
    p_abort->defn.routine.ext_function = cx_abort;

    cx_symtab_node *p_exit = p_symtab->enter("exit", dc_function);
    p_exit->defn.routine.which = func_standard;
    set_type(p_exit->p_type, (cx_type *) ct[cx_void]);
    p_exit->defn.routine.parm_count = 1;
    p_exit->defn.routine.locals.p_parms_ids = new cx_symtab_node("status", dc_value_parm);
    set_type(p_exit->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_int]);
    p_exit->defn.routine.locals.p_parms_ids->p_type->type_code = cx_int;
    p_exit->defn.routine.ext_function = cx_exit;

    cx_symtab_node *p_getenv = p_symtab->enter("getenv", dc_function);
    p_getenv->defn.routine.which = func_standard;
    set_type(p_getenv->p_type, new cx_type(fc_array, 0, nullptr));
    p_getenv->p_type->complex.p_class_scope = (cx_symtab *)ct[cx_std_member];
    set_type(p_getenv->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
    p_getenv->defn.routine.parm_count = 1;
    p_getenv->defn.routine.locals.p_parms_ids = new cx_symtab_node("name", dc_value_parm);
    set_type(p_getenv->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
    set_type(p_getenv->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
    p_getenv->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
    p_getenv->defn.routine.ext_function = cx_getenv;
    
    cx_symtab_node *p_system = p_symtab->enter("system", dc_function);
    p_system->defn.routine.which = func_standard;
    set_type(p_system->p_type, (cx_type *)ct[cx_int]);
    p_system->p_type->complex.p_class_scope = (cx_symtab *)ct[cx_std_member];
    p_system->defn.routine.parm_count = 1;
    p_system->defn.routine.locals.p_parms_ids = new cx_symtab_node("command", dc_value_parm);
    set_type(p_system->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
    set_type(p_system->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *) ct[cx_char]);
    p_system->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
    p_system->defn.routine.ext_function = cx_system;
}