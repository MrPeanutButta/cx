#pragma once

#include <fstream>
#include <cstdio>
#include <cstring>
#include <memory>
#include "error.h"

namespace cx{

#define MAX_INPUT_BUFFER_SIZE 1024

	class list_buffer;

	extern const char EOF_CHAR;
	extern int input_position;
	extern int list_flag;
	extern int level;
	extern list_buffer list;
	extern int current_line_number;
	extern int current_nesting_level;

	///  text_in_buffer       Abstract text input buffer class.

	class text_in_buffer {
	protected:
		std::ifstream file; // input text file
		std::string file_name_; // ptr to the file name
		char text[MAX_INPUT_BUFFER_SIZE]; // input text buffer
		char *p_char; /* ptr to the current char
					   * in the text buffer */

		virtual char get_line(void) = 0;

	public:
		text_in_buffer(std::string input_file_name, abort_code ac);
		virtual ~text_in_buffer(void) {	file.close();}
		const char *file_name(void) { return file_name_.c_str(); }
		char current_char(void) {return *p_char;}
		char get_char(void);
		char put_back_char(void);
	};

	///  cx_source_buffer       Source buffer subclass of cx_text_in_buffer.

	class source_buffer : public text_in_buffer {
		virtual char get_line(void);

	public:
		source_buffer(std::string source_file_name);
	};

	///  cx_text_out_buffer      Abstract text output buffer class.

	class text_out_buffer {
	public:

		char text[MAX_INPUT_BUFFER_SIZE + 16]; // output text buffer
		virtual ~text_out_buffer() = 0;
		virtual void put_line(void) = 0;

		void put_line(const char *p_text) {
			strcpy(text, p_text);
			put_line();
		}
	};

	///  cx_list_buffer         List buffer subclass of cx_text_out_buffer.

	class list_buffer : public text_out_buffer {
		std::string source_file_name; // ptr to source file name (for page header)
		int line_count; // count of lines in the current page
	public:

		virtual ~list_buffer(void) {}

		void initialize(const std::string file_name);
		virtual void put_line(void);

		void put_line(const char *p_text) {
			text_out_buffer::put_line(p_text);
		}

		void buffer(const char *p_text, int line_number, int nesting_level) {
			sprintf(text, "%4d %d: %s", line_number, nesting_level, p_text);
		}

		void put_line(const char *p_text, int line_number, int nesting_level) {
			sprintf(text, "%4d %d: %s", line_number, nesting_level, p_text);
			put_line();
		}
	};

	typedef std::shared_ptr<text_in_buffer> text_in_buffer_ptr;
}
