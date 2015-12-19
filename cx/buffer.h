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

#ifndef BUFFER_H
#define BUFFER_H

#include <fstream>
#include <cstdio>
#include <cstring>
#include <memory>
#include <wchar.h>
#include "error.h"

namespace cx{

#define MAX_INPUT_BUFFER_SIZE 1024

	class list_buffer;

	namespace buffer {
		extern const char EOF_CHAR;
		extern int input_position;
		extern bool list_flag;
		extern int level;
		extern list_buffer list;
		extern int current_line_number;
		extern const int max_printline_length;
	}

	///  text_in_buffer       Abstract text input buffer class.

	class text_in_buffer {
	protected:
		std::wifstream file;					// Input text file
		std::wstring file_name_;				// File name
		wchar_t text[MAX_INPUT_BUFFER_SIZE];	// Input text buffer
		wchar_t *p_char;						// Pointer to the current char position
		virtual wchar_t get_line(void) = 0;
	public:
		text_in_buffer(std::wstring input_file_name, abort_code ac);
		virtual ~text_in_buffer(void) {	file.close();}
		const wchar_t *file_name(void) { return file_name_.c_str(); }
		wchar_t current_char(void) {return *p_char;}
		wchar_t get_char(void);
		wchar_t put_back_char(void);
	};

	///  cx_source_buffer       Source buffer subclass of cx_text_in_buffer.

	class source_buffer : public text_in_buffer {
		virtual wchar_t get_line(void);

	public:
		source_buffer(std::wstring source_file_name);
	};

	///  cx_text_out_buffer      Abstract text output buffer class.

	class text_out_buffer {
	public:
		wchar_t text[MAX_INPUT_BUFFER_SIZE + 16]; // output text buffer
		virtual ~text_out_buffer() = 0;
		virtual void put_line(void) = 0;

		void put_line(const wchar_t *p_text) {
			wcscpy(text, p_text);
			put_line();
		}
	};

	///  cx_list_buffer         List buffer subclass of cx_text_out_buffer.

	class list_buffer : public text_out_buffer {
		std::wstring source_file_name; // ptr to source file name (for page header)
		int line_count; // count of lines in the current page
	public:

		virtual ~list_buffer(void) {}

		void initialize(const std::wstring file_name);
		virtual void put_line(void);

		void put_line(const wchar_t *p_text) {
			text_out_buffer::put_line(p_text);
		}

		void wbuffer(const wchar_t *p_text, int line_number, int nesting_level) {
			//swprintf(text, L"%4d %d: %s", line_number, nesting_level, p_text);
			_swprintf(text, L"%4d %d: %s", line_number, nesting_level, p_text);
		}
	};

	typedef std::shared_ptr<text_in_buffer> text_in_buffer_ptr;
}

#endif
