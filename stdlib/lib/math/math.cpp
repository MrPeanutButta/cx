#include "cx_api.h"
#include <cmath>

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

#define MATH_API __declspec(dllexport)
#elif defined __linux__
#define MATH_API extern "C"
#endif

MATH_API
cx_type *cx_cos(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) cos(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_sin(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) sin(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_tan(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) tan(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_acos(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) acos(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_asin(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) asin(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_atan(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) atan(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_atan2(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    const float y = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) atan2(y, x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_cosh(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) cosh(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_sinh(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) sinh(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_tanh(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) tanh(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_acosh(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) acosh(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_asinh(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) asinh(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_atanh(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) atanh(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_exp(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) exp(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_frexp(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    int *exp_ = &p_stack->top()->basic_types.int__;
    p_stack->pop();
    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    //p_stack->push(new cx_stack_item((float) frexp(x, exp)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_ldexp(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const int exp_ = p_stack->top()->basic_types.int__;
    p_stack->pop();
    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    //p_stack->push(new cx_stack_item((float) std::ldexp((float)x, (int)exp)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_log(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    double x = (double) p_stack->top()->basic_types.float__;
    p_stack->pop();
    
    double y = log(x);
    p_stack->push(new cx_stack_item((float)log(float(x))));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_log10(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) log10(float(x))));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_modf(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    float *intpart_ = &p_stack->top()->basic_types.float__;
    p_stack->pop();
    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) std::modf(x, intpart_)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_exp2(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) exp2(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_expm1(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) expm1(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_ilogb(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((int) ilogb(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_log1p(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) log1p(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_log2(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) log2((float) x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_scalbn(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const int n = p_stack->top()->basic_types.int__;
    p_stack->pop();
    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) scalbn(x, n)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_pow(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float exponent = p_stack->top()->basic_types.float__;
    p_stack->pop();
    const float base = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) pow(base, exponent)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_sqrt(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) sqrt(float(x))));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_cbrt(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) cbrt(x)));

    return cx_function_id->p_type;
}

MATH_API
cx_type *cx_hypot(cx_runtime_stack *p_stack,
        cx_symtab_node *cx_function_id,
        const cx_type *p_type) {

    const float y = p_stack->top()->basic_types.float__;
    p_stack->pop();
    const float x = p_stack->top()->basic_types.float__;
    p_stack->pop();
    p_stack->push(new cx_stack_item((float) hypot(x, y)));

    return cx_function_id->p_type;
}

MATH_API
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct) {


    cx_symtab_node *p_cos = p_symtab->enter("cos", dc_function);
    p_cos->defn.routine.which = func_standard;
    set_type(p_cos->p_type, (cx_type *) ct[cx_float]);
    p_cos->defn.routine.parm_count = 1;
    p_cos->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_cos->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_cos->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_cos->defn.routine.ext_function = cx_cos;

    cx_symtab_node *p_sin = p_symtab->enter("sin", dc_function);
    p_sin->defn.routine.which = func_standard;
    set_type(p_sin->p_type, (cx_type *) ct[cx_float]);
    p_sin->defn.routine.parm_count = 1;
    p_sin->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_sin->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_sin->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_sin->defn.routine.ext_function = cx_sin;

    cx_symtab_node *p_tan = p_symtab->enter("tan", dc_function);
    p_tan->defn.routine.which = func_standard;
    set_type(p_tan->p_type, (cx_type *) ct[cx_float]);
    p_tan->defn.routine.parm_count = 1;
    p_tan->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_tan->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_tan->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_tan->defn.routine.ext_function = cx_tan;

    cx_symtab_node *p_acos = p_symtab->enter("acos", dc_function);
    p_acos->defn.routine.which = func_standard;
    set_type(p_acos->p_type, (cx_type *) ct[cx_float]);
    p_acos->defn.routine.parm_count = 1;
    p_acos->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_acos->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_acos->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_acos->defn.routine.ext_function = cx_acos;

    cx_symtab_node *p_asin = p_symtab->enter("asin", dc_function);
    p_asin->defn.routine.which = func_standard;
    set_type(p_asin->p_type, (cx_type *) ct[cx_float]);
    p_asin->defn.routine.parm_count = 1;
    p_asin->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_asin->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_asin->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_asin->defn.routine.ext_function = cx_asin;

    cx_symtab_node *p_atan = p_symtab->enter("atan", dc_function);
    p_atan->defn.routine.which = func_standard;
    set_type(p_atan->p_type, (cx_type *) ct[cx_float]);
    p_atan->defn.routine.parm_count = 1;
    p_atan->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_atan->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_atan->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_atan->defn.routine.ext_function = cx_atan;

    cx_symtab_node *p_atan2 = p_symtab->enter("atan2", dc_function);
    p_atan2->defn.routine.which = func_standard;
    set_type(p_atan2->p_type, (cx_type *) ct[cx_float]);
    p_atan2->defn.routine.parm_count = 2;
    p_atan2->defn.routine.locals.p_parms_ids = new cx_symtab_node("y", dc_value_parm);
    set_type(p_atan2->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_atan2->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_atan2->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("x", dc_value_parm);
    set_type(p_atan2->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_float]);
    p_atan2->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_float;
    p_atan2->defn.routine.ext_function = cx_atan2;

    cx_symtab_node *p_cosh = p_symtab->enter("cosh", dc_function);
    p_cosh->defn.routine.which = func_standard;
    set_type(p_cosh->p_type, (cx_type *) ct[cx_float]);
    p_cosh->defn.routine.parm_count = 1;
    p_cosh->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_cosh->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_cosh->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_cosh->defn.routine.ext_function = cx_cosh;

    cx_symtab_node *p_sinh = p_symtab->enter("sinh", dc_function);
    p_sinh->defn.routine.which = func_standard;
    set_type(p_sinh->p_type, (cx_type *) ct[cx_float]);
    p_sinh->defn.routine.parm_count = 1;
    p_sinh->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_sinh->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_sinh->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_sinh->defn.routine.ext_function = cx_sinh;

    cx_symtab_node *p_tanh = p_symtab->enter("tanh", dc_function);
    p_tanh->defn.routine.which = func_standard;
    set_type(p_tanh->p_type, (cx_type *) ct[cx_float]);
    p_tanh->defn.routine.parm_count = 1;
    p_tanh->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_tanh->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_tanh->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_tanh->defn.routine.ext_function = cx_tanh;

    cx_symtab_node *p_acosh = p_symtab->enter("acosh", dc_function);
    p_acosh->defn.routine.which = func_standard;
    set_type(p_acosh->p_type, (cx_type *) ct[cx_float]);
    p_acosh->defn.routine.parm_count = 1;
    p_acosh->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_acosh->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_acosh->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_acosh->defn.routine.ext_function = cx_acosh;

    cx_symtab_node *p_asinh = p_symtab->enter("asinh", dc_function);
    p_asinh->defn.routine.which = func_standard;
    set_type(p_asinh->p_type, (cx_type *) ct[cx_float]);
    p_asinh->defn.routine.parm_count = 1;
    p_asinh->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_asinh->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_asinh->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_asinh->defn.routine.ext_function = cx_asinh;

    cx_symtab_node *p_atanh = p_symtab->enter("atanh", dc_function);
    p_atanh->defn.routine.which = func_standard;
    set_type(p_atanh->p_type, (cx_type *) ct[cx_float]);
    p_atanh->defn.routine.parm_count = 1;
    p_atanh->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_atanh->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_atanh->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_atanh->defn.routine.ext_function = cx_atanh;

    cx_symtab_node *p_exp = p_symtab->enter("exp", dc_function);
    p_exp->defn.routine.which = func_standard;
    set_type(p_exp->p_type, (cx_type *) ct[cx_float]);
    p_exp->defn.routine.parm_count = 1;
    p_exp->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_exp->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_exp->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_exp->defn.routine.ext_function = cx_exp;

    cx_symtab_node *p_frexp = p_symtab->enter("frexp", dc_function);
    p_frexp->defn.routine.which = func_standard;
    set_type(p_frexp->p_type, (cx_type *) ct[cx_float]);
    p_frexp->defn.routine.parm_count = 2;
    p_frexp->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_frexp->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_frexp->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_frexp->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("exp", dc_reference);
    set_type(p_frexp->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_int]);
    p_frexp->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_int;
    p_frexp->defn.routine.ext_function = cx_frexp;

    cx_symtab_node *p_ldexp = p_symtab->enter("ldexp", dc_function);
    p_ldexp->defn.routine.which = func_standard;
    set_type(p_ldexp->p_type, (cx_type *) ct[cx_float]);
    p_ldexp->defn.routine.parm_count = 2;
    p_ldexp->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_ldexp->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_ldexp->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_ldexp->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("exp", dc_value_parm);
    set_type(p_ldexp->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_int]);
    p_ldexp->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_int;
    p_ldexp->defn.routine.ext_function = cx_ldexp;

    cx_symtab_node *p_log = p_symtab->enter("log", dc_function);
    p_log->defn.routine.which = func_standard;
    set_type(p_log->p_type, (cx_type *) ct[cx_float]);
    p_log->defn.routine.parm_count = 1;
    p_log->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_log->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_log->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_log->defn.routine.ext_function = cx_log;

    cx_symtab_node *p_log10 = p_symtab->enter("log10", dc_function);
    p_log10->defn.routine.which = func_standard;
    set_type(p_log10->p_type, (cx_type *) ct[cx_float]);
    p_log10->defn.routine.parm_count = 1;
    p_log10->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_log10->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_log10->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_log10->defn.routine.ext_function = cx_log10;

    cx_symtab_node *p_modf = p_symtab->enter("modf", dc_function);
    p_modf->defn.routine.which = func_standard;
    set_type(p_modf->p_type, (cx_type *) ct[cx_float]);
    p_modf->defn.routine.parm_count = 2;
    p_modf->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_modf->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_modf->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_modf->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("intpart", dc_reference);
    set_type(p_modf->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_float]);
    p_modf->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_float;
    p_modf->defn.routine.ext_function = cx_modf;

    cx_symtab_node *p_exp2 = p_symtab->enter("exp2", dc_function);
    p_exp2->defn.routine.which = func_standard;
    set_type(p_exp2->p_type, (cx_type *) ct[cx_float]);
    p_exp2->defn.routine.parm_count = 1;
    p_exp2->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_exp2->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_exp2->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_exp2->defn.routine.ext_function = cx_exp2;

    cx_symtab_node *p_expm1 = p_symtab->enter("expm1", dc_function);
    p_expm1->defn.routine.which = func_standard;
    set_type(p_expm1->p_type, (cx_type *) ct[cx_float]);
    p_expm1->defn.routine.parm_count = 1;
    p_expm1->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_expm1->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_expm1->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_expm1->defn.routine.ext_function = cx_expm1;

    cx_symtab_node *p_ilogb = p_symtab->enter("ilogb", dc_function);
    p_ilogb->defn.routine.which = func_standard;
    set_type(p_ilogb->p_type, (cx_type *) ct[cx_int]);
    p_ilogb->defn.routine.parm_count = 1;
    p_ilogb->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_ilogb->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_ilogb->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_ilogb->defn.routine.ext_function = cx_ilogb;

    cx_symtab_node *p_log1p = p_symtab->enter("log1p", dc_function);
    p_log1p->defn.routine.which = func_standard;
    set_type(p_log1p->p_type, (cx_type *) ct[cx_float]);
    p_log1p->defn.routine.parm_count = 1;
    p_log1p->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_log1p->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_log1p->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_log1p->defn.routine.ext_function = cx_log1p;

    cx_symtab_node *p_log2 = p_symtab->enter("log2", dc_function);
    p_log2->defn.routine.which = func_standard;
    set_type(p_log2->p_type, (cx_type *) ct[cx_float]);
    p_log2->defn.routine.parm_count = 1;
    p_log2->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_log2->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_log2->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_log2->defn.routine.ext_function = cx_log2;

    cx_symtab_node *p_scalbn = p_symtab->enter("scalbn", dc_function);
    p_scalbn->defn.routine.which = func_standard;
    set_type(p_scalbn->p_type, (cx_type *) ct[cx_float]);
    p_scalbn->defn.routine.parm_count = 2;
    p_scalbn->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_scalbn->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_scalbn->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_scalbn->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("n", dc_value_parm);
    set_type(p_scalbn->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_int]);
    p_scalbn->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_int;
    p_scalbn->defn.routine.ext_function = cx_scalbn;

    cx_symtab_node *p_pow = p_symtab->enter("pow", dc_function);
    p_pow->defn.routine.which = func_standard;
    set_type(p_pow->p_type, (cx_type *) ct[cx_float]);
    p_pow->defn.routine.parm_count = 2;
    p_pow->defn.routine.locals.p_parms_ids = new cx_symtab_node("base", dc_value_parm);
    set_type(p_pow->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_pow->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_pow->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("exponent", dc_value_parm);
    set_type(p_pow->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_float]);
    p_pow->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_float;
    p_pow->defn.routine.ext_function = cx_pow;

    cx_symtab_node *p_sqrt = p_symtab->enter("sqrt", dc_function);
    p_sqrt->defn.routine.which = func_standard;
    set_type(p_sqrt->p_type, (cx_type *) ct[cx_float]);
    p_sqrt->defn.routine.parm_count = 1;
    p_sqrt->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_sqrt->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_sqrt->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_sqrt->defn.routine.ext_function = cx_sqrt;

    cx_symtab_node *p_cbrt = p_symtab->enter("cbrt", dc_function);
    p_cbrt->defn.routine.which = func_standard;
    set_type(p_cbrt->p_type, (cx_type *) ct[cx_float]);
    p_cbrt->defn.routine.parm_count = 1;
    p_cbrt->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_cbrt->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_cbrt->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_cbrt->defn.routine.ext_function = cx_cbrt;

    cx_symtab_node *p_hypot = p_symtab->enter("hypot", dc_function);
    p_hypot->defn.routine.which = func_standard;
    set_type(p_hypot->p_type, (cx_type *) ct[cx_float]);
    p_hypot->defn.routine.parm_count = 2;
    p_hypot->defn.routine.locals.p_parms_ids = new cx_symtab_node("x", dc_value_parm);
    set_type(p_hypot->defn.routine.locals.p_parms_ids->p_type, (cx_type *) ct[cx_float]);
    p_hypot->defn.routine.locals.p_parms_ids->p_type->type_code = cx_float;
    p_hypot->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("y", dc_value_parm);
    set_type(p_hypot->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *) ct[cx_float]);
    p_hypot->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_float;
    p_hypot->defn.routine.ext_function = cx_hypot;

    /*cx_symtab_node *p_eof = p_symtab->enter("EOF", dc_constant);
    set_type(p_eof->p_type, (cx_type *) ct[cx_int]);
    p_eof->defn.constant.value.int__ = EOF;*/

}