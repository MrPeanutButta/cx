#include <sstream>
#include "std_members.h"
#include "cx_stdio.h"
#include "types.h"
#include "symtable.h"

cx_symtab *std_type_members = nullptr;
cx_symtab *std_stream_members = nullptr;

void init_std_members (void) {
    // initialize after basic types are init

	enum member_code{
		mc_type,
		mc_stream
	};

	struct member{
		cx_symtab_node *p_node;
		member_code member_code;
		std::string name;
		cx_function_code func_code;
		m_call member_call;
		cx_type *p_type;
	} members[] = {
		{ nullptr, mc_type, "size", func_std_member, &cx_std_type_members::size, p_integer_type },
		{ nullptr, mc_type, "length", func_std_member, &cx_std_type_members::length, p_integer_type },
		{ nullptr, mc_type, "to_str", func_std_member, &cx_std_type_members::to_str, new cx_type(fc_array, 0, nullptr) },
		{ nullptr, mc_type, "to_wstr", func_std_member, &cx_std_type_members::to_wstr, new cx_type(fc_array, 0, nullptr) },
		{ nullptr, mc_type, "to_int", func_std_member, &cx_std_type_members::to_int, p_integer_type },
		{ nullptr, mc_type, "to_chr", func_std_member, &cx_std_type_members::to_chr, p_char_type },
		{ nullptr, mc_type, "to_flt", func_std_member, &cx_std_type_members::to_flt, p_float_type },
		{ nullptr, mc_type, "to_bool", func_std_member, &cx_std_type_members::to_bool, p_boolean_type },
		{ nullptr, mc_type, "to_wchr", func_std_member, &cx_std_type_members::to_wchr, p_wchar_type },
		{ nullptr, mc_type, "to_byte", func_std_member, &cx_std_type_members::to_byte, p_uint8_type },
		
		// stream members
		{ nullptr, mc_stream, "puts", func_std_member, &cx_stdio::puts, p_boolean_type },
		{ nullptr, mc_stream, "open", func_std_member, &cx_stdio::open, p_boolean_type },
		{ nullptr, mc_stream, "reopen", func_std_member, &cx_stdio::reopen, p_boolean_type },
		{ nullptr, mc_stream, "close", func_std_member, &cx_stdio::close, p_boolean_type },
		{ nullptr, mc_stream, "flush", func_std_member, &cx_stdio::flush, p_boolean_type },
		{ nullptr, mc_stream, "wide", func_std_member, &cx_stdio::wide, p_integer_type },
		{ nullptr, mc_stream, "read", func_std_member, &cx_stdio::read, new cx_type(fc_array, 0, nullptr) },
		{ nullptr, mc_stream, "write", func_std_member, &cx_stdio::write, p_integer_type },
		{ nullptr, mc_stream, "getc", func_std_member, &cx_stdio::getc, p_integer_type },
		{ nullptr, mc_stream, "gets", func_std_member, &cx_stdio::gets, new cx_type(fc_array, 0, nullptr) },
		{ nullptr, mc_stream, "putc", func_std_member, &cx_stdio::putc, p_integer_type },
		{ nullptr, mc_stream, "ungetc", func_std_member, &cx_stdio::ungetc, p_integer_type },

		//wide char
		{ nullptr, mc_stream, "getwc", func_std_member, &cx_stdio::getwc_, p_wchar_type },
		{ nullptr, mc_stream, "getws", func_std_member, &cx_stdio::getws_, new cx_type(fc_array, 0, nullptr) },
		{ nullptr, mc_stream, "putwc", func_std_member, &cx_stdio::putwc_, p_wchar_type },
		{ nullptr, mc_stream, "putws", func_std_member, &cx_stdio::putws_, p_boolean_type },
		{ nullptr, mc_stream, "ungetwc", func_std_member, &cx_stdio::ungetc, p_wchar_type },

		// file positioning 
		{ nullptr, mc_stream, "tell", func_std_member, &cx_stdio::tell, p_integer_type },
		{ nullptr, mc_stream, "seek", func_std_member, &cx_stdio::seek, p_boolean_type },
		{ nullptr, mc_stream, "rewind", func_std_member, &cx_stdio::rewind, p_void_type },

		// error handling
		{ nullptr, mc_stream, "clearerr", func_std_member, &cx_stdio::clearerr, p_void_type },
		{ nullptr, mc_stream, "eof", func_std_member, &cx_stdio::eof, p_boolean_type },
		{ nullptr, mc_stream, "error", func_std_member, &cx_stdio::error, p_boolean_type },

		// operations on files 
		{ nullptr, mc_stream, "tmpfile", func_std_member, &cx_stdio::tmpfile, p_boolean_type }
	};

    // allocate std member functions for basic types
	std_type_members = new cx_symtab;

    p_integer_type->complex.p_class_scope = std_type_members;
	p_uint8_type->complex.p_class_scope = std_type_members;
	p_float_type->complex.p_class_scope = std_type_members;
	p_boolean_type->complex.p_class_scope = std_type_members;
	p_char_type->complex.p_class_scope = std_type_members;
	p_wchar_type->complex.p_class_scope = std_type_members;

	// allocate stream members
	std_stream_members = new cx_symtab;

	p_file_type->complex.p_class_scope = std_stream_members;

    for (auto &mbr : members) {

		switch (mbr.member_code){
		case mc_type:
			// init basic members
			mbr.p_node = std_type_members->enter(mbr.name.c_str(), dc_function);
			mbr.p_node->defn.routine.parm_count = 0;
			mbr.p_node->defn.routine.total_parm_size = 0;

			break;
		case mc_stream:
			mbr.p_node = std_stream_members->enter(mbr.name.c_str(), dc_function);

			if ((mbr.name == "puts") || ((mbr.name == "putws"))){
				mbr.p_node->defn.routine.parm_count = 1;

				// char *str
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("str", dc_value_parm);
				mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
					mbr.p_node->defn.routine.locals.p_parms_ids);

				if (mbr.name == "puts"){
					set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, p_char_type);
				}
				else {
					set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, p_wchar_type);
				}

			} else if ((mbr.name == "open") || (mbr.name == "reopen")){
				mbr.p_node->defn.routine.parm_count = 2;
				
				// char *filename
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("filename", dc_value_parm);
				mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
					mbr.p_node->defn.routine.locals.p_parms_ids);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, p_char_type);

				// char *mode
				mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("mode", dc_value_parm);
				mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type = new cx_type(fc_array, 0,
					mbr.p_node->defn.routine.locals.p_parms_ids->next__);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type->array.p_element_type, p_char_type);
			}
			else if (mbr.name == "wide"){
				mbr.p_node->defn.routine.parm_count = 1;
				// int mode
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("mode", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_integer_type);
			}
			else if (mbr.name == "read"){
				mbr.p_node->defn.routine.parm_count = 2;
				// int size
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("size", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_integer_type);
				// int count
				mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("count", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, p_integer_type);

			}
			else if (mbr.name == "write"){
				mbr.p_node->defn.routine.parm_count = 3;
				
				// byte *buffer
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("buffer", dc_value_parm);
				mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
					mbr.p_node->defn.routine.locals.p_parms_ids);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, p_uint8_type);

				// int size
				mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("size", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, p_integer_type);
				
				// int count
				mbr.p_node->defn.routine.locals.p_parms_ids->next__->next__ = new cx_symtab_node("count", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->next__->p_type, p_integer_type);
			}
			else if ((mbr.name == "gets") || (mbr.name == "getws")){
				mbr.p_node->defn.routine.parm_count = 1;

				// int count
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("count", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_integer_type);
			}
			else if ((mbr.name == "putc") || (mbr.name == "putwc")){
				mbr.p_node->defn.routine.parm_count = 1;

				// int ch or wchar_t
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("ch", dc_value_parm);

				if (mbr.name == "putc"){
					set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_integer_type);
				}
				else {
					set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_wchar_type);
				}
			}
			else if ((mbr.name == "ungetc") || (mbr.name == "ungetwc")){
				mbr.p_node->defn.routine.parm_count = 1;

				// int ch
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("ch", dc_value_parm);

				if (mbr.name == "ungetc"){
					set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_integer_type);
				}
				else {
					set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_wchar_type);
				}
			}
			else if (mbr.name == "seek"){
				mbr.p_node->defn.routine.parm_count = 2;
				// int offset
				mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("offset", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, p_integer_type);
				// int origin
				mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("origin", dc_value_parm);
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, p_integer_type);
			}
			
			break;
		default: break;
		}

		set_type(mbr.p_node->p_type, mbr.p_type);
		mbr.p_node->defn.routine.std_member = mbr.member_call;
		mbr.p_node->defn.routine.which = mbr.func_code;

		if (mbr.name == "to_str") {
			set_type(mbr.p_node->p_type->array.p_element_type, p_char_type);
		}
		else if (mbr.name == "to_wstr") {
			set_type(mbr.p_node->p_type->array.p_element_type, p_wchar_type);
		}
		else if (mbr.name == "read"){
			set_type(mbr.p_node->p_type->array.p_element_type, p_char_type);
		}
		else if (mbr.name == "gets") {
			set_type(mbr.p_node->p_type->array.p_element_type, p_char_type);
		}
		else if (mbr.name == "getws") {
			set_type(mbr.p_node->p_type->array.p_element_type, p_wchar_type);
		}
    }
}

