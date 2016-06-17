/*
The MIT License (MIT)

Copyright (c) 2015 Aaron Hebert <aaron.hebert@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "parser.h"

#if defined _WIN32
#include <windows.h>
std::vector<HINSTANCE> windows_libs;
#elif defined __linux__
#include <dlfcn.h>
std::vector<void *> linux_libs;
#elif defined __APPLE__
// TODO
#endif

namespace cx{
	// env variable that holds the path to stdlib
#define __CX_STDLIB__   L"CX_STDLIB"

	void load_lib(const wchar_t *lib, symbol_table *p_symtab) {

		static const cx_type * cx_types_[] = {
			p_integer_type.get(),
			p_char_type.get(),
			p_double_type.get(),
			p_boolean_type.get(),
			p_byte_type.get(),
			p_void_type.get()
		};

		typedef void(*lib_init)(symbol_table *, const cx_type **);
		lib_init init_call = nullptr;
		bool f_runtime_linksuccess = false;

#ifdef _WIN32
		HINSTANCE lib_handle = nullptr;
#elif defined __linux__
		void *lib_handle = nullptr;
#endif

		std::wstring lib_path;
		wchar_t *env_path = _wgetenv(__CX_STDLIB__);

		if (env_path != nullptr) {
			lib_path = env_path;
#ifdef _WIN32
			lib_path += L"\\";
#elif defined __linux__
			lib_path += L"/";
#endif
		}

#if defined _WIN32
		std::wstring dll = std::wstring(lib) + L".dll";
		lib_path += dll;
		// Get a handle to the DLL module.
		lib_handle = LoadLibrary((LPCSTR)lib_path.c_str());
#elif defined __linux__
		std::string so = std::string(lib) + ".so";
		lib_path += so;
		// Get a handle to the SO module
		lib_handle = dlopen(lib_path.c_str(), RTLD_NOW);
#endif

		// If the handle is valid, try to get the function address.
		if (lib_handle != nullptr) {

#if defined _WIN32
			init_call = (lib_init)GetProcAddress(lib_handle, "cx_lib_init");
			windows_libs.push_back(lib_handle);
#elif defined __linux__
			init_call = (lib_init)dlsym(lib_handle, "cx_lib_init");
			linux_libs.push_back(lib_handle);
#endif

			// If the function address is valid, call the function.
			if (nullptr != init_call) {
				f_runtime_linksuccess = true;
				//pass our symbol table to the library with a array of allowed data types.
				(init_call)(p_symtab, cx_types_);
			}
			else {
#if defined __linux__
				std::cout << dlerror() << std::endl;
#endif
				cx_error(ERR_LIBRARY_NO_INIT);
			}

		}
		else {
#if defined __linux__
			std::cout << dlerror() << std::endl;
#endif
			cx_error(ERR_LOADING_LIBRARY);
		}
	}

	/** parse_execute_directive      Opens an external script module
	*                      for parsing.
	*
	*      #include <identifier>
	*
	* @param p_function_id : ptr to the routine owning this directive call.
	*/
	//	void parser::parse_execute_directive(symbol_table_node_ptr &p_function_id) {
	//
	//		//	extern symbol_table_node_ptr p_program_ptr_id;
	//
	//		switch (token) {
	//		case TC_INCLUDE:
	//		{
	//			std::wstring lib_path;
	//			wchar_t *env_path = _wgetenv(__CX_STDLIB__);
	//
	//			if (env_path != nullptr) {
	//				lib_path = env_path;
	//#ifdef _WIN32
	//				lib_path += L"\\";
	//#else
	//				lib_path += L"/";
	//#endif
	//			}
	//
	//			get_token();
	//
	//			lib_path += p_token->string;
	//			p_program_ptr_id->found_global_end = true;
	//
	//			// save current line number
	//			int old_line_num = current_line_number;
	//			current_line_number = 0;
	//
	//			/* true : stdlib module
	//			* returns nullptr */
	//			{
	//				std::unique_ptr<parser> p_parser;// = nullptr;
	//
	//				try {
	//					p_parser = std::make_unique<parser>(new source_buffer(lib_path.c_str()), true);
	//
	//					p_parser->parse();
	//				}
	//				catch (...) {
	//
	//				}
	//			}
	//
	//			current_line_number = old_line_num;
	//			p_program_ptr_id->found_global_end = false;
	//			get_token();
	//		}
	//		break;
	//		case TC_WARN:
	//		{
	//			get_token();
	//
	//			if (token != TC_STRING) {
	//				cx_error(ERR_UNEXPECTED_TOKEN);
	//			}
	//
	//			{
	//				std::wstring msg = p_token->string;
	//				msg[0] = ' ';
	//				msg[msg.length() - 1] = L'\0';
	//				std::wcerr << "warning:" << msg << std::endl;
	//			}
	//
	//			get_token();
	//		}
	//		break;
	//		case TC_IMPORT:
	//		{
	//			get_token();
	//
	//			load_lib(p_token->string, symtab_stack.get_current_symtab());
	//
	//			get_token();
	//		}
	//		break;
	//		default:
	//			break;
	//		}
	//	}
}