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
void cx_lib_init(cx_symtab *p_symtab, const cx_type **ct) {


	// Integer members
	struct member {
		cx_symtab_node *p_node;
		std::string name;
		std::string symbol_name;
		const cx_type *p_type;
		int num_params;
		ext_call ext_f;
	} members[] = {
		// stream members
		{ nullptr, "abs", "cx_abs", ct[cx_int], 0, cx_abs },
		{ nullptr, "div", "cx_div", ct[cx_int], 1, cx_div },
		{ nullptr, "mod", "cx_mod", ct[cx_int], 1, cx_mod },
		{ nullptr, "close", "cx_close", ct[cx_bool], 0, cx_close },
		{ nullptr, "flush", "cx_flush", ct[cx_bool], 0, cx_flush },
		{ nullptr, "wide", "cx_wide", ct[cx_int], 1, cx_wide },
		{ nullptr, "read", "cx_read", new cx_type(fc_array, 0, nullptr), 2, cx_read },
		{ nullptr, "write", "cx_write", ct[cx_int], 3, cx_write },
		{ nullptr, "getc", "cx_getc", ct[cx_char], 0, cx_getc },
		{ nullptr, "gets", "cx_gets", new cx_type(fc_array, 0, nullptr), 1, cx_gets },
		{ nullptr, "putc", "cx_putc", ct[cx_int], 1, cx_putc },
		{ nullptr, "ungetc", "cx_ungetc", ct[cx_int], 1, cx_ungetc },

		//wide char
		{ nullptr, "getwc", "cx_getwc", ct[cx_wchar], 0, cx_getwc },
		{ nullptr, "getws", "cx_getws", new cx_type(fc_array, 0, nullptr), 1, cx_getws },
		{ nullptr, "putwc", "cx_putwc", ct[cx_wchar], 1, cx_putwc },
		{ nullptr, "putws", "cx_putws", ct[cx_bool], 1, cx_putws },
		{ nullptr, "ungetwc", "cx_ungetwc", ct[cx_wchar], 1, cx_ungetwc },

		// file positioning 
		{ nullptr, "tell", "cx_tell", ct[cx_int], 0, cx_tell },
		{ nullptr, "seek", "cx_seek", ct[cx_bool], 2, cx_seek },
		{ nullptr, "rewind", "cx_rewind", ct[cx_void], 0, cx_rewind },

		// error handling
		{ nullptr, "clearerr", "cx_clearerr", ct[cx_void], 0, cx_clearerr },
		{ nullptr, "eof", "cx_eof", ct[cx_bool], 0, cx_eof },
		{ nullptr, "error", "cx_error_", ct[cx_bool], 0, cx_error_ },

		// operations on files 
		{ nullptr, "tmpfile", "cx_tmpfile", ct[cx_bool], 0, cx_tmpfile }
	};

	for (auto &mbr : members) {
		mbr.p_node = std_stream_members->enter(mbr.name.c_str(), dc_function);

		mbr.p_node->defn.routine.which = func_std_member;
		mbr.p_node->defn.routine.ext_function = mbr.ext_f;
		mbr.p_node->defn.routine.parm_count = mbr.num_params;
		set_type(mbr.p_node->p_type, (cx_type *)mbr.p_type);

		if ((mbr.name == "puts") || ((mbr.name == "putws"))) {

			// char *str
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("str", dc_value_parm);
			mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
				mbr.p_node->defn.routine.locals.p_parms_ids);

			if (mbr.name == "puts") {
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
			}
			else {
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *)ct[cx_wchar]);
			}

		}
		else if ((mbr.name == "open") || (mbr.name == "reopen")) {

			// char *filename
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("filename", dc_value_parm);
			mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
				mbr.p_node->defn.routine.locals.p_parms_ids);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *)ct[cx_char]);

			// char *mode
			mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("mode", dc_value_parm);
			mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type = new cx_type(fc_array, 0,
				mbr.p_node->defn.routine.locals.p_parms_ids->next__);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
		}
		else if (mbr.name == "wide") {
			// int mode
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("mode", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_int]);
		}
		else if (mbr.name == "read") {
			// int size
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("size", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_int]);
			// int count
			mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("count", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *)ct[cx_int]);

		}
		else if (mbr.name == "write") {

			// byte *buffer
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("buffer", dc_value_parm);
			mbr.p_node->defn.routine.locals.p_parms_ids->p_type = new cx_type(fc_array, 0,
				mbr.p_node->defn.routine.locals.p_parms_ids);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *)ct[cx_uint8]);

			// int size
			mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("size", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *)ct[cx_int]);

			// int count
			mbr.p_node->defn.routine.locals.p_parms_ids->next__->next__ = new cx_symtab_node("count", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->next__->p_type, (cx_type *)ct[cx_int]);
		}
		else if ((mbr.name == "gets") || (mbr.name == "getws")) {

			// int count
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("count", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_int]);
		}
		else if ((mbr.name == "putc") || (mbr.name == "putwc")) {

			// int ch or wchar_t
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("ch", dc_value_parm);

			if (mbr.name == "putc") {
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_int]);
			}
			else {
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_wchar]);
			}
		}
		else if ((mbr.name == "ungetc") || (mbr.name == "ungetwc")) {

			// int ch
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("ch", dc_value_parm);

			if (mbr.name == "ungetc") {
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_int]);
			}
			else {
				set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_wchar]);
			}
		}
		else if (mbr.name == "seek") {
			// int offset
			mbr.p_node->defn.routine.locals.p_parms_ids = new cx_symtab_node("offset", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->p_type, (cx_type *)ct[cx_int]);
			// int origin
			mbr.p_node->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("origin", dc_value_parm);
			set_type(mbr.p_node->defn.routine.locals.p_parms_ids->next__->p_type, (cx_type *)ct[cx_int]);
		}


		if (mbr.name == "read") {
			set_type(mbr.p_node->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
		}
		else if (mbr.name == "gets") {
			set_type(mbr.p_node->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
		}
		else if (mbr.name == "getws") {
			set_type(mbr.p_node->p_type->array.p_element_type, (cx_type *)ct[cx_wchar]);
		}

	}

	cx_symtab_node *p_remove = p_symtab->enter("remove", dc_function);
	p_remove->defn.routine.which = func_standard;
	set_type(p_remove->p_type, (cx_type *)ct[cx_bool]);
	p_remove->defn.routine.parm_count = 1;
	p_remove->defn.routine.locals.p_parms_ids = new cx_symtab_node("filename", dc_value_parm);
	set_type(p_remove->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
	p_remove->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
	set_type(p_remove->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
	p_remove->defn.routine.ext_function = cx_remove;

	cx_symtab_node *p_perror = p_symtab->enter("perror", dc_function);
	p_perror->defn.routine.which = func_standard;
	set_type(p_perror->p_type, (cx_type *)ct[cx_void]);
	p_perror->defn.routine.parm_count = 1;
	p_perror->defn.routine.locals.p_parms_ids = new cx_symtab_node("str", dc_value_parm);
	set_type(p_perror->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
	p_perror->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
	set_type(p_perror->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
	p_perror->defn.routine.ext_function = cx_perror;

	cx_symtab_node *p_rename = p_symtab->enter("rename", dc_function);
	p_rename->defn.routine.which = func_standard;
	set_type(p_rename->p_type, (cx_type *)ct[cx_bool]);
	p_rename->defn.routine.parm_count = 2;
	p_rename->defn.routine.locals.p_parms_ids = new cx_symtab_node("old_filename", dc_value_parm);
	set_type(p_rename->defn.routine.locals.p_parms_ids->p_type, new cx_type(fc_array, 0, nullptr));
	p_rename->defn.routine.locals.p_parms_ids->p_type->type_code = cx_address;
	set_type(p_rename->defn.routine.locals.p_parms_ids->p_type->array.p_element_type, (cx_type *)ct[cx_char]);

	p_rename->defn.routine.locals.p_parms_ids->next__ = new cx_symtab_node("new_filename", dc_value_parm);
	set_type(p_rename->defn.routine.locals.p_parms_ids->next__->p_type, new cx_type(fc_array, 0, nullptr));
	p_rename->defn.routine.locals.p_parms_ids->next__->p_type->type_code = cx_address;
	set_type(p_rename->defn.routine.locals.p_parms_ids->next__->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
	p_rename->defn.routine.ext_function = cx_rename;

	cx_symtab_node *p_tmpnam = p_symtab->enter("tmpnam", dc_function);
	p_tmpnam->defn.routine.which = func_standard;
	set_type(p_tmpnam->p_type, new cx_type(fc_array, 0, nullptr));
	set_type(p_tmpnam->p_type->array.p_element_type, (cx_type *)ct[cx_char]);
	p_perror->defn.routine.parm_count = 0;
	p_tmpnam->defn.routine.ext_function = cx_tmpnam;

	// values

	cx_symtab_node *p_eof = p_symtab->enter("EOF", dc_constant);
	set_type(p_eof->p_type, (cx_type *)ct[cx_int]);
	p_eof->defn.constant.value.int__ = EOF;

	cx_symtab_node *p_fopen_max = p_symtab->enter("FOPEN_MAX", dc_constant);
	set_type(p_fopen_max->p_type, (cx_type *)ct[cx_int]);
	p_fopen_max->defn.constant.value.int__ = FOPEN_MAX;

	cx_symtab_node *p_filename_max = p_symtab->enter("FILENAME_MAX", dc_constant);
	set_type(p_filename_max->p_type, (cx_type *)ct[cx_int]);
	p_filename_max->defn.constant.value.int__ = FILENAME_MAX;

	cx_symtab_node *p_seek_set = p_symtab->enter("SEEK_SET", dc_constant);
	set_type(p_seek_set->p_type, (cx_type *)ct[cx_int]);
	p_seek_set->defn.constant.value.int__ = SEEK_SET;

	cx_symtab_node *p_seek_cur = p_symtab->enter("SEEK_CUR", dc_constant);
	set_type(p_seek_cur->p_type, (cx_type *)ct[cx_int]);
	p_seek_cur->defn.constant.value.int__ = SEEK_CUR;

	cx_symtab_node *p_seek_end = p_symtab->enter("SEEK_END", dc_constant);
	set_type(p_seek_end->p_type, (cx_type *)ct[cx_int]);
	p_seek_end->defn.constant.value.int__ = SEEK_END;

	cx_symtab_node *p_tmp_max = p_symtab->enter("TMP_MAX", dc_constant);
	set_type(p_tmp_max->p_type, (cx_type *)ct[cx_int]);
	p_tmp_max->defn.constant.value.int__ = TMP_MAX;

	cx_symtab_node *p_L_tmpnam = p_symtab->enter("L_tmpnam", dc_constant);
	set_type(p_L_tmpnam->p_type, (cx_type *)ct[cx_int]);
	p_L_tmpnam->defn.constant.value.int__ = L_tmpnam;

}