cx_type *cx_std_type_members::size(cx_executor* cx,
                               cx_symtab_node* cx_function_id,
                               const cx_type *p_type) {
    cx->pop();

	if (!p_type->is_address){
		
		cx_stack_item *p_size_val = new cx_stack_item;
		p_size_val->basic_types.int__ = p_type->size;
		
		cx->push((void*)p_size_val);
	}
	else {
		cx->push((int)p_type->size);
	}

    return cx_function_id->p_type;
}

cx_type *cx_std_type_members::length(cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

	cx->pop();

	if (!p_type->is_address){
		cx_stack_item *p_length_val = new cx_stack_item;

		p_type->form == fc_array ?
			p_length_val->basic_types.int__ = p_type->array.element_count :
			p_length_val->basic_types.int__ = 1;

		cx->push((void *)p_length_val);
	}
	else {
		cx->push((int)p_type->form == fc_array ? p_type->array.element_count : 1);
	}

    return cx_function_id->p_type;
}

/** @TODO - needs to be fixed for wide char
 */
cx_type *cx_std_type_members::to_str(cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    std::stringstream ss;
    ss.clear();

    cx_stack_item *mem = nullptr;

	if (!p_type->is_address){
		mem = (cx_stack_item *)cx->top()->basic_types.addr__;
	}
	else {
		mem = cx->top();
	}

    switch (p_type->type_code) {
        case cx_int:
            ss << mem->basic_types.int__ << '\0';
            break;
        case cx_char:
            ss << mem->basic_types.char__ << '\0';
            break;
        case cx_wchar:
            ss << mem->basic_types.wchar__ << '\0';
            break;
        case cx_float:
            ss << mem->basic_types.float__ << '\0';
            break;
        case cx_bool:
            ss << (mem->basic_types.bool__ ? "true" : "false") << '\0';
            break;
        case cx_uint8:
            ss << (int) mem->basic_types.uint8__ << '\0';
            break;
        default:
            ss << mem->basic_types.addr__ << '\0';
            break;
    }

    cx->pop();

    const int size = ss.str().size() - 1;
    const int length = ss.str().length() - 1;

    cx_type *p_str = new cx_type(fc_array, size, nullptr);
    set_type(p_str->array.p_element_type, p_char_type);
    p_str->array.element_count = length;
    p_str->array.max_index = length;

    char *p_to_str = (char *) malloc(size + 1);
    memcpy(p_to_str, ss.str().c_str(), size + 1);

    cx->push((void *) p_to_str);

    return p_str;
}

