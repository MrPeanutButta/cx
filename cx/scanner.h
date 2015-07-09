#ifndef SCANNER_H
#define SCANNER_H
#pragma once
#include "misc.h"
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
		text_in_buffer * const p_text_in_buffer; /* ptr to input text buffer
													 * to scan */

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
