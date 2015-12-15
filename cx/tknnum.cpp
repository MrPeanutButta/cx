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

#include <cstring>
#include <cstdio>
#include <cmath>
#include <sstream>
#include <climits>
#include <cfloat>
#include <cctype>
#include "token.h"

namespace cx{

	/// radix number bases

	enum cx_radix_base {
		rb_DECIMAL = 10,
		rb_BINARY = 2,
		rb_HEXADECIMAL = 16,
		rb_OCTAL = 8
	};

	/*******************
	 *                 *
	 *  Number Tokens  *
	 *                 *
	 *******************/

	/** get         Extract a number token from the source and set
	 *              its value.
	 *
	 * @param buffer : ptr to text input buffer.
	 */
	void number_token::get(text_in_buffer &buffer) {

		cx_real number_value = 0.0; /* value of number ignoring
							   * the decimal point */
		int whole_places = 0; // no. digits before the decimal point
		int decimal_places = 0; // no. digits after  the decimal point
		wchar_t exponent_sign = L'+';
		cx_real e_value = 0.0; // value of number after 'E'
		int exponent = 0; // final value of exponent
		bool saw_dot_dot_Flag = false; // true if encountered '..',

		ch = buffer.current_char();
		ps = string;
		digit_count = 0;
		count_error_flag = false;
		code__ = TC_ERROR;
		type__ = T_INT; /* we don't know what it is yet, but
						   * assume it'll be an integer */

		// assume base 10 radix
		radix = rb_DECIMAL;

		// octal base
		if (ch == L'0') {
			radix = rb_OCTAL;
			ch = buffer.get_char();
			switch (ch) {
			case L'x':
				radix = rb_HEXADECIMAL;
				ch = buffer.get_char();
				break;
			case L'b':
				radix = rb_BINARY;
				ch = buffer.get_char();
				break;
			case L'.':
				radix = rb_DECIMAL;
				ch = buffer.put_back_char();
				break;
			default:
				ch = buffer.put_back_char();
				break;
			}
		}

		/* get the whole part of the number by accumulating
		 * the values of its digits into number_value.  whole_places keeps
		 * track of the number of digits in this part. */
		if (!accumulate_value(buffer, number_value, ERR_INVALID_NUMBER)) return;
		whole_places = digit_count;

		/* If the current character is a dot, then either we have a
		 * fraction part or we are seeing the first character of a '..'
		 * token.  To find out, we must fetch the next__ character. */
		if (ch == '.') {
			ch = buffer.get_char();

			if (ch == '.') {

				/* We have a .. token.  Back up bufferp so that the
				 * token can be extracted next__. */
				saw_dot_dot_Flag = true;
				buffer.put_back_char();
			}
			else {
				type__ = T_DOUBLE;
				*ps++ = '.';

				// We have a fraction part.  Accumulate it into number_value.
				if (!accumulate_value(buffer, number_value,
					ERR_INVALID_FRACTION)) return;
				decimal_places = digit_count - whole_places;
			}
		}

		/* get the exponent part, if any. There cannot be an
		 * exponent part if we already saw the '..' token. */
		if (!saw_dot_dot_Flag && ((ch == 'E') || (ch == 'e'))) {
			type__ = T_DOUBLE;
			*ps++ = ch;
			ch = buffer.get_char();

			// Fetch the exponent's sign, if any.
			if ((ch == '+') || (ch == '-')) {
				*ps++ = exponent_sign = ch;
				ch = buffer.get_char();
			}

			// Accumulate the value of the number after 'E' into e_value.
			digit_count = 0;
			if (!accumulate_value(buffer, e_value,
				ERR_INVALID_EXPONENT)) return;

			if (exponent_sign == '-') e_value = -e_value;
		}

		// Were there too many digits?
		if (count_error_flag) {
			cx_error(ERR_TOO_MANY_DIGITS);
			return;
		}

		/* Calculate and check the final exponent value,
		 * and then use it to adjust the number's value. */
		exponent = int(e_value) - decimal_places;
		if ((exponent + whole_places < DBL_MIN_10_EXP) ||
			(exponent + whole_places > DBL_MAX_10_EXP)) {
			cx_error(ERR_REAL_OUT_OF_RANGE);
			return;
		}
		if (exponent != 0) number_value *= pow(10.0L, exponent);

		// Check and set the numeric value.
		if (type__ == T_INT) {
			if ((number_value < LLONG_MIN) || (number_value > LLONG_MAX)){
				cx_error(ERR_INTEGER_OUT_OF_RANGE);
				return;
			}
			value__.i_ = static_cast<cx_int>(number_value);
		}
		else {	// TODO Range check for max double
			if ((number_value < -DBL_MAX) || (number_value > DBL_MAX)){
				cx_error(ERR_REAL_OUT_OF_RANGE);
				return;
			}
			value__.d_ = number_value;
		}

		*ps = '\0';
		code__ = TC_NUMBER;
	}

	/** accumulate_value     Extract a number part from the source
	 *                      and set its value.
	 *
	 * @param buffer : ptr to text input buffer.
	 * @param value  : accumulated value (from one or more calls).
	 * @param ec     : error code if failure.
	 * @return true  if success false if failure.
	 */
	int number_token::accumulate_value(text_in_buffer &buffer,
		cx_real &value, error_code ec) {

		const int max_digit_count = 20;

		/* cx_error if the first character is not a digit
		 * and radix base is not hex */
		if ((char_map[ch] != CC_DIGIT) && (!isxdigit(static_cast<int>(ch)))) {
			cx_error(ec);
			return false; // failure
		}

		/* Accumulate the value as long as the total allowable
		 * number of digits has not been exceeded. */
		do {

			*ps++ = ch;

			if (++digit_count <= max_digit_count) {
				value = radix * value + char_value(ch); // shift left__ and add
			}
			else count_error_flag = true; // too many digits

			ch = buffer.get_char();

		} while ((char_map[ch] == CC_DIGIT) || isxdigit(ch));

		return true; // success
	}

	int number_token::char_value(const wchar_t &c) {
		if (isxdigit(c)) {
			switch (c) {
			case L'A':
			case L'a': return 10;
			case L'B':
			case L'b': return 11;
			case L'C':
			case L'c': return 12;
			case L'D':
			case L'd': return 13;
			case L'E':
			case L'e': return 14;
			case L'F':
			case L'f': return 15;
			}
		}

		return (c - L'0');
	}
}