cx_type *cx_std_type_members::to_wstr(cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

	std::wstringstream ss;
	ss.clear();

	cx_stack_item *mem = nullptr;

	if (p_type->is_address){
		mem = (cx_stack_item *)cx->top()->basic_types.addr__;
	}
	else {
		mem = cx->top();
	}

	switch (p_type->type_code) {
	case cx_int:
		ss << mem->basic_types.int__ << L'\0';
		break;
	case cx_char:
		ss << mem->basic_types.char__ << L'\0';
		break;
	case cx_wchar:
		ss << mem->basic_types.wchar__ << L'\0';
		break;
	case cx_float:
		ss << mem->basic_types.float__ << L'\0';
		break;
	case cx_bool:
		ss << (mem->basic_types.bool__ ? L"true" : L"false") << L'\0';
		break;
	case cx_uint8:
		ss << (int)mem->basic_types.uint8__ << L'\0';
		break;
	default:
		ss << mem->basic_types.addr__ << L'\0';
		break;
	}

	cx->pop();

	const int size = ss.str().size() - 1;
	const int length = ss.str().length() - 1;

	cx_type *p_wstr = new cx_type(fc_array, size, nullptr);
	set_type(p_wstr->array.p_element_type, p_wchar_type);
	p_wstr->array.element_count = length;
	p_wstr->array.max_index = length;

	wchar_t *p_to_wstr = (wchar_t *)malloc((sizeof(wchar_t) * length) + 1);

	memset(p_to_wstr, 0, (sizeof(wchar_t)* length) + 1);

	memcpy(p_to_wstr, ss.str().c_str(), (sizeof(wchar_t) * length) + 1);

	cx->push((void *)p_to_wstr);

	return p_wstr;
}

cx_type *cx_std_type_members::to_int(cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_type_members::to_chr(cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_type_members::to_flt(cx_executor *cx,
                                 cx_symtab_node *cx_function_id,
                                 const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_type_members::to_bool(cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_type_members::to_wchr(cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

    return cx_function_id->p_type;
}

cx_type *cx_std_type_members::to_byte(cx_executor *cx,
                                  cx_symtab_node *cx_function_id,
                                  const cx_type *p_type) {

    return cx_function_id->p_type;
}