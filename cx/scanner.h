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

#ifndef SCANNER_H
#define SCANNER_H
#pragma once
//#include "misc.h"
#include "buffer.h"
#include "token.h"

namespace cx{
	///  cx_scanner            Abstract scanner class.
	///  cx_char_code           Character codes.

	class scanner {
	protected:

		// Tokens extracted and returned by the scanner.
		word_token word_token;
		number_token number_token;
		string_token string_token;
		char_token char_token;
		special_token special_token;
		eof_token eof_token;
		error_token error_token;

	public:

		virtual ~scanner(void) {}
		virtual token *get(void) = 0;
	};
		///  cx_text_scanner        Text scanner subclass of cx_scanner.

	class text_scanner : public scanner {
		text_in_buffer * const p_text_in_buffer; // ptr to input text buffer to scan
		void skip_whitespace(void);

	public:
		text_scanner(text_in_buffer *p_buffer);

		virtual ~text_scanner(void) {
			delete p_text_in_buffer;
		}

		virtual token *get(void);
	};
}
#endif
